/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "test-plugin.h"

#include <QDebug>
#include <QStringList>
#include <LomiriSystemSettings/ItemBase>

using namespace LomiriSystemSettings;

class TestItem: public ItemBase
{
    Q_OBJECT

public:
    TestItem(const QVariantMap &staticData, QObject *parent = 0);
    ~TestItem();

    virtual QQmlComponent *entryComponent(QQmlEngine *engine,
                                          QObject *parent = 0);
    virtual QQmlComponent *pageComponent(QQmlEngine *engine,
                                         QObject *parent = 0);
    QString name() const;
private:
    QQmlComponent *m_pageComponent;
};

TestItem::TestItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent),
    m_pageComponent(0)
{
    QString name = staticData["name"].toString();
    if (name == "Wireless") {
        QStringList keywords;
        keywords << "one" << "two" << "three";
        setKeywords(keywords);
    } else if (name == "Brightness") {
        setName("Brightness & Display");
    }
}

TestItem::~TestItem()
{
}

QQmlComponent *TestItem::entryComponent(QQmlEngine *engine, QObject *parent)
{
    Q_UNUSED(engine);
    Q_UNUSED(parent);
    return 0;
}


QQmlComponent *TestItem::pageComponent(QQmlEngine *engine, QObject *parent)
{
    if (m_pageComponent == NULL) {
        QQmlComponent *page = new QQmlComponent(engine, parent);
        page->setData("import QtQuick 2.4\n"
                      "Rectangle {\n"
                      "  function reset() { console.log('Hello') }\n"
                      "  width: 200; height: 200;\n"
                      "  objectName: \"myRect\"\n"
                      "  color: \"red\""
                      "}",
                      QUrl());
        m_pageComponent = page;
    }
    return m_pageComponent;
}

TestPlugin::TestPlugin():
    QObject()
{
}

ItemBase *TestPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new TestItem(staticData, parent);
}

#include "test-plugin.moc"
