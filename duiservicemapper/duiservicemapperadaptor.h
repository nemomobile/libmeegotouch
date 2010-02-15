/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -c DuiServiceMapperAdaptor -a duiservicemapperadaptor.h:duiservicemapperadaptor.cpp duiservicefw.xml
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */
// Make doxygen skip this internal class
//! \cond

#ifndef DUISERVICEMAPPERADAPTOR_H_1263907450
#define DUISERVICEMAPPERADAPTOR_H_1263907450

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface com.nokia.DuiServiceFwIf
 */
class DuiServiceMapperAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.DuiServiceFwIf")
    Q_CLASSINFO("D-Bus Introspection", ""
                "  <interface name=\"com.nokia.DuiServiceFwIf\" >\n"
                "    <!--\n"
                "            Returns a list of service names that implement the specified interface,\n"
                "            or, if none specified, all servicenames\n"
                "        -->\n"
                "    <method name=\"serviceNames\" >\n"
                "      <arg direction=\"in\" type=\"s\" name=\"interfaceName\" />\n"
                "      <arg direction=\"out\" type=\"as\" />\n"
                "    </method>\n"
                "    <!--\n"
                "            Returns a (preferred) service name that implements the specified interface\n"
                "        -->\n"
                "    <method name=\"serviceName\" >\n"
                "      <arg direction=\"in\" type=\"s\" name=\"interfaceName\" />\n"
                "      <arg direction=\"out\" type=\"s\" />\n"
                "    </method>\n"
                "    <!--\n"
                "            Returns the service path for the specified interface\n"
                "        -->\n"
                "    <method name=\"servicePath\" >\n"
                "      <arg direction=\"in\" type=\"s\" name=\"interfaceName\" />\n"
                "      <arg direction=\"out\" type=\"s\" />\n"
                "    </method>\n"
                "    <method name=\"interfaceName\" >\n"
                "      <arg direction=\"in\" type=\"s\" name=\"serviceName\" />\n"
                "      <arg direction=\"out\" type=\"s\" />\n"
                "    </method>\n"
                "    <signal name=\"serviceAvailable\" >\n"
                "      <arg direction=\"out\" type=\"s\" name=\"service\" />\n"
                "      <arg direction=\"out\" type=\"s\" name=\"interface\" />\n"
                "    </signal>\n"
                "    <signal name=\"serviceUnavailable\" >\n"
                "      <arg direction=\"out\" type=\"s\" name=\"service\" />\n"
                "    </signal>\n"
                "  </interface>\n"
                "")
public:
    DuiServiceMapperAdaptor(QObject *parent);
    virtual ~DuiServiceMapperAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QString serviceName(const QString &interfaceName);
    QStringList serviceNames(const QString &interfaceName);
    QString servicePath(const QString &interfaceName);
    QString interfaceName(const QString &serviceName);
Q_SIGNALS: // SIGNALS
    void serviceAvailable(const QString &service, const QString &interface);
    void serviceUnavailable(const QString &service);
};

#endif
//! \endcond
