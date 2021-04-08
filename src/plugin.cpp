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

#include "plugin.h"
#include "debug.h"

#include <QEventLoop>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QPluginLoader>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QStringList>
#include <QVariantMap>

#include <LomiriSystemSettings/ItemBase>
#include <LomiriSystemSettings/PluginInterface>

using namespace SystemSettings;

static const QLatin1String pluginModuleDir{PLUGIN_MODULE_DIR};
static const QLatin1String pluginQmlDir{QML_DIR};

namespace SystemSettings {

class PluginPrivate
{
    Q_DECLARE_PUBLIC(Plugin)

    inline PluginPrivate(Plugin *q, const QFileInfo &manifest);
    ~PluginPrivate() {};

    bool ensureLoaded() const;
    QUrl componentFromSettingsFile(const QString &key) const;

private:
    mutable Plugin *q_ptr;
    mutable ItemBase *m_item;
    mutable QPluginLoader m_loader;
    mutable PluginInterface *m_plugin;
    mutable PluginInterface2 *m_plugin2;
    QString m_baseName;
    QVariantMap m_data;
    QString m_dataPath;
};

} // namespace

PluginPrivate::PluginPrivate(Plugin *q, const QFileInfo &manifest):
    q_ptr(q),
    m_item(0),
    m_plugin(0),
    m_plugin2(0),
    m_baseName(manifest.completeBaseName())
{
    QFile file(manifest.filePath());
    if (Q_UNLIKELY(!file.open(QIODevice::ReadOnly | QIODevice::Text))) {
        qWarning() << "Couldn't open file" << manifest.filePath();
        return;
    }

    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &error);
    if (Q_UNLIKELY(json.isEmpty())) {
        qWarning() << "File is empty:" << manifest.filePath() <<
            error.errorString();
        return;
    }

    m_data = json.toVariant().toMap();
    m_dataPath = manifest.absolutePath();
}

bool PluginPrivate::ensureLoaded() const
{
    Q_Q(const Plugin);

    if (m_item != 0) return true;

    if (Q_UNLIKELY(m_loader.isLoaded())) return false;

    /* We also get called if there is no pageComponent nor plugin in the
     * settings file. Just return. */
    QString plugin = m_data.value(keyPlugin).toString();
    if (plugin.isEmpty())
        return false;

    auto ctx = QQmlEngine::contextForObject(q);
    const QString mountPoint = ctx ?
        ctx->contextProperty("mountPoint").value<QByteArray>() :
        "";

    QString name = QString("%1%2/lib%3.so")
        .arg(mountPoint).arg(pluginModuleDir).arg(plugin);

    m_loader.setFileName(name);
    if (Q_UNLIKELY(!m_loader.load())) {
        qWarning() << m_loader.errorString() << name;
        return false;
    }

    m_plugin2 = qobject_cast<SystemSettings::PluginInterface2*>(
                m_loader.instance());

    if (m_plugin2)
        m_plugin = m_plugin2;
    else
        m_plugin = qobject_cast<SystemSettings::PluginInterface*>(
                    m_loader.instance());

    if (Q_UNLIKELY(m_plugin == 0)) {
        qWarning() << name << "doesn't implement PluginInterface";
        return false;
    }

    m_item = m_plugin->createItem(m_data);
    if (m_item == 0) return false;

    QObject::connect(m_item, SIGNAL(iconChanged()),
                     q, SIGNAL(iconChanged()));
    QObject::connect(m_item, SIGNAL(keywordsChanged()),
                     q, SIGNAL(keywordsChanged()));
    QObject::connect(m_item, SIGNAL(nameChanged()),
                     q, SIGNAL(displayNameChanged()));
    QObject::connect(m_item, SIGNAL(visibilityChanged()),
                     q, SIGNAL(visibilityChanged()));
    return true;
}

QUrl PluginPrivate::componentFromSettingsFile(const QString &key) const
{
    QUrl componentUrl = m_data.value(key).toString();
    if (!componentUrl.isEmpty()) {
        if (componentUrl.isRelative()) {
            QDir dir(m_dataPath);
            if (dir.cd(pluginQmlDir) && dir.cd(m_baseName)) {
                componentUrl =
                    QUrl::fromLocalFile(dir.filePath(componentUrl.path()));
            }
        }
    }
    return componentUrl;
}

Plugin::Plugin(const QFileInfo &manifest, QObject *parent):
    QObject(parent),
    d_ptr(new PluginPrivate(this, manifest))
{
}

Plugin::~Plugin()
{
    delete d_ptr;
}

QString Plugin::displayName() const
{
    Q_D(const Plugin);
    QString ret = d->m_data.value(keyName).toString();
    if (d->m_data.value(keyHasDynamicName).toBool()) {
        if (!d->ensureLoaded()) return ret;
        ret = d->m_item->name();
    }
    return ret;
}

QString Plugin::baseName() const
{
    Q_D(const Plugin);
    return d->m_baseName;
}

QUrl Plugin::icon() const
{
    Q_D(const Plugin);
    QString iconName = d->m_data.value(keyIcon).toString();
    if (iconName.isEmpty()) {
        if (!d->ensureLoaded()) return QUrl();
        return d->m_item->icon();
    } else if (iconName.startsWith("/")) {
        return QString("file://") + iconName;
    } else {
        return QString("image://theme/") + iconName;
    }
}

QString Plugin::category() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyCategory).toString();
}

int Plugin::priority() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyPriority).toInt();
}

QString Plugin::translations() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyTranslations).toString();
}

QStringList Plugin::keywords() const
{
    Q_D(const Plugin);
    QStringList ret = d->m_data.value(keyKeywords).toStringList();
    if (d->m_data.value(keyHasDynamicKeywords).toBool()) {
        if (!d->ensureLoaded()) return ret;
        ret += d->m_item->keywords();
    }
    return ret;
}

bool Plugin::isVisible() const
{
    Q_D(const Plugin);
    if (!d->m_data.value(keyVisibleIfFileExists).isNull()) {
        QFile KeyFile(d->m_data.value(keyVisibleIfFileExists).toString());
        if (!KeyFile.exists())
                return false;
    }

    // TODO: visibility check depending on form-factor
    if (d->m_data.value(keyHasDynamicVisibility).toBool()) {
        if (!d->ensureLoaded()) return false;
        return d->m_item->isVisible();
    }
    return true;
}

bool Plugin::hideByDefault() const
{
    Q_D(const Plugin);
    return d->m_data.value(keyHideByDefault, false).toBool();
}

void Plugin::reset()
{
    Q_D(const Plugin);

    d->ensureLoaded();

    /* If the plugin implements version 1 of the interface but not version
     * 2, we assume that we won't find a "reset()" method in the
     * pageComponent either.
     */
    if (d->m_plugin && !d->m_plugin2)
        return;

    // Try to use the plugin's reset method
    if (d->m_plugin2 && d->m_plugin2->reset())
        return;

    // Otherwise, try to use one from the page component
    QQmlComponent *component = pageComponent();

    if (!component)
        return;

    QObject *object = component->create();

    // If it's there, try to search for the method
    if (!object) {
        delete component;
        return;
    }

    const QMetaObject *metaObject = object->metaObject();
    int index = metaObject->indexOfMethod(
                QMetaObject::normalizedSignature("reset(void)"));

    // and if that exists, call it
    if (index >= 0) {
        QMetaMethod method = metaObject->method(index);
        method.invoke(object, Qt::DirectConnection);
    }

    delete object;
    delete component;
}

QQmlComponent *Plugin::entryComponent()
{
    Q_D(const Plugin);

    QQmlContext *context = QQmlEngine::contextForObject(this);
    if (Q_UNLIKELY(context == 0)) return 0;

    QString title = displayName();
    QUrl iconUrl = icon();
    QUrl entryComponentUrl = d->componentFromSettingsFile(keyEntryComponent);
    if (!entryComponentUrl.isEmpty()) {
        return new QQmlComponent(context->engine(), entryComponentUrl, this);
    } else if (title.isEmpty() || iconUrl.isEmpty()) {
        /* The entry component is generated by the plugin */
        if (!d->ensureLoaded()) return 0;
        return d->m_item->entryComponent(context->engine(), this);
    } else {
        return new QQmlComponent(context->engine(),
                                 QUrl("qrc:/qml/EntryComponent.qml"),
                                 this);
    }
}

QQmlComponent *Plugin::pageComponent()
{
    Q_D(const Plugin);
    QQmlContext *context = QQmlEngine::contextForObject(this);
    if (Q_UNLIKELY(context == 0)) return 0;

    QUrl pageComponentUrl = d->componentFromSettingsFile(keyPageComponent);
    if (!pageComponentUrl.isEmpty()) {
        return new QQmlComponent(context->engine(), pageComponentUrl, this);
    } else {
        if (!d->ensureLoaded()) return 0;
        return d->m_item->pageComponent(context->engine(), this);
    }
}
