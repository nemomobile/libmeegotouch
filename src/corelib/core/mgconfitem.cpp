/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
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

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <MDebug>

#include "mgconfitem.h"

#include <gconf/gconf-value.h>
#include <gconf/gconf-client.h>

struct MGConfItemPrivate {
    QString key;
    QVariant value;
    guint notify_id;
    bool have_gconf;

    static void notify_trampoline(GConfClient *, guint, GConfEntry *, gpointer);
};

#define withClient(c) for(GConfClient *c = (g_type_init(), gconf_client_get_default()); c; g_object_unref(c), c=NULL)

static QByteArray convertKey(const QString &key)
{
    if (key.startsWith('/'))
        return key.toUtf8();
    else {
        QString replaced = key;
        replaced.replace('.', '/');
        mWarning("mgconfitem.cpp") << "Using dot-separated key names with MGConfItem is deprecated.";
        mWarning("mgconfitem.cpp") << "Please use" << '/' + replaced << "instead of" << key;
        return '/' + replaced.toUtf8();
    }
}

static QString convertKey(const char *key)
{
    return QString::fromUtf8(key);
}

static QVariant convertValue(GConfValue *src)
{
    if (!src) {
        return QVariant();
    } else {
        switch (src->type) {
        case GCONF_VALUE_INVALID:
            return QVariant(QVariant::Invalid);
        case GCONF_VALUE_BOOL:
            return QVariant((bool)gconf_value_get_bool(src));
        case GCONF_VALUE_INT:
            return QVariant(gconf_value_get_int(src));
        case GCONF_VALUE_FLOAT:
            return QVariant(gconf_value_get_float(src));
        case GCONF_VALUE_STRING:
            return QVariant(QString::fromUtf8(gconf_value_get_string(src)));
        case GCONF_VALUE_LIST:
            switch (gconf_value_get_list_type(src)) {
            case GCONF_VALUE_STRING: {
                QStringList result;
                for (GSList *elts = gconf_value_get_list(src); elts; elts = elts->next)
                    result.append(QString::fromUtf8(gconf_value_get_string((GConfValue *)elts->data)));
                return QVariant(result);
            }
            default: {
                QList<QVariant> result;
                for (GSList *elts = gconf_value_get_list(src); elts; elts = elts->next)
                    result.append(convertValue((GConfValue *)elts->data));
                return QVariant(result);
            }
            }
        case GCONF_VALUE_SCHEMA:
        default:
            return QVariant();
        }
    }
}

static GConfValue *convertString(const QString &str)
{
    GConfValue *v = gconf_value_new(GCONF_VALUE_STRING);
    gconf_value_set_string(v, str.toUtf8().data());
    return v;
}

static GConfValueType primitiveType(const QVariant &elt)
{
    switch (elt.type()) {
    case QVariant::String:
        return GCONF_VALUE_STRING;
    case QVariant::Int:
        return GCONF_VALUE_INT;
    case QVariant::Double:
        return GCONF_VALUE_FLOAT;
    case QVariant::Bool:
        return GCONF_VALUE_BOOL;
    default:
        return GCONF_VALUE_INVALID;
    }
}

static GConfValueType uniformType(const QList<QVariant> &list)
{
    GConfValueType result = GCONF_VALUE_INVALID;

    foreach(const QVariant & elt, list) {
        GConfValueType elt_type = primitiveType(elt);

        if (elt_type == GCONF_VALUE_INVALID)
            return GCONF_VALUE_INVALID;

        if (result == GCONF_VALUE_INVALID)
            result = elt_type;
        else if (result != elt_type)
            return GCONF_VALUE_INVALID;
    }

    if (result == GCONF_VALUE_INVALID)
        return GCONF_VALUE_STRING;  // empty list.
    else
        return result;
}

static int convertValue(const QVariant &src, GConfValue **valp)
{
    GConfValue *v;

    switch (src.type()) {
    case QVariant::Invalid:
        v = NULL;
        break;
    case QVariant::Bool:
        v = gconf_value_new(GCONF_VALUE_BOOL);
        gconf_value_set_bool(v, src.toBool());
        break;
    case QVariant::Int:
        v = gconf_value_new(GCONF_VALUE_INT);
        gconf_value_set_int(v, src.toInt());
        break;
    case QVariant::Double:
        v = gconf_value_new(GCONF_VALUE_FLOAT);
        gconf_value_set_float(v, src.toDouble());
        break;
    case QVariant::String:
        v = convertString(src.toString());
        break;
    case QVariant::StringList: {
        GSList *elts = NULL;
        v = gconf_value_new(GCONF_VALUE_LIST);
        gconf_value_set_list_type(v, GCONF_VALUE_STRING);
        foreach(const QString & str, src.toStringList())
        elts = g_slist_prepend(elts, convertString(str));
        gconf_value_set_list_nocopy(v, g_slist_reverse(elts));
        break;
    }
    case QVariant::List: {
        GConfValueType elt_type = uniformType(src.toList());
        if (elt_type == GCONF_VALUE_INVALID)
            v = NULL;
        else {
            GSList *elts = NULL;
            v = gconf_value_new(GCONF_VALUE_LIST);
            gconf_value_set_list_type(v, elt_type);
            foreach(const QVariant & elt, src.toList()) {
                GConfValue *val = NULL;
                convertValue(elt, &val);  // guaranteed to succeed.
                elts = g_slist_prepend(elts, val);
            }
            gconf_value_set_list_nocopy(v, g_slist_reverse(elts));
        }
        break;
    }
    default:
        return 0;
    }

    *valp = v;
    return 1;
}

void MGConfItemPrivate::notify_trampoline(GConfClient *,
        guint,
        GConfEntry *,
        gpointer data)
{
    MGConfItem *item = (MGConfItem *)data;
    item->update_value(true);
}

void MGConfItem::update_value(bool emit_signal)
{
    QVariant new_value;

    withClient(client) {
        GError *error = NULL;
        QByteArray k = convertKey(priv->key);
        GConfValue *v = gconf_client_get(client, k.data(), &error);

        if (error) {
            mWarning("MGConfItem") << error->message;
            g_error_free(error);
            new_value = priv->value;
        } else {
            new_value = convertValue(v);
            if (v)
                gconf_value_free(v);
        }
    }

    if (new_value != priv->value) {
        priv->value = new_value;
        if (emit_signal)
            emit valueChanged();
    }
}

QString MGConfItem::key() const
{
    return priv->key;
}

QVariant MGConfItem::value() const
{
    return priv->value;
}

QVariant MGConfItem::value(const QVariant &def) const
{
    if (priv->value.isNull())
        return def;
    else
        return priv->value;
}

void MGConfItem::set(const QVariant &val)
{
    withClient(client) {
        QByteArray k = convertKey(priv->key);
        GConfValue *v;
        if (convertValue(val, &v)) {
            GError *error = NULL;

            if (v) {
                gconf_client_set(client, k.data(), v, &error);
                gconf_value_free(v);
            } else {
                gconf_client_unset(client, k.data(), &error);
            }

            if (error) {
                mWarning("MGConfItem") << error->message;
                g_error_free(error);
            } else if (priv->value != val) {
                priv->value = val;
                emit valueChanged();
            }

        } else
            mWarning("MGConfItem") << "Can't store a" << val.typeName();
    }
}

void MGConfItem::unset()
{
    set(QVariant());
}

QList<QString> MGConfItem::listDirs() const
{
    QList<QString> children;

    withClient(client) {
        QByteArray k = convertKey(priv->key);
        GError *error = NULL;
        GSList *dirs = gconf_client_all_dirs(client, k.data(), &error);
        if(error) {
            mWarning("MGConfItem") << error->message;
            g_error_free(error);
            return children;
        }

        for (GSList *d = dirs; d; d = d->next) {
            children.append(convertKey((char *)d->data));
            g_free(d->data);
        }
        g_slist_free(dirs);
    }

    return children;
}

QList<QString> MGConfItem::listEntries() const
{
    QList<QString> children;

    withClient(client) {
        QByteArray k = convertKey(priv->key);
        GError *error = NULL;
        GSList *entries = gconf_client_all_entries(client, k.data(), &error);
        if(error) {
            mWarning("MGConfItem") << error->message;
            g_error_free(error);
            return children;
        }

        for (GSList *e = entries; e; e = e->next) {
            children.append(convertKey(((GConfEntry *)e->data)->key));
            gconf_entry_free((GConfEntry *)e->data);
        }
        g_slist_free(entries);
    }

    return children;
}

MGConfItem::MGConfItem(const QString &key, QObject *parent)
    : QObject(parent)
{
    priv = new MGConfItemPrivate;
    priv->key = key;
    priv->notify_id = 0;
    withClient(client) {
        update_value(false);
        QByteArray k = convertKey(priv->key);
        GError *error = NULL;
        gconf_client_add_dir(client, k.data(), GCONF_CLIENT_PRELOAD_ONELEVEL, &error);
        if(error) {
            mWarning("MGConfItem") << error->message;
            g_error_free(error);
            priv->have_gconf = false;
            return;
        }
        priv->notify_id = gconf_client_notify_add(client, k.data(),
                          MGConfItemPrivate::notify_trampoline, this,
                          NULL, &error);
        if(error) {
            mWarning("MGConfItem") << error->message;
            g_error_free(error);
            priv->have_gconf = false;
            return;
        }
    }
    priv->have_gconf = true;
}

MGConfItem::~MGConfItem()
{
    if(priv->have_gconf) {
        withClient(client) {
            QByteArray k = convertKey(priv->key);
            gconf_client_notify_remove(client, priv->notify_id);
            GError *error = NULL;
            gconf_client_remove_dir(client, k.data(), &error);
            if(error) {
                mWarning("MGConfItem") << error->message;
                g_error_free(error);
                return;
            }
        }
    }
    delete priv;
}
