/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
#include <QFontDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QtDebug>
#include "language-plugin.h"
#include "keyboard-layout.h"

#include <algorithm>
#include <act/act.h>
#include <unicode/locid.h>
#include <unicode/unistr.h>

struct LanguageLocale
{
public:

    // Should be true if locale is the default for its language.
    // e.g. 'en_US' is the likely locale for 'en', 'en_CA' is not.
    bool likely;

    QString localeName;
    QString displayName;
    icu::Locale locale;

public:

    explicit LanguageLocale(const QString &name);

    bool operator<(const LanguageLocale &l) const;
};

LanguageLocale::LanguageLocale(const QString &name) :
    likely(false),
    localeName(name),
    locale(qPrintable(name))
{
    std::string string;
    icu::UnicodeString unicodeString;
    locale.getDisplayName(locale, unicodeString);
    unicodeString.toUTF8String(string);
    displayName = string.c_str();
    /* workaround iso-codes casing being inconsistant */
    if (displayName.length() > 0)
        displayName[0] = displayName[0].toUpper();
}

bool LanguageLocale::operator<(const LanguageLocale &l) const
{
    // Likely locales should precede unlikely ones of the same language.
    if (strcasecmp(locale.getLanguage(), l.locale.getLanguage()) == 0) {
        if (likely || l.likely)
            return likely && !l.likely;
    }

    return QString::localeAwareCompare(displayName, l.displayName) < 0;
}

void managerLoaded(GObject    *object,
                   GParamSpec *pspec,
                   gpointer    user_data);

LanguagePlugin::LanguagePlugin(QObject *parent) :
    QObject(parent),
    m_currentLanguage(-1),
    m_nextCurrentLanguage(-1),
    m_manager(act_user_manager_get_default()),
    m_user(nullptr)
{
    if (m_manager != nullptr) {
        g_object_ref(m_manager);

        gboolean loaded;
        g_object_get(m_manager, "is-loaded", &loaded, nullptr);

        if (loaded)
            managerLoaded();
        else
            g_signal_connect(m_manager, "notify::is-loaded",
                             G_CALLBACK(::managerLoaded), this);
    }

    updateLanguageNamesAndCodes();
    updateCurrentLanguage();
    updateSpellCheckingModel();
}

LanguagePlugin::~LanguagePlugin()
{
    if (m_user != nullptr) {
        g_signal_handlers_disconnect_by_data(m_user, this);
        g_object_unref(m_user);
    }

    if (m_manager != nullptr) {
        g_signal_handlers_disconnect_by_data(m_manager, this);
        g_object_unref(m_manager);
    }
}

const QStringList &
LanguagePlugin::languageNames() const
{
    return m_languageNames;
}

const QStringList &
LanguagePlugin::languageCodes() const
{
    return m_languageCodes;
}

int
LanguagePlugin::currentLanguage() const
{
    return m_currentLanguage;
}

void
LanguagePlugin::setCurrentLanguage(int index)
{
    if (index >= 0 && index < m_languageCodes.length()) {
        m_nextCurrentLanguage = index;

        updateCurrentLanguage();
    }
}

SubsetModel *
LanguagePlugin::spellCheckingModel()
{
    return &m_spellCheckingModel;
}

void
LanguagePlugin::spellCheckingModelChanged()
{
    // TODO: update spell checking model
}

static QFontDatabase::WritingSystem localeScript(const QLocale &locale) {
    /* Unfortunately Qt has two different enums for language scripts:
     * QLocale::Script and QFontDatabase::WritingSystem, and their values do
     * not match.
     * This function converts the QLocale one into the QFontDatabase one.
     */
    switch (locale.script()) {
    case QLocale::LatinScript: return QFontDatabase::Latin;
    case QLocale::GreekScript: return QFontDatabase::Greek;
    case QLocale::CyrillicScript: return QFontDatabase::Cyrillic;
    case QLocale::ArmenianScript: return QFontDatabase::Armenian;
    case QLocale::HebrewScript: return QFontDatabase::Hebrew;
    case QLocale::ArabicScript: return QFontDatabase::Arabic;
    case QLocale::SyriacScript: return QFontDatabase::Syriac;
    case QLocale::ThaanaScript: return QFontDatabase::Thaana;
    case QLocale::DevanagariScript: return QFontDatabase::Devanagari;
    case QLocale::BengaliScript: return QFontDatabase::Bengali;
    case QLocale::GurmukhiScript: return QFontDatabase::Gurmukhi;
    case QLocale::GujaratiScript: return QFontDatabase::Gujarati;
    case QLocale::OriyaScript: return QFontDatabase::Oriya;
    case QLocale::TamilScript: return QFontDatabase::Tamil;
    case QLocale::TeluguScript: return QFontDatabase::Telugu;
    case QLocale::KannadaScript: return QFontDatabase::Kannada;
    case QLocale::MalayalamScript: return QFontDatabase::Malayalam;
    case QLocale::SinhalaScript: return QFontDatabase::Sinhala;
    case QLocale::ThaiScript: return QFontDatabase::Thai;
    case QLocale::LaoScript: return QFontDatabase::Lao;
    case QLocale::TibetanScript: return QFontDatabase::Tibetan;
    case QLocale::MyanmarScript: return QFontDatabase::Myanmar;
    case QLocale::GeorgianScript: return QFontDatabase::Georgian;
    case QLocale::KhmerScript: return QFontDatabase::Khmer;
    case QLocale::SimplifiedChineseScript: return QFontDatabase::SimplifiedChinese;
    case QLocale::TraditionalChineseScript: return QFontDatabase::TraditionalChinese;
    case QLocale::JapaneseScript: return QFontDatabase::Japanese;
    case QLocale::KoreanScript: return QFontDatabase::Korean;
    case QLocale::OghamScript: return QFontDatabase::Ogham;
    case QLocale::RunicScript: return QFontDatabase::Runic;
    case QLocale::NkoScript: return QFontDatabase::Nko;
    default:
        qWarning() << "Unsupported QLocale script" <<
            QLocale::scriptToString(locale.script());
        return QFontDatabase::Any;
    }
}

static QSet<QString> parseLocaleOutput(const QByteArray &localeOutput)
{
    const QStringList lines = QString::fromUtf8(localeOutput).split('\n');
    QSet<QString> localeNames;
    for (QString line: lines) {
        if (line.isEmpty()) continue;
        /* "locale -a" returns lines like "bg_BG.utf8", but AccountsService
         * wants "bg_BG.UTF-8" */
        localeNames.insert(line.replace("utf8", "UTF-8"));
    }
    return localeNames;
}

void
LanguagePlugin::updateLanguageNamesAndCodes()
{
    m_languageNames.clear();
    m_languageCodes.clear();
    m_indicesByLocale.clear();

    /* Run "locale -a" to get the list of supported locales; we run it in the
     * background, while we compute a list of supported languages (having
     * translations, and for which we have an available font). Then we'll
     * intersect the results.
     */
    QProcess localeProcess;
    localeProcess.start("locale", { "-a" });

    const QByteArray localeRoot = qgetenv("SNAP") + "/usr/share/locale";
    QDir localeDir(localeRoot);

    const QStringList locales = localeDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);

    /* Get the list of writing systems supported by our fonts. Unfortunately we
     * have to exclude all unsupported locales, because text written in their
     * script would be unprintable. */
    const QFontDatabase fontDb;
    const QList<QFontDatabase::WritingSystem> supportedWritingSystems =
        fontDb.writingSystems();

    QStringList tmpLocales {
        "en_US.UTF-8", // That's the native language of our UI texts
        "en_CA.UTF-8", // This doesn't have any translation, but we still want it
    };
    Q_FOREACH(const QString &localeName, locales) {
        QFileInfo systemSettingsTranslations(
            localeDir,
            localeName + "/LC_MESSAGES/" GETTEXT_PACKAGE ".mo");
        if (!systemSettingsTranslations.exists()) {
            /* If the locale doesn't even have a translation for the System
             * Settings, it's not worth considering.
             */
            continue;
        }
        if (systemSettingsTranslations.size() < 10000) {
            /* The check on the size is to exclude those languages whose
             * translation is mostly incomplete: while it's true that the size
             * depends on the encoding and other factors, most of the
             * translation files are at least 20KB in size (with the most
             * complete exceeding 40 or 50 KB); so excluding those languages
             * for which the translation file weighs less than 10K seems
             * appropriate.
             */
            qDebug() << "Skipping locale" << localeName << "as not fully translated";
            continue;
        }

        QLocale tmpLoc(localeName == "pt" ? "pt_PT" : localeName); // "pt" work around for https://bugreports.qt.io/browse/QTBUG-47891
        if (!supportedWritingSystems.contains(localeScript(tmpLoc))) {
            qDebug() << "Skipping locale" << localeName << "as we lack a font for it";
            continue;
        }
        tmpLocales.append(tmpLoc.name() + QStringLiteral(".UTF-8"));
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QSet<QString> localeNames = tmpLocales.toSet();
#else
    QSet<QString> localeNames = QSet<QString>(tmpLocales.begin(), tmpLocales.end());
#endif
    QList<LanguageLocale> languageLocales;

    /* Collect the result from "locale -a" and intersect */
    if (!localeProcess.waitForFinished(10000)) {
        qWarning() << "Running `locale -a` took more than 10 seconds!";
        return;
    }
    const QSet<QString> availableLocales =
        parseLocaleOutput(localeProcess.readAllStandardOutput());
    localeNames = localeNames.intersect(availableLocales);

    Q_FOREACH(const QString &loc, localeNames) {
        LanguageLocale languageLocale(loc);

        // Filter out locales for which we have no display name.
        if (languageLocale.displayName.isEmpty())
            continue;

        // Ignore "C"
        // https://github.com/ubports/ubports-touch/issues/182
        if (languageLocale.displayName == "C")
            continue;

        QLocale tmpLoc(languageLocale.locale.getLanguage());
        languageLocale.likely = tmpLoc.name() == loc.left(loc.indexOf('.')) || // likely if: en_US -> en -> en_US, NOT likely if: en_GB -> en -> en_US
                (loc.startsWith("pt_PT") && !loc.startsWith("pt_BR")); // "pt" work around for https://bugreports.qt.io/browse/QTBUG-47891

        languageLocales += languageLocale;
    }
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    qSort(languageLocales);
#else
    std::sort(languageLocales.begin(), languageLocales.end());
#endif

    for (int i(0); i < languageLocales.length(); i++) {
        const LanguageLocale &languageLocale(languageLocales[i]);

        m_languageNames += languageLocale.displayName;
        m_languageCodes += languageLocale.localeName;

        QString localeName(languageLocale.localeName);
        localeName = localeName.left(localeName.indexOf('.'));
        m_indicesByLocale.insert(localeName, i);

        if (languageLocale.likely) {
            localeName = localeName.left(localeName.indexOf('_'));
            m_indicesByLocale.insert(localeName, i);
        }
    }
}

void
LanguagePlugin::updateCurrentLanguage()
{
    int previousLanguage(m_currentLanguage);

    if (m_user != nullptr && act_user_is_loaded(m_user)) {
        if (m_nextCurrentLanguage >= 0) {
            m_currentLanguage = m_nextCurrentLanguage;
            m_nextCurrentLanguage = -1;

            QString formatsLocale(m_languageCodes[m_currentLanguage]);
            QString language(formatsLocale.left(formatsLocale.indexOf('.')));
            act_user_set_language(m_user, qPrintable(language));
            act_user_set_formats_locale(m_user, qPrintable(formatsLocale));
        } else {
            QString formatsLocale(act_user_get_formats_locale(m_user));
            m_currentLanguage = indexForLocale(formatsLocale);

            if (m_currentLanguage < 0) {
                QString language(act_user_get_language(m_user));
                m_currentLanguage = indexForLocale(language);
            }
        }
    }

    if (m_currentLanguage < 0)
        m_currentLanguage = indexForLocale(QLocale::system().name());

    if (m_currentLanguage != previousLanguage)
        Q_EMIT currentLanguageChanged();
}


void
LanguagePlugin::updateSpellCheckingModel()
{
    // TODO: populate spell checking model
    QVariantList superset;

    for (QStringList::const_iterator
         i(m_languageNames.begin()); i != m_languageNames.end(); ++i) {
        QVariantList element;
        element += *i;
        superset += QVariant(element);
    }

    m_spellCheckingModel.setCustomRoles(QStringList("language"));
    m_spellCheckingModel.setSuperset(superset);
    m_spellCheckingModel.setSubset(QList<int>());
    m_spellCheckingModel.setAllowEmpty(false);

    connect(&m_spellCheckingModel,
            SIGNAL(subsetChanged()), SLOT(spellCheckingModelChanged()));
}

int
LanguagePlugin::indexForLocale(const QString &name) const
{
    return m_indicesByLocale.value(name.left(name.indexOf('.')), -1);
}

void
LanguagePlugin::userLoaded()
{
    if (act_user_is_loaded(m_user)) {
        g_signal_handlers_disconnect_by_data(m_user, this);

        updateCurrentLanguage();
    }
}

void
userLoaded(GObject    *object,
           GParamSpec *pspec,
           gpointer    user_data)
{
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    LanguagePlugin *plugin(static_cast<LanguagePlugin *>(user_data));
    plugin->userLoaded();
}

void
LanguagePlugin::managerLoaded()
{
    gboolean loaded;
    g_object_get(m_manager, "is-loaded", &loaded, nullptr);

    if (loaded) {
        g_signal_handlers_disconnect_by_data(m_manager, this);

        m_user = act_user_manager_get_user_by_id(m_manager, geteuid());

        if (m_user != nullptr) {
            g_object_ref(m_user);

            if (act_user_is_loaded(m_user))
                userLoaded();
            else
                g_signal_connect(m_user, "notify::is-loaded",
                                 G_CALLBACK(::userLoaded), this);
        }
    }
}

void
managerLoaded(GObject    *object,
              GParamSpec *pspec,
              gpointer    user_data)
{
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    LanguagePlugin *plugin(static_cast<LanguagePlugin *>(user_data));
    plugin->managerLoaded();
}

void LanguagePlugin::reboot()
{
    m_sessionService.reboot();
}
