/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libdui.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "qtmaemo6style.h"
#include "qtmaemo6style_p.h"

#include <QAbstractButton>
#include <QPainter>
#include <QDebug>
#include <QStyleOptionButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QInputContext>
#include <QInputContextFactory>
#include <QPoint>
#include <QToolBar>
#include <QMainWindow>
#include <QDialog>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QMoveEvent>
#include <QSequentialAnimationGroup>
#include <QPixmapCache>
#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QAbstractItemView>
#include <QStatusBar>
#include <QMenuBar>

#include <DuiComponentData>
#include <DuiTheme>
#include <DuiScalableImage>
#include <DuiLabel>
#include <duibuttonstyle.h>
#include <duiapplicationpagestyle.h>
#include <duipositionindicatorstyle.h>
#include <duiprogressindicatorstyle.h>
#include <duipannablewidgetstyle.h>
#include <duiwidgetstyle.h>
#include <duidialogstyle.h>
#include <duibuttoniconstyle.h>
#include <duicheckboxstyle.h>
#include <duicontainerstyle.h>
#include <duicomboboxstyle.h>
#include <duilabelstyle.h>
#include <duiimagewidgetstyle.h>
#include <duinavigationbarstyle.h>
#include <duiseparatorstyle.h>
#include <duisliderstyle.h>
#include <duitexteditstyle.h>
#include <duicontentitemstyle.h>
#include <duiapplicationmenustyle.h>
#include <duifeedbackplayer.h>
#include <duiwidgetfadeinanimationstyle.h>
#include <duiwidgetfadeoutanimationstyle.h>
#include <duideviceprofile.h>

#include "qtmaemo6pangesture.h"
#include "qtmaemo6titlebar.h"
#include "qtmaemo6dialogtitle.h"
#include "qtmaemo6windowdecoration.h"
#include "qtmaemo6dialogproxy.h"
#include "qtmaemo6menu.h"

//#include "duicontainerheader_p.h"

#define WIDGET_OPACITY "widgetOpacity"
#define SCROLLAREATHUMBVIEW "scrollAreaThumbView"

//#define MOVE_ACTIONS_FROM_TOOLBAR_TO_TITLEBAR

QPixmap setPixmapOpacity(const QPixmap &pixmap, double opacity)
{
    //opacity == 1 makes no transparency, so let the pixmap unchanged
    if (opacity >= 1.0)
        return pixmap;

    QPixmap transparentPixmap(pixmap.size());
    transparentPixmap.fill(Qt::transparent);

    //if the opacity is 0, just return a fully transparent pixmap
    if (opacity >= 0.0) {
        QPainter painter(&transparentPixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(0, 0, pixmap);

        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.fillRect(transparentPixmap.rect(), QColor(0, 0, 0, 255 * opacity));
        painter.end();
    }
    return transparentPixmap;
}

QtMaemo6StylePrivate::QtMaemo6StylePrivate()
    : m_actionsInTitleBarCount(5),
      m_componentData(0),
      m_isDuiInitialized(false),
      m_isDuiApplication(false),
      m_scrollBarEventFilter(0),
      m_panGestureScrolling(0),
      m_menuBar(0)
{
}

QtMaemo6StylePrivate::~QtMaemo6StylePrivate()
{
    if (!m_isDuiApplication) {
        delete m_componentData;
    }
}

void QtMaemo6StylePrivate::initDui()
{
    Q_Q(QtMaemo6Style);

    m_isDuiInitialized = true;

    QStringList args = qApp->arguments();

    int argc = 1;
    char *argv[ 1 ];
    argv[ 0 ] = 0;

    if (! args.isEmpty()) {
        //FIXME: using QString would be nicer
        argv[ 0 ] = strndup(args[ 0 ].toLocal8Bit().constData(), 42);

        qDebug("appName: %s", argv[ 0 ]);
    }

    if (DuiComponentData::instance() != 0) {
        m_isDuiApplication = true;
    } else {
        m_componentData = new DuiComponentData(argc, argv);
    }

    m_windowEventFilter = new QtMaemo6StyleEventFilter(q);
    m_panGestureScrolling = new QtMaemo6PanGesture(q);
    m_scrollBarEventFilter = new QtMaemo6ScrollBarEventFilter(q);
    m_scrollBarEventFilter->setScrollBarsAlwaysVisible(false);
    m_panGestureScrolling = new QtMaemo6PanGesture(q);

    const DuiLabelStyle *style =
        static_cast<const DuiLabelStyle *>(duiStyle(QStyle::State_Active,
                                           "DuiLabelStyle"));
    qApp->setFont(style->font());
    qApp->setGlobalStrut(QSize(0, 0));
    qApp->setInputContext(QInputContextFactory::create("DuiInputContext", qApp));
}

const DuiStyle *QtMaemo6StylePrivate::duiStyle(QStyle::State state,
        const QString &styleClass,
        const QString &styleObject,
        const QString &type,
        const DuiWidgetController *parent
                                              )
{
    // Set mode
    QString mode = modeFromState(state);

    return DuiTheme::style(styleClass.toLocal8Bit().constData(),
                           styleObject.toLocal8Bit().constData(),
                           mode, type, Dui::Landscape, parent);
}

QString QtMaemo6StylePrivate::modeFromState(QStyle::State state)
{
    QString mode;
    if (state & QStyle::State_Enabled) {
        if (state & QStyle::State_Active)
            mode = "active";
        if (state & QStyle::State_Sunken || state & QStyle::State_On
                || state & QStyle::State_Selected)
            mode = "selected";
    } else {
        mode = "disabled";
    }
    return mode;
}

void QtMaemo6StylePrivate::drawWindowBackground(QWidget *widget)
{
    if (NULL != widget) {
        QStyleOption widgetOption;
        widgetOption.initFrom(widget);

        QPixmap backgroundPixmap(widget->size());
        backgroundPixmap.fill(Qt::transparent);
        QPainter painter;
        painter.begin(&backgroundPixmap);

        if (qobject_cast<QDialog *>(widget)) {
            const DuiPannableWidgetStyle *style =
                static_cast<const DuiPannableWidgetStyle *>(
                    QtMaemo6StylePrivate::duiStyle(widgetOption.state, "DuiPannableWidgetStyle", "DuiDialogContentsViewport"));
            QtMaemo6StylePrivate::drawWidgetBackground(&painter, &widgetOption, backgroundPixmap.rect(), style);
        } else {
            const DuiApplicationPageStyle *style =
                static_cast<const DuiApplicationPageStyle *>(
                    QtMaemo6StylePrivate::duiStyle(widgetOption.state, "DuiApplicationPageStyle", ""));
            QtMaemo6StylePrivate::drawWidgetBackground(&painter, &widgetOption, backgroundPixmap.rect(), style);
        }

        painter.end();

        QPalette palette;
        palette.setBrush(widget->backgroundRole(), QBrush(backgroundPixmap));
        widget->setPalette(palette);
    }
}

void QtMaemo6StylePrivate::drawWidgetBackground(QPainter *p,
        const QStyleOption *option,
        const QRect &rect,
        const DuiWidgetStyle *style)
{
    if (style && style->backgroundImage()) {
        drawScalableImage(p, option, rect, style->backgroundImage(), style, "bg");
    }
}

void QtMaemo6StylePrivate::drawScalableImage(QPainter *p,
        const QStyleOption *option,
        const QRect &rect,
        const DuiScalableImage *scalableImage,
        const DuiWidgetStyle *style,
        const QString &purpose,
        bool enableCache)
{
    if (style) {
        // draw Background
        qreal effectiveOpacity = p->opacity();
        p->setOpacity(style->backgroundOpacity() * effectiveOpacity);

        // This is a cheap hack to enfore synchronous loading
        // FIXME: Implement dynamic updating for asynchronous loading
        // to get rid of this.
        while (DuiTheme::hasPendingRequests()) {
            usleep(10000);
            QCoreApplication::processEvents();
        }

        if (scalableImage) {

            // Per Widget background image cache implementation
            QPixmap backgroundPixmap(rect.size());

            QPaintDevice *device = p->device();
            QWidget *cachedWidget = dynamic_cast<QWidget *>(device);

            QString mode;
            if (cachedWidget) {
                mode = QtMaemo6StylePrivate::modeFromState(option->state);
            }

            quintptr pWidget = reinterpret_cast<quintptr>(device);
            quintptr pImage = reinterpret_cast<quintptr>(scalableImage);

            QString cacheKey = QString("%1_2%_%3_%4_%5x%6").arg(pWidget)
                               .arg(pImage)
                               .arg(mode)
                               .arg(purpose)
                               .arg(rect.width())
                               .arg(rect.height());

            if (!enableCache || !QPixmapCache::find(cacheKey, backgroundPixmap)) {
                backgroundPixmap.fill(Qt::transparent);
                QPainter pixmapPainter;
                pixmapPainter.begin(&backgroundPixmap);
                scalableImage->draw(0, 0, rect.width(), rect.height(), &pixmapPainter);
                pixmapPainter.end();
                QPixmapCache::insert(cacheKey, backgroundPixmap);
            }

            p->drawPixmap(rect, backgroundPixmap);
        } else if (style->backgroundColor().isValid()) {
            p->fillRect(rect, QBrush(style->backgroundColor()));
        }
        p->setOpacity(effectiveOpacity);
    }
}

void QtMaemo6StylePrivate::drawSliderBaseBackground(QPainter *p,
        const QStyleOption *option,
        const QRect &rect,
        const DuiSliderStyle *style,
        int maxSliderLength)
{
    Q_UNUSED(maxSliderLength);

    p->save();
    if (style) {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            // draw Background
            qreal effectiveOpacity = p->opacity();
            p->setOpacity(style->backgroundOpacity() * effectiveOpacity);

            bool isHorizontal = slider->orientation == Qt::Horizontal;

            //unsued
            //int span = ( isHorizontal )
            //            ? slider->rect.width() - maxSliderLength
            //            : slider->rect.height() - maxSliderLength;

            //unused
            //int handlePos = sliderPositionFromValue(slider->minimum,
            //                                        slider->maximum,
            //                                        slider->sliderPosition,
            //                                        span,
            //                                        slider->upsideDown);

            const DuiScalableImage *baseImage = (isHorizontal)
                                                ? style->backgroundBaseImage()
                                                : style->backgroundVerticalBaseImage();

            if (baseImage) {
                // Per Widget background image cache implementation
                QPixmap backgroundPixmap(rect.size());

                QPaintDevice *device = p->device();
                QWidget *cachedWidget = dynamic_cast<QWidget *>(device);

                QString mode;
                if (cachedWidget) {
                    mode = QtMaemo6StylePrivate::modeFromState(option->state);
                }

                quintptr pWidget = reinterpret_cast<quintptr>(device);

                QString cacheKey = QString("%1_%2_%3x%4_base").arg(pWidget)
                                   .arg(mode)
                                   .arg(rect.width())
                                   .arg(rect.height());

                if (!QPixmapCache::find(cacheKey, backgroundPixmap)) {
                    backgroundPixmap.fill(Qt::transparent);
                    QPainter pixmapPainter;
                    pixmapPainter.begin(&backgroundPixmap);
                    baseImage->draw(0, 0, rect.width(), rect.height(), &pixmapPainter);
                    pixmapPainter.end();
                    QPixmapCache::insert(cacheKey, backgroundPixmap);
                }
                p->drawPixmap(rect, backgroundPixmap);
            } else if (style->backgroundColor().isValid()) {
                p->fillRect(rect, QBrush(style->backgroundColor()));
            }

            p->setOpacity(effectiveOpacity);
        }
    }
    p->restore();
}

void QtMaemo6StylePrivate::drawBasicButton(QPainter *painter,
        const QString &text,
        const QIcon &icon,
        const QRect &rect,
        const QStyleOption *option,
        const DuiButtonStyle *style) const
{
    drawBasicButton(painter, text, icon, rect, option, style, style->font(), style->iconSize());
}

void QtMaemo6StylePrivate::drawBasicButton(QPainter *p,
        const QString &text,
        const QIcon &icon,
        const QRect &rect,
        const QStyleOption *option,
        const QString &styleClass,
        const QString &styleObject /*= QString()*/) const
{
    const DuiButtonStyle *style =
        static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(option->state,
                                            styleClass.toLocal8Bit().constData(),
                                            styleObject));
    drawBasicButton(p, text, icon, rect, option, style, style->font(), style->iconSize());

}

void QtMaemo6StylePrivate::drawToggleButton(QPainter *p,
        const QString &text,
        const QIcon &icon,
        const QRect &rect,
        const QStyleOption *option,
        const QString &styleClass,
        const QString &styleObject /*= QString()*/) const
{
    const DuiButtonStyle *style =
        static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(option->state,
                                            styleClass.toLocal8Bit().constData(),
                                            styleObject,
                                            "toggle"));
    drawBasicButton(p, text, icon, rect, option, style, style->font(), style->iconSize());
}

void QtMaemo6StylePrivate::drawCheckBox(QPainter *p,
                                        const QString &text,
                                        const QIcon &icon,
                                        const QRect &rect,
                                        const QStyleOption *option,
                                        const QString &styleClass,
                                        const QString &styleObject /*= QString()*/) const
{
    const DuiCheckboxStyle *style =
        static_cast<const DuiCheckboxStyle *>(QtMaemo6StylePrivate::duiStyle(option->state,
                styleClass.toLocal8Bit().constData(),
                styleObject,
                "checkbox"));
    // TODO: Fix regression due to introduction of DuiCheckBoxStyle
    drawBasicButton(p, text, icon, rect, option, style, style->font(), style->iconSize());
}

void QtMaemo6StylePrivate::drawBasicButton(QPainter *p,
        const QString &text,
        const QIcon &icon,
        const QRect &rect,
        const QStyleOption *option,
        const DuiButtonStyle *style,
        const QFont &font,
        const QSize &iconSize) const
{
    if (style) {
        // draw Background
        QtMaemo6StylePrivate::drawWidgetBackground(p, option, rect, style);

        QSize usedIconSize = iconSize.isValid() ? iconSize : style->iconSize();

        QRect textAndIconRect = getTextAndIconRect(style, text, icon, font, usedIconSize);

        //there is only an icon
        if (!icon.isNull() && text.isEmpty()) {
            //If the iconsize exceeds the widget's size (maybe fixed- or maximumSize set)
            // scale the icon to the widget size. This is only a fallback solution
            int paddingLeft, paddingTop, paddingRight, paddingBottom;
            paddingFromStyle(style, &paddingLeft, &paddingTop, &paddingRight, &paddingBottom);

            if (usedIconSize.width() > option->rect.width()
                    || usedIconSize.height() > option->rect.height()) {
                QSize maxIconSize = option->rect.size() -
                                    QSize(paddingLeft + paddingRight,
                                          paddingTop + paddingBottom);
                usedIconSize.scale(maxIconSize, Qt::KeepAspectRatio);
            }
            textAndIconRect = option->rect;
        } else {
            //TODO: is always centered
            textAndIconRect.moveTo(
                option->rect.left() + (rect.width() - textAndIconRect.width()) / 2,
                option->rect.top() + (rect.height() - textAndIconRect.height()) / 2);
        }

        if (!icon.isNull()) {
            drawButtonIcon(style, p, textAndIconRect, icon, usedIconSize);

            //merge the alignment of icon and text
            // if the button is aligned horizontaly to the text, text must be drawn with inverted
            // icon's horizontal align and horizontal align of the text
            if (hasVerticalAlignment(style->iconAlign())) {
                drawButtonText(style, p, textAndIconRect, text,
                               invertAlignment(verticalAlignment(style->iconAlign())) | style->horizontalTextAlign(), font);
            } else if (hasHorizontalAlignment(style->iconAlign())) {
                drawButtonText(style, p, textAndIconRect, text,
                               invertAlignment(horizontalAlignment(style->iconAlign())) | style->verticalTextAlign(), font);
            } else {
                qDebug("QtMaemo6Style: Button has no text align");
            }
        } else { //Text only
            drawButtonText(style, p, textAndIconRect, text, style->horizontalTextAlign() | style->verticalTextAlign(), font);
        }
    }
}

void QtMaemo6StylePrivate::drawButtonText(const DuiButtonStyle *style,
        QPainter *painter,
        const QRectF &textRect,
        const QString &text,
        Qt::Alignment align,
        const QFont &font) const
{
    if (text.isEmpty())
        return;

    // update text area by margin
    int marginLeft = style->textMarginLeft();
    int marginRight = style->textMarginRight();
    int marginTop = style->textMarginTop();
    int marginBottom = style->textMarginBottom();

    QRectF rect;
    rect.setX(textRect.x() + marginLeft);
    rect.setY(textRect.y() + marginTop);
    rect.setWidth(textRect.width() - marginLeft - marginRight);
    rect.setHeight(textRect.height() - marginTop - marginBottom);

    int alignment = align | Qt::TextHideMnemonic;

    painter->setFont(font);
    painter->setPen(style->textColor());
    painter->drawText(rect, alignment, text);
}

void QtMaemo6StylePrivate::drawButtonText(const DuiButtonStyle *style,
        QPainter *painter,
        const QRectF &textRect,
        const QString &text,
        Qt::Alignment align) const
{
    drawButtonText(style, painter, textRect, text, align, style->font());
}

void QtMaemo6StylePrivate::drawButtonIcon(const DuiButtonStyle *style,
        QPainter *painter,
        const QRect &contentsRect,
        const QIcon &icon,
        const QSize &iconSize /*= QSize()*/) const
{
    Q_Q(const QtMaemo6Style);
    QSize usedIconSize = iconSize.isValid() ? iconSize : style->iconSize();

    //If alignment is only horizontal oder vertical, center in the other direction
    Qt::Alignment usedAlign = style->iconAlign();
    if (!hasHorizontalAlignment(usedAlign))
        usedAlign |= Qt::AlignHCenter;
    if (!hasVerticalAlignment(usedAlign))
        usedAlign |= Qt::AlignVCenter;

    int paddingLeft, paddingTop, paddingRight, paddingBottom;
    paddingFromStyle(style, &paddingLeft, &paddingTop, &paddingRight, &paddingBottom);

    QRect ctRect = contentsRect.translated(paddingLeft, paddingTop);
    ctRect.setWidth(ctRect.width() - (paddingLeft + paddingRight));
    ctRect.setHeight(ctRect.height() - (paddingTop + paddingBottom));
    q->drawItemPixmap(painter, ctRect, usedAlign, icon.pixmap(usedIconSize));
}

QRect QtMaemo6StylePrivate::getTextAndIconRect(const DuiButtonStyle *style,
        const QString &text,
        const QIcon &icon /*= QIcon()*/,
        const QFont &font /*= QFont()*/,
        const QSize &iconSize /*= QSize()*/) const
{
    QRect textAndIconRect;
    QRect textRect = textBoundingRect(text, font);
    if (!text.isEmpty()) {
        textRect.setWidth(textRect.width() + style->textMarginLeft() + style->textMarginRight());
        textRect.setHeight(textRect.height() + style->textMarginTop() + style->textMarginBottom());
    }
    QSize usedIconSize = iconSize.isValid() ? iconSize : style->iconSize();

    if (hasHorizontalAlignment(style->iconAlign())) {
        if (!icon.isNull()) {
            QRect iconRect = QRect(QPoint(0, 0), usedIconSize);
            //TODO: currently the textMarginLeft is used as space between icon and text
            textAndIconRect = iconRect | textRect.translated(iconRect.width(), 0);
            //if the alignment is left -> calc with textMarginLeft
            //if the alignment is right -> calc with textMarginRight
        } else {
            textAndIconRect = textRect;
        }
    } else {
        if (!icon.isNull()) {
            QRect iconRect = QRect(QPoint(0, 0), usedIconSize);
            //TODO: currently the textMarginLeft is used as space between icon and text
            textAndIconRect = iconRect | textRect.translated(0, iconRect.height());
            //if the alignment is top -> calc with textMarginTop
            //if the alignment is bottom -> calc with textMarginBottom
        } else {
            textAndIconRect = textRect;
        }
    }

    int paddingLeft, paddingTop, paddingRight, paddingBottom;
    paddingFromStyle(style, &paddingLeft, &paddingTop, &paddingRight, &paddingBottom);

    //add margins to the final rect
    textAndIconRect = QRect(textAndIconRect.x(), textAndIconRect.y(),
                            textAndIconRect.width() + paddingLeft + paddingRight,
                            textAndIconRect.height() + paddingTop + paddingBottom);

    return textAndIconRect;
}

QRect QtMaemo6StylePrivate::textBoundingRect(const QString &text, const QFont &font) const
{
    QRect rect = QFontMetrics(font).boundingRect(text);
    return QRect(QPoint(0, 0), QSize(rect.x() + rect.width(), rect.height()));
}

void QtMaemo6StylePrivate::paddingFromStyle(const DuiWidgetStyle *style,
        int *left,
        int *top,
        int *right,
        int *bottom) const
{
    style->backgroundImage()->borders(left, right, top, bottom);
    if (left && style->paddingLeft() > *left)
        *left = style->paddingLeft();
    if (top && style->paddingTop() > *top)
        *top = style->paddingTop();
    if (right && style->paddingRight() > *right)
        *right = style->paddingRight();
    if (bottom && style->paddingBottom() > *bottom)
        *bottom = style->paddingBottom();
}

QRect QtMaemo6StylePrivate::scrollBarSliderRect(const QStyleOptionComplex *option,
        const QWidget *widget /*= 0*/) const
{
    Q_Q(const QtMaemo6Style);
    if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
        int sliderMaxLength = (scrollBar->orientation == Qt::Horizontal) ?
                              scrollBar->rect.width() : scrollBar->rect.height();
        int sliderMinLength = q->proxy()->pixelMetric(QStyle::PM_ScrollBarSliderMin, scrollBar, widget);
        int sliderLength;

        // calculate slider length
        if (scrollBar->maximum != scrollBar->minimum) {
            uint valueRange = scrollBar->maximum - scrollBar->minimum;
            sliderLength = (scrollBar->pageStep * sliderMaxLength) / (valueRange + scrollBar->pageStep);

            if (sliderLength < sliderMinLength || valueRange > INT_MAX / 2)
                sliderLength = sliderMinLength;
            if (sliderLength > sliderMaxLength)
                sliderLength = sliderMaxLength;
        } else {
            sliderLength = sliderMaxLength;
        }

        int sliderStart = q->sliderPositionFromValue(scrollBar->minimum,
                          scrollBar->maximum,
                          scrollBar->sliderPosition,
                          sliderMaxLength - sliderLength,
                          scrollBar->upsideDown);

        QRect scrollBarRect = scrollBar->rect;
        QRect rect;
        if (scrollBar->orientation == Qt::Horizontal) {
            rect.setRect(sliderStart, 0, sliderLength, scrollBarRect.height());
        } else {
            rect.setRect(0, sliderStart, scrollBarRect.width(), sliderLength);
        }

        return q->visualRect(scrollBar->direction, scrollBarRect, rect);
    }
    return QRect();
}

Qt::Alignment QtMaemo6StylePrivate::invertAlignment(Qt::Alignment align) const
{
    Qt::Alignment retAlign;
    //invert horizontal alignment
    if (align.testFlag(Qt::AlignLeft))
        retAlign = Qt::AlignRight;
    else if (align.testFlag(Qt::AlignRight))
        retAlign = Qt::AlignLeft;
    else if (align.testFlag(Qt::AlignHCenter))
        retAlign = Qt::AlignHCenter;

    //invert vertical alignment
    if (align.testFlag(Qt::AlignTop))
        retAlign |= Qt::AlignBottom;
    else if (align.testFlag(Qt::AlignBottom))
        retAlign = Qt::AlignTop;
    else if (align.testFlag(Qt::AlignVCenter))
        retAlign = Qt::AlignVCenter;

    return retAlign;
}


QtMaemo6Style::QtMaemo6Style()
    : QtMaemo6TestStyle(*new QtMaemo6StylePrivate)
{
    init();
}

QtMaemo6Style::~QtMaemo6Style()
{

}

void QtMaemo6Style::init()
{
//    QPixmapCache::setCacheLimit( 1000 );
}




void QtMaemo6Style::polish(QApplication *app)
{
    QtMaemo6TestStyle::polish(app);
}

void QtMaemo6Style::polish(QWidget *widget)
{
    Q_D(QtMaemo6Style);
    // Lazy initialization of the DuiFramework.
    // This is needed to guarantee that actual DuiApplications will work as well.
    if (!d->m_isDuiInitialized) {
        d->initDui();
    }

    //Ensure that the widget draws its background transparent
    if (!qobject_cast<QtMaemo6WindowDecoration *>(widget)
            && !qobject_cast<QMenu *>(widget)
            && !qobject_cast<QtMaemo6DialogProxy *>(widget)) {
        //FIXME: public API usage
        QPalette pal = widget->palette();
        pal.setBrush(QPalette::Window, Qt::transparent);
        widget->setPalette(pal);
    }

    widget->installEventFilter(d->m_windowEventFilter);

#ifdef MOVE_ACTIONS_FROM_TOOLBAR_TO_TITLEBAR
    if (QtMaemo6TitleBar *titleBar = qobject_cast<QtMaemo6TitleBar *>(widget)) {
        foreach(QAction * action, d->m_toolBarActions) {
            titleBar->addAction(action);
        }
    }

    QToolBar *toolBar = qobject_cast<QToolBar *>(widget);
    if (toolBar && qobject_cast<QMainWindow *>(widget->parent())) {
        foreach(QAction * action, toolBar->actions()) {
            if (d->m_toolBarActions.count() < d->m_actionsInTitleBarCount) {
                d->m_toolBarActions.append(action);
                toolBar->removeAction(action);
            } else
                break;
        }
        if (toolBar->actions().isEmpty()) {
            QMainWindow *mw = qobject_cast<QMainWindow *>(widget->parent());
            mw->removeToolBar(toolBar);
        }
    }
#endif

    if (qobject_cast<QScrollBar *>(widget)) {
        //FIXME: public API usage
        widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
        d->m_scrollBarEventFilter->enableOn(widget);
    }

    if (QAbstractScrollArea *abstractScrollArea = qobject_cast<QAbstractScrollArea *>(widget)) {
        d->m_panGestureScrolling->enableOn(abstractScrollArea);
        d->m_scrollBarEventFilter->enableOn(abstractScrollArea);
        //FIXME: public API usage
        //widget->setAutoFillBackground(false);

        //must use inherits() because it's an private Qt class
        if (!abstractScrollArea->inherits("QComboBoxListView")) {
            //don't do this for QComboBoxListView, because it uses the QItemDelegate
            //which not uses the style
            QPalette pal = abstractScrollArea->viewport()->palette();
            pal.setBrush(QPalette::Base, Qt::transparent);
            abstractScrollArea->viewport()->setPalette(pal);
        } else {
            //this must be set for ComboBox DropDowns, don't know why!
            QPalette pal = abstractScrollArea->viewport()->palette();
            pal.setBrush(QPalette::Base, Qt::white);
            abstractScrollArea->viewport()->setPalette(pal);
        }
    }

    if (QStatusBar *statusBar = qobject_cast<QStatusBar *>(widget)) {
        //only use statusbars directly from MainWindow
        if (qobject_cast<QMainWindow *>(statusBar->parent())) {
            //FIXME: add handling here for not showing empty statusbars
            d->m_statusBar = statusBar;
        }
        //FIXME: Public API usage
        statusBar->setSizeGripEnabled(false);
    }

    if (QMenuBar *menuBar = qobject_cast<QMenuBar *>(widget)) {
        //Only take the menubar from the main window
        //TODO: this doesn't work for applications that use shared menuBars, like eg. some
        //      mac applications do.
        if (menuBar->parent()) {
            if (qobject_cast<QMainWindow *>(menuBar->parent())) {
                d->m_menuBar = menuBar;
                //do not remove the menuBar from the mainWindow, it may be changed
                //by the user
            }
        }
    }

    if (QtMaemo6WindowDecoration *decoration = qobject_cast<QtMaemo6WindowDecoration *>(widget)) {
        if (d->m_statusBar)
            decoration->setStatusBar(d->m_statusBar);
        if (d->m_menuBar)
            decoration->setMenuBar(d->m_menuBar);
    }
}

void QtMaemo6Style::drawPrimitive(PrimitiveElement element,
                                  const QStyleOption *option,
                                  QPainter *painter,
                                  const QWidget *widget) const
{
    Q_D(const QtMaemo6Style);

    switch (element) {
    case PE_Widget: {
        if (qobject_cast<const QtMaemo6TitleBar *>(widget)) {
            const DuiNavigationBarStyle *style =
                static_cast<const DuiNavigationBarStyle *>(QtMaemo6StylePrivate::duiStyle(option->state,
                        "DuiNavigationBarStyle"));
            // draw widget background
            d->drawWidgetBackground(painter, option, widget->rect(), style);

        }
        if (qobject_cast<const QtMaemo6DialogTitle *>(widget)) {
            const DuiWidgetStyle *style =
                static_cast<const DuiWidgetStyle *>(
                    QtMaemo6StylePrivate::duiStyle(option->state, "DuiWidgetStyle", "DuiDialogTitleBar"));
            // draw widget background
            d->drawWidgetBackground(painter, option, widget->rect(), style);

        }
        if (qobject_cast<const QtMaemo6Menu *>(widget)) {
            const DuiApplicationMenuStyle *style =
                static_cast<const DuiApplicationMenuStyle *>(
                    QtMaemo6StylePrivate::duiStyle(option->state, "DuiApplicationMenuStyle"));
            // draw widget background
            d->drawScalableImage(painter, option, widget->rect(), style->canvasImage(), style, "canvas-image");
            //d->drawWidgetBackground( painter, option, widget->rect(), style );
        }
    }
    break;

    case PE_PanelLineEdit: {
        if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(option)) {

            const DuiTextEditStyle *style =
                static_cast<const DuiTextEditStyle *>(QtMaemo6StylePrivate::duiStyle(panel->state,
                        "DuiTextEditStyle"));
            QtMaemo6StylePrivate::drawWidgetBackground(painter, option, panel->rect, style);
        }
    }
    break;
    case PE_PanelItemViewItem: { //draw the background of ItemViewItems
        if (const QStyleOptionViewItemV4 *item = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option)) {
            const DuiContentItemStyle *style =
                static_cast<const DuiContentItemStyle *>(QtMaemo6StylePrivate::duiStyle(item->state,
                        "DuiContentItemStyle"));
            const QAbstractItemModel *model = item->index.model();

            //it's a simple Listview
            if (model->columnCount() == 1) {
                if (model->rowCount() == 1) {
                    d->drawScalableImage(painter, option, item->rect, style->backgroundImageSingle(), style);
                } else if (item->index.row() == 0) {
                    d->drawScalableImage(painter, option, item->rect, style->backgroundImageSinglecolumnTop(), style);
                } else if (item->index.row() == model->rowCount() - 1) {
                    d->drawScalableImage(painter, option, item->rect, style->backgroundImageSinglecolumnBottom(), style);
                } else {
                    d->drawScalableImage(painter, option, item->rect, style->backgroundImageSinglecolumnCenter(), style);
                }
            } else {
                //TODO: Add implementation for tableviews here
                QtMaemo6TestStyle::drawPrimitive(element, option, painter, widget);
                if (model->rowCount() == 1) {
                    if (item->index.column() == 0) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageSinglerowLeft(), style);
                    } else if (item->index.column() == model->columnCount() - 1) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageSinglerowRight(), style);
                    } else {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageSinglerowCenter(), style);
                    }
                } else if (item->index.row() == 0) {
                    if (item->index.column() == 0) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageTopLeft(), style);
                    } else if (item->index.column() == model->columnCount() - 1) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageTopRight(), style);
                    } else {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageTop(), style);
                    }
                } else if (item->index.row() == model->rowCount() - 1) {
                    if (item->index.column() == 0) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageBottomLeft(), style);
                    } else if (item->index.column() == model->columnCount() - 1) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageBottomRight(), style);
                    } else {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageBottom(), style);
                    }
                } else {
                    if (item->index.column() == 0) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageLeft(), style);
                    } else if (item->index.column() == model->columnCount() - 1) {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageRight(), style);
                    } else {
                        d->drawScalableImage(painter, option, item->rect, style->backgroundImageCenter(), style);
                    }
                }
            }

        }
    }
    break;
    case PE_IndicatorBranch: {
        QtMaemo6TestStyle::drawPrimitive(element, option, painter, widget);
        if (option->state.testFlag(QStyle::State_Children)) {
            QToolButton button;
            QStyleOptionToolButton buttonOption;
            buttonOption.initFrom(&button);
            buttonOption.rect = option->rect;
            if (option->state.testFlag(QStyle::State_Open)) {
                buttonOption.text = '-';
            } else {
                buttonOption.text = '+';
            }

            QSize size = sizeFromContents(QStyle::CT_ToolButton, &buttonOption, QSize(), &button);

            QRect buttonRect = QRect(option->rect.left() + (option->rect.width() - size.width()) / 2,
                                     option->rect.top() + (option->rect.height() - size.height()) / 2,
                                     size.width(), size.height());
            buttonOption.rect = buttonRect;

            drawComplexControl(QStyle::CC_ToolButton, &buttonOption, painter, &button);
        }
    }
    break;
    case PE_Frame: {
        //don't draw the frame for item views, this is done by the items themselfs
        if (qobject_cast<const QAbstractItemView *>(widget)) {
        } else
            QtMaemo6TestStyle::drawPrimitive(element, option, painter, widget);
    }
    break;
    case PE_FrameFocusRect: {
        //The focus rect of selected items in AbstractItemViews
        //don't draw anything here
    }
    default: {
        QtMaemo6TestStyle::drawPrimitive(element, option, painter, widget);
    }
    break;
    }

}


void QtMaemo6Style::drawControl(ControlElement element,
                                const QStyleOption *opt,
                                QPainter *p,
                                const QWidget *widget) const
{
    Q_D(const QtMaemo6Style);

    switch (element) {
    case CE_ComboBoxLabel: {
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {

            QStyleOptionComboBox subopt = *cmb;
            subopt.palette = QPalette(Qt::black);

            //const DuiComboBoxStyle * style =
            //    static_cast<const DuiComboBoxStyle *>( QtMaemo6StylePrivate::duiStyle( cmb->state,
            //                                           "DuiComboBoxStyle", "DuiComboBoxTitle" ) );
// Not implemented yet on the DUI side
            /*
                            p->setFont( style->font() );
                            p->setPen( QPen( style->color() ) );
             */
            QtMaemo6TestStyle::drawControl(element, cmb, p, widget);
        }
    }
    break;
    case CE_PushButton: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {

            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(SE_PushButtonContents, btn, widget);

            const QAbstractButton *buttonWidget = qobject_cast<const QAbstractButton * >(widget);
            if (buttonWidget->isCheckable())
                d->drawToggleButton(p, subopt.text, subopt.icon, subopt.rect, opt, "DuiButtonIconStyle");
            else
                d->drawBasicButton(p, subopt.text, subopt.icon, subopt.rect, opt, "DuiButtonIconStyle");
        }
    }
    break;

    case CE_ProgressBar: {
        if (const QStyleOptionProgressBar * pb
                = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            QStyleOptionProgressBarV2 subopt = *pb;
            subopt.rect = subElementRect(SE_ProgressBarGroove, pb, widget);
            proxy()->drawControl(CE_ProgressBarGroove, &subopt, p, widget);
            subopt.rect = subElementRect(SE_ProgressBarContents, pb, widget);
            proxy()->drawControl(CE_ProgressBarContents, &subopt, p, widget);
        }
    }
    break;

    case CE_RadioButton:
    case CE_CheckBox: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {

            bool isCheckBox = (element == CE_CheckBox);

            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(isCheckBox ? SE_CheckBoxIndicator
                                         : SE_RadioButtonIndicator,
                                         btn, widget);

            d->drawCheckBox(p, QString(), subopt.icon, subopt.rect, opt, "DuiButtonStyle");

            subopt.rect = subElementRect(isCheckBox ? SE_CheckBoxContents
                                         : SE_RadioButtonContents,
                                         btn, widget);

            proxy()->drawControl(isCheckBox ? CE_CheckBoxLabel
                                 : CE_RadioButtonLabel,
                                 &subopt, p, widget);
        }
        break;
    }
    case CE_TabBarTabShape: {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {

            QStyleOptionButton btn;
            btn.initFrom(widget);
            btn.rect = tab->rect;
            btn.state = tab->state;

            const DuiButtonStyle *style =
                static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(opt->state,
                                                    "DuiButtonIconStyle"));
            // Don't show the edge of the button that is closest to the
            // TabBarBase.
            // FIXME: Solve for other tab bar orientations
            if (style) {
                int left, right, top, bottom;
                style->backgroundImage()->borders(&left, &right, &top, &bottom);

                qreal cutOff = 0;
                qreal origSize = 0;

                const QPixmap *pixmap = style->backgroundImage()->pixmap();
                if (pixmap) {
                    int buttonHeight = btn.rect.height();
                    int buttonWidth = btn.rect.width();

                    switch (tab->shape) {
                    case QTabBar::RoundedNorth:
                    case QTabBar::TriangularNorth:
                        cutOff = bottom;
                        origSize = pixmap->height();
                        btn.rect.setHeight(buttonHeight * origSize / (origSize - cutOff));
                        break;

                    case QTabBar::RoundedSouth:
                    case QTabBar::TriangularSouth:
                        cutOff = top;
                        origSize = pixmap->height();
                        btn.rect.setTop(-buttonHeight * cutOff / origSize);
                        btn.rect.setHeight(buttonHeight * origSize / (origSize - cutOff));
                        break;

                    case QTabBar::RoundedWest:
                    case QTabBar::TriangularWest:
                        cutOff = right;
                        origSize = pixmap->width();
                        btn.rect.setWidth(buttonWidth * origSize / (origSize - cutOff));
                        break;

                    case QTabBar::RoundedEast:
                    case QTabBar::TriangularEast:
                        cutOff = left;
                        origSize = pixmap->width();
                        btn.rect.setLeft(-buttonWidth * cutOff / origSize);
                        btn.rect.setWidth(buttonWidth * origSize / (origSize - cutOff));
                        break;

                    default:
                        break;
                    }
                }
            }

            d->drawBasicButton(p,
                               QString() /*tab->text*/,
                               QIcon() /*tab->icon*/,
                               btn.rect, &btn, "DuiButtonIconStyle");
        }
        break;
    }
    case CE_ToolBar: {
        //const QToolBar* toolBar = qobject_cast<const QToolBar*>(widget);
        if (widget->actions().count() > 0)
            QtMaemo6TestStyle::drawControl(element, opt, p, widget);
        break;
    }
    case CE_ProgressBarGroove: {
        if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            const DuiProgressIndicatorStyle *style =
                static_cast<const DuiProgressIndicatorStyle *>(QtMaemo6StylePrivate::duiStyle(bar->state,
                        "DuiProgressIndicatorStyle", QString(), "bar"));
            if (style && style->inactiveImage())
                d->drawScalableImage(p, opt, bar->rect, style->inactiveImage(), style, "groove");
        }
        break;
    }
    case CE_ProgressBarContents: {

        if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            const DuiProgressIndicatorStyle *style =
                static_cast<const DuiProgressIndicatorStyle *>(QtMaemo6StylePrivate::duiStyle(bar->state,
                        "DuiProgressIndicatorStyle", QString(), "bar"));
            if (style && style->activeImage()) {

                bool reverse = qApp->isRightToLeft();

                QRect r(bar->rect);

                int left, right;
                style->activeImage()->borders(&left, &right, NULL, NULL);

                qreal position = (qreal)(bar->progress - bar->minimum) / (qreal)(bar->maximum - bar->minimum);
                qreal distance = position * (qreal) r.width();

                // need to draw in 1 or 2 parts, depending if the indicator element goes across the ends
                // one draw call
                if (!reverse) {
                    r.setLeft(0);
                    r.setRight(distance);
                } else {
                    r.setRight(r.width());
                    r.setLeft(r.width() - distance);
                }
                d->drawScalableImage(p, opt, r, style->activeImage(), style, "contents", false);
            }
        }
        break;
    }
    case CE_ScrollBarAddLine:
    case CE_ScrollBarSubLine:
    case CE_ScrollBarAddPage:
    case CE_ScrollBarSubPage:
    case CE_ScrollBarFirst:
    case CE_ScrollBarLast:
        //QtMaemo6TestStyle::drawControl(element, opt, p, widget);
        //these cases do not occur
        break;
    case CE_ScrollBarSlider: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            const DuiPositionIndicatorStyle *style =
                static_cast<const DuiPositionIndicatorStyle *>(QtMaemo6StylePrivate::duiStyle(slider->state,
                        "DuiPositionIndicatorStyle"));

            d->drawWidgetBackground(p, opt, slider->rect, style);

            const QPixmap *railDotPixmap = style->offPixmap();
            const QPixmap *railDotPlayedPixmap = style->onPixmap();
            QSize maxRailDotSize =
                (railDotPixmap->size()).expandedTo(railDotPlayedPixmap->size());

            int sliderLength = slider->orientation == Qt::Horizontal ? slider->rect.width() : slider->rect.height();
            int railDotSize = slider->orientation == Qt::Horizontal ? maxRailDotSize.width() : maxRailDotSize.height();

            int dotCount = sliderLength / (railDotSize + style->pixmapDistance());
            int offset = (sliderLength - (dotCount * (railDotSize + style->pixmapDistance()) - style->pixmapDistance())) / 2;

            //calculating the slider size, and position within the groove
            //QRect scSliderRect = subControlRect(CC_ScrollBar, slider, SC_ScrollBarSlider, widget);
            QRect scSliderRect = subControlRect(CC_ScrollBar, slider, SC_ScrollBarSlider, widget);

            int sliderStart, sliderEnd;
            if (slider->orientation == Qt::Horizontal) {
                sliderStart = scSliderRect.x();
                sliderEnd = scSliderRect.x() + scSliderRect.width();
            } else {
                sliderStart = scSliderRect.y();
                sliderEnd = scSliderRect.y() + scSliderRect.height();
            }
            int grooveLength = sliderEnd - sliderStart;

            int grooveDotCount = qRound((qreal)grooveLength * (qreal)dotCount / (qreal)sliderLength);
            int sliderDotsSubPage = qRound((qreal)sliderStart * (qreal)dotCount / (qreal)sliderLength);
            //int sliderDotsAddPage = dotCount - (grooveDotCount + sliderDotsSubPage);

            //make copies of the pixmaps to avoid doing the transparency within the loop
            QPixmap railDotPixmapAlpha(*railDotPixmap);
            QPixmap railDotPlayedPixmapAlpha(*railDotPlayedPixmap);
            if (widget->dynamicPropertyNames().contains(WIDGET_OPACITY)) {
                double opacity = widget->property(WIDGET_OPACITY).toDouble();

                railDotPixmapAlpha = setPixmapOpacity(railDotPixmapAlpha, opacity);
                railDotPlayedPixmapAlpha = setPixmapOpacity(railDotPlayedPixmapAlpha, opacity);
            }

            for (int i = 0; i < dotCount; ++i) {
                QPixmap *pm = i < sliderDotsSubPage || i >= sliderDotsSubPage + grooveDotCount
                              ? &railDotPixmapAlpha : &railDotPlayedPixmapAlpha;

                if (slider->orientation == Qt::Horizontal)
                    p->drawPixmap(QPoint(offset, 0), *pm);
                else
                    p->drawPixmap(QPoint(0, offset), *pm);
                offset += railDotSize + style->pixmapDistance();
            }
        }
    }
    break;
    case CE_ItemViewItem: {
        if (const QStyleOptionViewItemV4 *item = qstyleoption_cast<const QStyleOptionViewItemV4 *>(opt)) {
            proxy()->drawPrimitive(PE_PanelItemViewItem, opt, p, widget);
            const DuiContentItemStyle *style =
                static_cast<const DuiContentItemStyle *>(QtMaemo6StylePrivate::duiStyle(item->state,
                        "DuiContentItemStyle"));
            const DuiLabelStyle *labelTitle =
                static_cast<const DuiLabelStyle *>(QtMaemo6StylePrivate::duiStyle(item->state,
                                                   "DuiLabelStyle", style->titleObjectName()));
            //subtitles for list view items are currently not supported by qt.
            //const DuiLabelStyle* labelSubTitle =
            //        static_cast<const DuiLabelStyle*>( QtMaemo6StylePrivate::duiStyle( item->state,
            //                                                                           "DuiLabelStyle", style->subtitleObjectName() ) );
            const DuiImageWidgetStyle *labelIcon =
                static_cast<const DuiImageWidgetStyle *>(QtMaemo6StylePrivate::duiStyle(item->state,
                        "DuiImageWidgetStyle", style->imageObjectName()));

            QString itemText = qvariant_cast<QString>(item->index.data(Qt::DisplayRole));
            QIcon itemIcon = qvariant_cast<QIcon>(item->index.data(Qt::DecorationRole));

            QRect rect = item->rect;

            if (!itemIcon.isNull()) {
                rect.setTop(rect.top() + labelIcon->marginTop());
                rect.setLeft(rect.left() + labelIcon->marginLeft());
                p->drawPixmap(rect.topLeft(), itemIcon.pixmap(labelIcon->preferredSize()));
                rect.setLeft(rect.left() + labelIcon->preferredSize().width() + labelIcon->marginRight());
            }

            rect.setLeft(rect.left() + labelTitle->marginLeft());
            QFont labelFont = labelTitle->font();

            p->setFont(labelFont);
            p->setPen(labelTitle->color());

            p->drawText(rect, itemText, QTextOption(Qt::AlignLeft | Qt::AlignTop));
        }
    }
    break;
    default: {
        QtMaemo6TestStyle::drawControl(element, opt, p, widget);
    }
    break;
    }
}

void QtMaemo6Style::drawComplexControl(ComplexControl control,
                                       const QStyleOptionComplex *opt,
                                       QPainter *p,
                                       const QWidget *widget /*= 0*/) const
{
    Q_D(const QtMaemo6Style);
    switch (control) {
    case CC_ComboBox: {
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {

            const DuiComboBoxStyle *style =
                static_cast<const DuiComboBoxStyle *>(QtMaemo6StylePrivate::duiStyle(cmb->state,
                        "DuiComboBoxStyle"));
            d->drawWidgetBackground(p, opt, cmb->rect, style);
        }
    }
    break;

    case CC_ToolButton: {
        QWidget *parentOfControl = widget->parentWidget();

        if (const QStyleOptionToolButton *btn = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QStyleOptionToolButton subopt = *btn;
            subopt.rect = subControlRect(control, opt, SC_ToolButton, widget);

            //a ToolButton on a ToolBar
            if (qobject_cast<QToolBar *>(parentOfControl)) {
                QtMaemo6TestStyle::drawComplexControl(control, opt, p, widget);
                //a ToolButton on the DuiTitleBar
            } else if (qobject_cast<QtMaemo6TitleBar *>(parentOfControl))  {
                QtMaemo6TestStyle::drawComplexControl(control, opt, p, widget);
                //a ToolButton anywhere else on the Screen should be drawn like a PushButton
            } else {
                const DuiButtonStyle *style =
                    static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(subopt.state, "DuiButtonIconStyle", ""));
                const DuiButtonStyle *styleFont =
                    static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(subopt.state, "DuiButtonIconStyle",
                                                        "NavigationBarToolBarButton"));

                qDebug("Button \"%s\" font-size: %d, icon-size: %d", subopt.text.toLocal8Bit().constData(),
                       styleFont->font().pixelSize(), styleFont->iconSize().width());

                d->drawBasicButton(p, subopt.text, subopt.icon, subopt.rect, opt, style, styleFont->font(), styleFont->iconSize());
            }
        }
    }
    break;

    case CC_Slider: {
        p->save();
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            const DuiSliderStyle *style =
                static_cast<const DuiSliderStyle *>(QtMaemo6StylePrivate::duiStyle(slider->state,
                                                    "DuiSliderStyle"));

            // draw widget background
            d->drawWidgetBackground(p, opt, widget->rect(), style);

            // draw groove
            bool isHorizontal = slider->orientation == Qt::Horizontal;

            QRect grooveArea = proxy()->subControlRect(CC_Slider, opt,
                               SC_SliderGroove,
                               widget);

            int maxSliderLength = proxy()->pixelMetric(PM_SliderLength,
                                  slider, widget);
            int grooveThickness = style->grooveThickness();

            QRect grooveRect;
            if (isHorizontal) {
                grooveRect = grooveArea.adjusted(maxSliderLength / 2,
                                                 (grooveArea.height() - grooveThickness) / 2,
                                                 - maxSliderLength / 2,
                                                 - (grooveArea.height() - grooveThickness) / 2);
            } else {
                grooveRect = grooveArea.adjusted((grooveArea.width() - grooveThickness) / 2,
                                                 maxSliderLength / 2,
                                                 - (grooveArea.width() - grooveThickness) / 2,
                                                 - maxSliderLength / 2);
            }

            QtMaemo6StylePrivate::drawSliderBaseBackground(p, opt, grooveRect, style, maxSliderLength);

            // Handle
            QPixmap handlePixmap;

            if (isHorizontal) {
                const QPixmap *handleSourcePixmap =
                    (slider->state & State_Sunken)
                    ? style->handlePressedPixmap()
                    : style->handlePixmap();
                handlePixmap = *handleSourcePixmap;
            } else {
                const QPixmap *handleSourcePixmap =
                    (slider->state & State_Sunken)
                    ? style->handleVerticalPressedPixmap()
                    : style->handleVerticalPixmap();
                handlePixmap = *handleSourcePixmap;
            }

            QRect handleRect = proxy()->subControlRect(CC_Slider, opt,
                               SC_SliderHandle,
                               widget);
            p->drawPixmap(handleRect, handlePixmap);
        }
        p->restore();
    }
    break;
    case CC_ScrollBar: {
        drawControl(CE_ScrollBarSlider, opt, p, widget);
    }
    break;
    case CC_GroupBox: {
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
            const DuiContainerStyle *groupBoxStyle =
                static_cast<const DuiContainerStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                        "DuiContainerStyle"));
            //causes problems because requires to include a private header file
            //DuiContainerHeader containerHeaderLabel;
            //const DuiLabelStyle * headerLabelStyle =
            //    static_cast<const DuiLabelStyle *>( QtMaemo6StylePrivate::duiStyle( groupBox->state,
            //                                                    "DuiLabelStyle", "", "", &containerHeaderLabel ) );

            const DuiLabelStyle *headerStyle =
                static_cast<const DuiLabelStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                                                   "DuiContainerHeaderStyle"));

            const DuiSeparatorStyle *separatorStyle =
                static_cast<const DuiSeparatorStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                        "DuiSeparatorStyle"));

            QRect headerTextRect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxLabel, widget);
            QRect checkBoxRect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxCheckBox, widget);

            int headMarginLeft =      headerStyle->paddingLeft()
                                      + headerStyle->marginLeft();
            int headMarginRight =     headerStyle->paddingRight()
                                      + headerStyle->marginRight();
            int headMarginBottom =    headerStyle->paddingBottom()
                                      + headerStyle->marginBottom()
                                      + 4; // Make it look good fix (while the header text padding in duicontainer is broken)

            QRect unitedRect = headerTextRect.united(checkBoxRect);
            int headerBottom = unitedRect.bottom() + headMarginBottom;

            int sepMarginLeft   =  separatorStyle->marginLeft() + separatorStyle->paddingLeft();
            int sepMarginRight  =  separatorStyle->marginRight() + separatorStyle->paddingRight();
            int sepMarginTop    =  separatorStyle->marginTop() + separatorStyle->paddingTop();

            QRect separatorRect(groupBox->rect.left() + sepMarginLeft,
                                headerBottom + sepMarginTop,
                                groupBox->rect.width()
                                - headMarginLeft - headMarginRight
                                - sepMarginLeft - sepMarginRight,
                                separatorStyle->span());

            // Draw frame
            // Draw widget background
            QRect headerRect = widget->rect();
            headerRect.setBottom(headerBottom);
            QRect containerRect = widget->rect();
            containerRect.adjust(0, headerRect.height(), 0, 0);

            d->drawWidgetBackground(p, groupBox, headerRect, headerStyle);
            d->drawWidgetBackground(p, groupBox, containerRect, groupBoxStyle);
//                drawWidgetBackground( p, groupBox, separatorRect, separatorStyle );

            // Draw title
            if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {

                QColor headerColor = Qt::black; //FIXME: = headerLabelStyle->color();
                if (headerColor.isValid()) {
                    p->setPen(headerColor);
                }

                int alignment = int(groupBox->textAlignment);
                if (!proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
                    alignment |= Qt::TextHideMnemonic;

                p->drawText(headerTextRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                            groupBox->text);
            }

            // Draw checkbox
            if (groupBox->subControls & SC_GroupBoxCheckBox) {
                d->drawCheckBox(p, QString(), QIcon(), checkBoxRect,
                                groupBox, "DuiButtonIconStyle");
            }

        }
    }
    break;
    default: {
        QtMaemo6TestStyle::drawComplexControl(control, opt, p, widget);
    }
    break;
    }
}

QRect QtMaemo6Style::subControlRect(ComplexControl control,
                                    const QStyleOptionComplex *option,
                                    SubControl subControl,
                                    const QWidget *widget /*= 0*/) const
{
    Q_D(const QtMaemo6Style);

    if (!d->m_isDuiInitialized) {
        return QRect();
    }

    switch (control) {
    case CC_ToolButton: {
    }
    break;

    case CC_Slider: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            int maxSliderThickness = proxy()->pixelMetric(PM_SliderControlThickness, slider, widget);
            int maxSliderLength = proxy()->pixelMetric(PM_SliderLength, slider, widget);
            bool isHorizontal = (slider->orientation == Qt::Horizontal);

            switch (subControl) {
            case SC_SliderHandle: {
                int span = (isHorizontal)
                           ? slider->rect.width() - maxSliderLength
                           : slider->rect.height() - maxSliderLength;

                int handlePos = sliderPositionFromValue(slider->minimum,
                                                        slider->maximum,
                                                        slider->sliderPosition,
                                                        span,
                                                        slider->upsideDown);
                if (isHorizontal)
                    return QRect(slider->rect.x() + handlePos,
                                 slider->rect.y(),
                                 maxSliderLength,
                                 maxSliderThickness);
                else
                    return QRect(slider->rect.x(),
                                 slider->rect.y() + handlePos,
                                 maxSliderThickness,
                                 maxSliderLength);
            }
            break;
            case SC_SliderGroove: {
                if (isHorizontal)
                    return QRect(slider->rect.x(),
                                 slider->rect.y(),
                                 slider->rect.width(),
                                 maxSliderThickness);
                else
                    return QRect(slider->rect.x(),
                                 slider->rect.y(),
                                 maxSliderThickness,
                                 slider->rect.height());
            }
            break;
            default:
                break;
            }
        }
    }
    break;

    case CC_ScrollBar: {
        if (subControl == SC_ScrollBarSlider)
            return d->scrollBarSliderRect(option, widget);
        break;
    }
    case CC_GroupBox: {
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            switch (subControl) {
            case SC_GroupBoxFrame:
                // Fall Through
            case SC_GroupBoxContents: {
                QRect contentsRect;

                const DuiLabelStyle *headerStyle =
                    static_cast<const DuiLabelStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                                                       "DuiLabelStyle",
                                                       "DuiContainerTitle"));

                QFont headerFont = headerStyle->font();
                QFontMetrics fontMetrics(headerFont);
                int height = fontMetrics.height();

                const DuiContainerStyle *groupBoxStyle =
                    static_cast<const DuiContainerStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                            "DuiContainerStyle"));
                const DuiSeparatorStyle *separatorStyle =
                    static_cast<const DuiSeparatorStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                            "DuiSeparatorStyle"));

                int sepMarginTop    =  separatorStyle->marginTop() + separatorStyle->paddingTop();
                int sepMarginBottom =  separatorStyle->marginBottom() + separatorStyle->paddingBottom();

                int leftMargin =    groupBoxStyle->paddingLeft()
                                    + groupBoxStyle->marginLeft();

                int rightMargin =   groupBoxStyle->paddingRight()
                                    + groupBoxStyle->marginRight();

                int topMargin =     groupBoxStyle->paddingTop()
                                    + groupBoxStyle->marginTop()
                                    + headerStyle->paddingTop()
                                    + headerStyle->marginTop()
                                    + sepMarginTop
                                    + separatorStyle->span()
                                    + sepMarginBottom;

                int checkMarkHeight = 0;

                bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;

                if (hasCheckBox) {
                    QStyleOptionButton checkBox;
                    checkBox.QStyleOption::operator=(*option);
                    checkMarkHeight = proxy()->pixelMetric(PM_IndicatorHeight, &checkBox, widget);
                }

                topMargin += qMax(height, checkMarkHeight);

                contentsRect = groupBox->rect.adjusted(leftMargin,
                                                       topMargin,
                                                       -rightMargin,
                                                       0);

                return contentsRect;
                break;
            }
            case SC_GroupBoxCheckBox:
                // Fall Through
            case SC_GroupBoxLabel: {
                QRect labelRect;

                const DuiLabelStyle *headerStyle =
                    static_cast<const DuiLabelStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                                                       "DuiLabelStyle",
                                                       "DuiContainerTitle"));
                QFont headerFont = headerStyle->font();
                QFontMetrics fontMetrics(headerFont);
                int height = fontMetrics.height();
                int textWidth = fontMetrics.size(Qt::TextShowMnemonic, groupBox->text).width() + 1;

                const DuiContainerStyle *groupBoxStyle =
                    static_cast<const DuiContainerStyle *>(QtMaemo6StylePrivate::duiStyle(groupBox->state,
                            "DuiContainerStyle"));

                int leftMargin =    groupBoxStyle->paddingLeft()
                                    + groupBoxStyle->marginLeft()
                                    + headerStyle->paddingLeft()
                                    + headerStyle->marginLeft();

                int rightMargin =   groupBoxStyle->paddingRight()
                                    + groupBoxStyle->marginRight()
                                    + headerStyle->paddingRight()
                                    + headerStyle->marginRight();

                int topMargin =     groupBoxStyle->paddingTop()
                                    + groupBoxStyle->marginTop()
                                    + headerStyle->paddingTop()
                                    + headerStyle->marginTop();

                //unused
                //int bottomMargin =   groupBoxStyle->paddingBottom()
                //                  + groupBoxStyle->marginBottom()
                //                  + headerStyle->paddingBottom()
                //                  + headerStyle->marginBottom();

                labelRect = groupBox->rect.adjusted(leftMargin,
                                                    topMargin,
                                                    -rightMargin,
                                                    0);

                labelRect.setHeight(height);

                // We need a QStyleOptionButton based style option in order
                // to get correct geometries.
                QStyleOptionButton checkBox;
                checkBox.QStyleOption::operator=(*option);
                bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
                int checkMarkWidth = 0;
                if (hasCheckBox) {
                    checkMarkWidth =   proxy()->pixelMetric(PM_IndicatorWidth, &checkBox, widget);
                    + proxy()->pixelMetric(PM_CheckBoxLabelSpacing, &checkBox, widget);
                }
                QRect totalRect = alignedRect(groupBox->direction,
                                              groupBox->textAlignment,
                                              QSize(textWidth + checkMarkWidth, height),
                                              labelRect);

                // Adjust totalRect if checkbox is set
                if (hasCheckBox) {
                    int checkMarkHeight = proxy()->pixelMetric(PM_IndicatorHeight, &checkBox, widget);

                    bool leftToRight = (groupBox->direction == Qt::LeftToRight);
                    int left = 0;
                    // Adjust for check box
                    if (subControl == SC_GroupBoxCheckBox) {
                        left = leftToRight ? totalRect.left() : (totalRect.right() - checkMarkWidth);
                        int top = totalRect.top();
                        totalRect.setRect(left, top, checkMarkWidth, checkMarkHeight);
                        // Adjust for label
                    } else {
                        left = leftToRight ? (totalRect.left() + checkMarkWidth) : totalRect.left();
                        int top = totalRect.top() + (checkMarkHeight - fontMetrics.height()) / 2;
                        totalRect.setRect(left, top,
                                          totalRect.width() - checkMarkWidth, totalRect.height());
                    }
                }
                return totalRect;

                break;
            }
            default:
                break;
            }
        }
        break;
    }
    default:
        break;
    }
    return QtMaemo6TestStyle::subControlRect(control, option, subControl, widget);
}

QSize QtMaemo6Style::sizeFromContents(ContentsType type,
                                      const QStyleOption *option,
                                      const QSize &contentsSize,
                                      const QWidget *widget) const
{
    Q_D(const QtMaemo6Style);

    QSize retSize = QtMaemo6TestStyle::sizeFromContents(type, option, contentsSize, widget);

    switch (type) {
    case CT_ScrollBar:
        if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {

            int scrollBarExtent = proxy()->pixelMetric(PM_ScrollBarExtent, option, widget);
            int scrollBarSliderMinimum = proxy()->pixelMetric(PM_ScrollBarSliderMin, option, widget);
            if (scrollBar->orientation == Qt::Horizontal) {
                retSize = QSize(scrollBarSliderMinimum, scrollBarExtent);
            } else {
                retSize = QSize(scrollBarExtent, scrollBarSliderMinimum);
            }
        }
        break;
    case CT_Slider: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            int sliderThickness = proxy()->pixelMetric(PM_SliderThickness, slider, widget);

            bool isHorizontal = (slider->orientation == Qt::Horizontal);
            if (isHorizontal)
                retSize.setHeight(sliderThickness);
            else
                retSize.setWidth(sliderThickness);
            return retSize;
        }
    }
    break;
    case CT_ToolButton:
        if (widget) {
            QWidget *parentOfControl = widget->parentWidget();

            if (const QStyleOptionToolButton *btn = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
                //a ToolButton on a ToolBar
                if (dynamic_cast<QToolBar *>(parentOfControl)) {
                    //intentionally no code here at the moment
                    //a ToolButton on the DuiTitleBar
                } else if (dynamic_cast<QtMaemo6TitleBar *>(parentOfControl))  {
                    //intentionally no code here at the moment
                    //a ToolButton anywhere else on the Screen should be drawn like a PushButton
                } else {
                    QStyleOptionToolButton subopt = *btn;
                    const DuiButtonStyle *style =
                        static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(subopt.state, "DuiButtonIconStyle", ""));
                    const DuiButtonStyle *styleFont =
                        static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(subopt.state, "DuiButtonIconStyle",
                                                            "NavigationBarToolBarButton"));

                    int borderTop, borderRight, borderBottom, borderLeft;
                    d->paddingFromStyle(style, &borderLeft, &borderTop, &borderRight, &borderBottom);

                    QRect textAndIconRect = d->getTextAndIconRect(style, subopt.text, subopt.icon, styleFont->font(), styleFont->iconSize());
                    textAndIconRect.setWidth(textAndIconRect.width() + borderLeft + borderRight);
                    textAndIconRect.setHeight(textAndIconRect.height() + borderTop + borderBottom);
                    retSize = textAndIconRect.size();
                }
            }
        }
        break;
    case CT_PushButton: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QStyleOptionButton subopt = *btn;
            const DuiButtonStyle *style =
                static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(subopt.state, "DuiButtonIconStyle", ""));

            int borderTop, borderRight, borderBottom, borderLeft;
            d->paddingFromStyle(style, &borderLeft, &borderTop, &borderRight, &borderBottom);

            QRect textAndIconRect = d->getTextAndIconRect(style, subopt.text, subopt.icon);
            textAndIconRect.setWidth(textAndIconRect.width() + borderLeft + borderRight);
            textAndIconRect.setHeight(textAndIconRect.height() + borderTop + borderBottom);
            retSize = textAndIconRect.size();
        }
    }
    break;
    case CT_ItemViewItem:
        if (const QStyleOptionViewItemV4 *item = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option)) {
            const DuiContentItemStyle *style =
                static_cast<const DuiContentItemStyle *>(QtMaemo6StylePrivate::duiStyle(option->state,
                        "DuiContentItemStyle"));
            const DuiLabelStyle *labelTitle =
                static_cast<const DuiLabelStyle *>(QtMaemo6StylePrivate::duiStyle(option->state,
                                                   "DuiLabelStyle", style->titleObjectName()));
            // subtitles for ListViewItems are currently not provided by qt.
            //const DuiLabelStyle* labelSubTitle =
            //        static_cast<const DuiLabelStyle*>( QtMaemo6StylePrivate::duiStyle( option->state,
            //                                               "DuiLabelStyle", style->subtitleObjectName() ) );
            const DuiImageWidgetStyle *labelIcon =
                static_cast<const DuiImageWidgetStyle *>(QtMaemo6StylePrivate::duiStyle(option->state,
                        "DuiImageWidgetStyle", style->imageObjectName()));

            QString itemText = qvariant_cast<QString>(item->index.data(Qt::DisplayRole)); //.toString();
            QIcon itemIcon = qvariant_cast<QIcon>(item->index.data(Qt::DecorationRole));

            //always calculate icon size, even if the index has no item,
            // to get equal row heights
            QSize iconSize = labelIcon->preferredSize() +
                             QSize(labelIcon->marginLeft() + labelIcon->marginRight(),
                                   labelIcon->marginTop() + labelIcon->marginBottom());

            QFontMetrics fontMetrics(labelTitle->font());
            QRect textRect = fontMetrics.boundingRect(itemText);
            textRect.setWidth(textRect.width() +
                              labelTitle->marginLeft() + labelTitle->marginRight());

            retSize.setWidth(textRect.width() + iconSize.width());
            if (iconSize.height() < textRect.height())
                retSize.setHeight(textRect.height());
            else
                retSize.setHeight(iconSize.height());

        }
        break;
    case CT_MenuBar: {
        //return 0 size because the menubar should not be drawn
        // this is done by the windowdecoration
        if (widget == d->m_menuBar)
            retSize = QSize(0, 0);
    }
    break;
    default:
        break;
    }
    return retSize;
}

int QtMaemo6Style::pixelMetric(PixelMetric metric,
                               const QStyleOption *option,
                               const QWidget *widget) const
{
    switch (metric) {
    case PM_ScrollBarExtent:
    case PM_ScrollBarSliderMin: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            const DuiPositionIndicatorStyle *style =
                static_cast<const DuiPositionIndicatorStyle *>(QtMaemo6StylePrivate::duiStyle(slider->state,
                        "DuiPositionIndicatorStyle"));

            QSize pixmapSize = style->onPixmap()->size().expandedTo(style->offPixmap()->size());

            int minSize;
            if (slider->orientation == Qt::Horizontal)
                minSize = pixmapSize.width();
            else
                minSize = pixmapSize.height();
            return minSize;

        }
    }
    break;
    case PM_SliderThickness:
    case PM_SliderControlThickness: {
        int sliderThickness = 0;

        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            const DuiSliderStyle *style =
                static_cast<const DuiSliderStyle *>(QtMaemo6StylePrivate::duiStyle(slider->state,
                                                    "DuiSliderStyle"));
            if (style) {
                bool isHorizontal = slider->orientation == Qt::Horizontal;
                const QPixmap *handleSourcePixmap = (isHorizontal)
                                                    ? style->handlePixmap()
                                                    : style->handleVerticalPixmap();
                const QPixmap *handlePressedSourcePixmap = (isHorizontal)
                        ? style->handlePressedPixmap()
                        : style->handleVerticalPressedPixmap();

                QSize sliderHandleSize = handleSourcePixmap->size();
                QSize sliderHandlePressedSize = handlePressedSourcePixmap->size();
                QSize maxHandleSize = sliderHandleSize.expandedTo(sliderHandlePressedSize);
                sliderThickness = maxHandleSize.height();
            }
        }
        return sliderThickness;
    }
    break;
    case PM_SliderLength: {
        int sliderLength = 0;

        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            const DuiSliderStyle *style =
                static_cast<const DuiSliderStyle *>(QtMaemo6StylePrivate::duiStyle(slider->state,
                                                    "DuiSliderStyle"));
            if (style) {
                bool isHorizontal = slider->orientation == Qt::Horizontal;
                const QPixmap *handleSourcePixmap = (isHorizontal)
                                                    ? style->handlePixmap()
                                                    : style->handleVerticalPixmap();
                const QPixmap *handlePressedSourcePixmap = (isHorizontal)
                        ? style->handlePressedPixmap()
                        : style->handleVerticalPressedPixmap();

                QSize sliderHandleSize = handleSourcePixmap->size();
                QSize sliderHandlePressedSize = handlePressedSourcePixmap->size();
                QSize maxHandleSize = sliderHandleSize.expandedTo(sliderHandlePressedSize);
                sliderLength = maxHandleSize.width();
            }
        }
        return sliderLength;
    }
    break;
    case PM_ExclusiveIndicatorWidth:
    case PM_IndicatorWidth: {

        int left = 0;
        int right = 0;
        int top = 0;
        int bottom = 0;
        int fontSize = 10;

        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            const DuiButtonStyle *style =
                static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(btn->state,
                                                    "DuiButtonIconStyle",
                                                    QString(),
                                                    "checkbox"));

            if (style) {
                // Check the recommended border size ...
                if (style->backgroundImage()) {
                    style->backgroundImage()->borders(&left, &right, &top, &bottom);
                }
                // Use the font size for determining the ideal checkmark height
                fontSize = style->font().pixelSize();
            }
        }
        return left + 0.8 * fontSize + right;
    }
    break;
    case PM_ExclusiveIndicatorHeight:
    case PM_IndicatorHeight: {

        int left = 0;
        int right = 0;
        int top = 0;
        int bottom = 0;
        int fontSize = 10;

        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            const DuiButtonStyle *style =
                static_cast<const DuiButtonStyle *>(QtMaemo6StylePrivate::duiStyle(btn->state,
                                                    "DuiButtonIconStyle",
                                                    QString(),
                                                    "checkbox"));

            if (style) {
                // Check the recommended border size ...
                if (style->backgroundImage()) {
                    style->backgroundImage()->borders(&left, &right, &top, &bottom);
                }
                // Use the font size for determining the ideal checkmark height
                fontSize = style->font().pixelSize();
            }
        }
        return top + 0.8 * fontSize + bottom;
    }
    break;
    case PM_TabBarScrollButtonWidth:
        return widget->height();
    default:
        break;
    }

    return QtMaemo6TestStyle::pixelMetric(metric, option, widget);
}

QIcon QtMaemo6Style::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option,
        const QWidget *widget) const
{
    QIcon   icon;
    QPixmap pixmap;

    switch (standardIcon) {
    case QStyle::SP_MessageBoxInformation: // 09
        return QIcon();
    case QStyle::SP_MessageBoxWarning:   // 10
        return QIcon();
    case QStyle::SP_MessageBoxCritical:  // 11
        return QIcon();
    case QStyle::SP_MessageBoxQuestion:  // 12
        return QIcon();
    case QStyle::SP_DialogOkButton:      // 38
        return QIcon();
    case QStyle::SP_DialogCancelButton:  // 39
        return QIcon();
    case QStyle::SP_DialogHelpButton:    // 40
        return QIcon();
    case QStyle::SP_DialogOpenButton:    // 41
        return QIcon();
    case QStyle::SP_DialogSaveButton:    // 42
        return QIcon();
    case QStyle::SP_DialogCloseButton:   // 43
        return QIcon();
    case QStyle::SP_DialogApplyButton:   // 44
        return QIcon();
    case QStyle::SP_DialogResetButton:   // 45
        return QIcon();
    case QStyle::SP_DialogDiscardButton: // 46
        return QIcon();
    case QStyle::SP_DialogYesButton:     // 47
        return QIcon();
    case QStyle::SP_DialogNoButton:      // 48
        return QIcon();
    default:
        break;
    }

    if (icon.isNull())
        icon = QCommonStyle::standardIconImplementation(standardIcon, option, widget);
    return icon;
}

int QtMaemo6Style::styleHint(StyleHint hint, const QStyleOption *option,
                             const QWidget *widget, QStyleHintReturn *returnData) const
{
    if (hint == QStyle::SH_ToolBar_Movable) {
        return false;
    } else if (hint == QStyle::SH_RequestSoftwareInputPanel) {
        return QStyle::RSIP_OnMouseClick;
    }

    return QtMaemo6TestStyle::styleHint(hint, option, widget, returnData);
}
/*
    Private implementation specific methods:
 */








