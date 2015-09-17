/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c BluezAgentManager1 -p agentmanager1 org.bluez.AgentManager1.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef AGENTMANAGER1_H_1442489332
#define AGENTMANAGER1_H_1442489332

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.bluez.AgentManager1
 */
class BluezAgentManager1: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.bluez.AgentManager1"; }

public:
    BluezAgentManager1(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~BluezAgentManager1();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> RegisterAgent(const QDBusObjectPath &agent, const QString &capability)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(agent) << QVariant::fromValue(capability);
        return asyncCallWithArgumentList(QStringLiteral("RegisterAgent"), argumentList);
    }

    inline QDBusPendingReply<> RequestDefaultAgent(const QDBusObjectPath &agent)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(agent);
        return asyncCallWithArgumentList(QStringLiteral("RequestDefaultAgent"), argumentList);
    }

    inline QDBusPendingReply<> UnregisterAgent(const QDBusObjectPath &agent)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(agent);
        return asyncCallWithArgumentList(QStringLiteral("UnregisterAgent"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace org {
  namespace bluez {
    typedef ::BluezAgentManager1 AgentManager1;
  }
}
#endif
