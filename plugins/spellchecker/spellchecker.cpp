/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

#if defined(HAVE_ASPELL)
#define ASPELL_STATIC
#include <aspell.h>
#elif defined(HAVE_ENCHANT)
#include <enchant.h>
#endif

#include "misc/paths-provider.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-widget/chat-widget-repository.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/configuration/config-group-box.h"
#include "widgets/configuration/configuration-widget.h"
#include "widgets/custom-input.h"
#include "windows/main-configuration-window.h"
#include "windows/message-dialog.h"

#include "configuration/spellchecker-configuration.h"
#include "highlighter.h"
#include "suggester.h"

#include "spellchecker.h"
#include "spellchecker.moc"

#if defined(HAVE_ENCHANT)
typedef std::pair<const SpellChecker::Checkers *, QStringList *> DescWrapper;

static void enchantDictDescribe(
    const char *const langTag, const char *const providerName, const char *const providerDesc,
    const char *const providerFile, void *userData)
{
    Q_UNUSED(providerName)
    Q_UNUSED(providerDesc)
    Q_UNUSED(providerFile)

    DescWrapper *pWrapper = static_cast<DescWrapper *>(userData);
    const SpellChecker::Checkers &checkers = *pWrapper->first;
    QStringList &result = *pWrapper->second;
    if (!checkers.contains(langTag))
        result.append(langTag);
}

static void enchantUsedDictDescribe(
    const char *const langTag, const char *const providerName, const char *const providerDesc,
    const char *const providerFile, void *userData)
{
    Q_UNUSED(providerName)
    Q_UNUSED(providerDesc)
    Q_UNUSED(providerFile)

    DescWrapper *pWrapper = static_cast<DescWrapper *>(userData);
    const SpellChecker::Checkers &checkers = *pWrapper->first;
    QStringList &result = *pWrapper->second;
    if (checkers.contains(langTag))
        result.append(langTag);
}
#endif

SpellChecker::SpellChecker(QObject *parent) : QObject{parent}
{
}

SpellChecker::~SpellChecker()
{
    if (m_chatWidgetRepository)
        disconnect(m_chatWidgetRepository.data(), 0, this, 0);

    Highlighter::removeAll();

#if defined(HAVE_ASPELL)
    for (auto speller : MyCheckers)
        delete_aspell_speller(speller);
    delete_aspell_config(SpellConfig);
#elif defined(HAVE_ENCHANT)
    for (auto dict : MyCheckers)
        enchant_broker_free_dict(Broker, dict);
    enchant_broker_free(Broker);
#endif
}

void SpellChecker::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
    m_chatWidgetRepository = chatWidgetRepository;
}

void SpellChecker::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void SpellChecker::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void SpellChecker::setSpellcheckerConfiguration(SpellcheckerConfiguration *spellcheckerConfiguration)
{
    m_spellcheckerConfiguration = spellcheckerConfiguration;
}

void SpellChecker::setSuggester(Suggester *suggester)
{
    m_suggester = suggester;
}

void SpellChecker::init()
{
    connect(
        m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget *)), this,
        SLOT(chatWidgetAdded(ChatWidget *)));
    connect(m_spellcheckerConfiguration, SIGNAL(updated()), this, SLOT(configurationUpdated()));

#if defined(HAVE_ASPELL)
    // prepare configuration of spellchecker
    SpellConfig = new_aspell_config();
    aspell_config_replace(SpellConfig, "encoding", "utf-8");
    aspell_config_replace(SpellConfig, "sug-mode", "ultra");

#if defined(Q_OS_WIN)
    aspell_config_replace(SpellConfig, "dict-dir", qPrintable(m_pathsProvider->dataPath() + QStringLiteral("aspell")));
    aspell_config_replace(SpellConfig, "data-dir", qPrintable(m_pathsProvider->dataPath() + QStringLiteral("aspell")));
    aspell_config_replace(SpellConfig, "prefix", qPrintable(m_pathsProvider->profilePath() + QStringLiteral("dicts")));
#endif
#elif defined(HAVE_ENCHANT)
    Broker = enchant_broker_init();
#if defined(Q_OS_WIN)
    enchant_broker_set_param(
        Broker, "enchant.myspell.dictionary.path",
        qPrintable(m_pathsProvider->dataPath() + QStringLiteral("share/enchant/myspell/")));
#endif
#endif

    configurationUpdated();
}

void SpellChecker::configurationUpdated()
{
    buildCheckers();
    buildMarkTag();
}

void SpellChecker::buildCheckers()
{
#if defined(HAVE_ASPELL)
    for (auto speller : MyCheckers)
        delete_aspell_speller(speller);
#elif defined(HAVE_ENCHANT)
    for (auto dict : MyCheckers)
        enchant_broker_free_dict(Broker, dict);
#endif
    MyCheckers.clear();

#if defined(HAVE_ASPELL)
    if (m_spellcheckerConfiguration->accents())
        aspell_config_replace(SpellConfig, "ignore-accents", "true");
    else
        aspell_config_replace(SpellConfig, "ignore-accents", "false");

    if (m_spellcheckerConfiguration->casesens())
        aspell_config_replace(SpellConfig, "ignore-case", "true");
    else
        aspell_config_replace(SpellConfig, "ignore-case", "false");
#endif

    for (auto checked : m_spellcheckerConfiguration->checked())
        addCheckedLang(checked);
}

QStringList SpellChecker::checkedLanguages() const
{
    QStringList result;

#if defined(HAVE_ASPELL)
    AspellDictInfoList *dlist;
    AspellDictInfoEnumeration *dels;
    const AspellDictInfo *entry;

    /* the returned pointer should _not_ need to be deleted */
    dlist = get_aspell_dict_info_list(SpellConfig);

    dels = aspell_dict_info_list_elements(dlist);
    while ((entry = aspell_dict_info_enumeration_next(dels)))
        if (MyCheckers.contains(entry->name))
            result.push_back(entry->name);
    delete_aspell_dict_info_enumeration(dels);
#elif defined(HAVE_ENCHANT)
    DescWrapper aWrapper(&MyCheckers, &result);
    enchant_broker_list_dicts(Broker, enchantUsedDictDescribe, &aWrapper);
#endif

    return result;
}

QStringList SpellChecker::notCheckedLanguages() const
{
    QStringList result;

#if defined(HAVE_ASPELL)
    AspellDictInfoList *dlist;
    AspellDictInfoEnumeration *dels;
    const AspellDictInfo *entry;

    /* the returned pointer should _not_ need to be deleted */
    dlist = get_aspell_dict_info_list(SpellConfig);

    dels = aspell_dict_info_list_elements(dlist);
    while ((entry = aspell_dict_info_enumeration_next(dels)))
        if (!MyCheckers.contains(entry->name))
            result.push_back(entry->name);
    delete_aspell_dict_info_enumeration(dels);
#elif defined(HAVE_ENCHANT)
    DescWrapper aWrapper(&MyCheckers, &result);
    enchant_broker_list_dicts(Broker, enchantDictDescribe, &aWrapper);
#endif

    return result;
}

bool SpellChecker::addCheckedLang(const QString &name)
{
    if (MyCheckers.contains(name))
        return true;

    bool ok = true;
    const char *errorMsg = 0;

#if defined(HAVE_ASPELL)
    aspell_config_replace(SpellConfig, "lang", name.toUtf8().constData());

    // create spell checker using prepared configuration
    AspellCanHaveError *possibleErr = new_aspell_speller(SpellConfig);
    if (aspell_error_number(possibleErr) == 0)
        MyCheckers.insert(name, to_aspell_speller(possibleErr));
    else
    {
        errorMsg = aspell_error_message(possibleErr);
        ok = false;
    }
#elif defined(HAVE_ENCHANT)
    EnchantDict *dict = enchant_broker_request_dict(Broker, name.toUtf8().constData());
    if (dict)
        MyCheckers.insert(name, dict);
    else
    {
        errorMsg = enchant_broker_get_error(Broker);
        ok = false;
    }
#endif

    if (!ok)
    {
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-error")), tr("Kadu"),
            tr("Could not find dictionary for %1 language.").arg(name) +
                (qstrlen(errorMsg) > 0 ? QString(" %1: %2").arg(tr("Details"), errorMsg) : QString()));
        return false;
    }

    if ((MyCheckers.size() == 1) && m_chatWidgetRepository)
        for (ChatWidget *chatWidget : m_chatWidgetRepository.data())
            chatWidgetAdded(chatWidget);

    return true;
}

void SpellChecker::removeCheckedLang(const QString &name)
{
    Checkers::iterator checker = MyCheckers.find(name);
    if (checker != MyCheckers.end())
    {
#if defined(HAVE_ASPELL)
        delete_aspell_speller(checker.value());
#elif defined(HAVE_ENCHANT)
        enchant_broker_free_dict(Broker, checker.value());
#endif
        MyCheckers.erase(checker);
    }
}

void SpellChecker::buildMarkTag()
{
    if (!m_spellcheckerConfiguration)
        return;

    QTextCharFormat format;

    if (m_spellcheckerConfiguration->bold())
        format.setFontWeight(600);
    if (m_spellcheckerConfiguration->italic())
        format.setFontItalic(true);
    if (m_spellcheckerConfiguration->underline())
    {
        format.setFontUnderline(true);
        format.setUnderlineColor(m_spellcheckerConfiguration->color());
        format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    }
    format.setForeground(QBrush(m_spellcheckerConfiguration->color()));

    Highlighter::setHighlightFormat(format);
    Highlighter::rehighlightAll();
}

void SpellChecker::chatWidgetAdded(ChatWidget *chat)
{
    if (!MyCheckers.isEmpty())
    {
        chat->getChatEditBox()->inputBox()->installEventFilter(m_suggester);
        new Highlighter(this, chat->edit()->document());
    }
}

bool SpellChecker::checkWord(const QString &word)
{
    bool isWordValid = false;

    if (MyCheckers.isEmpty())
        return true;

    if (!word.contains(QRegExp("\\D")))
        isWordValid = true;
    else
        for (Checkers::const_iterator it = MyCheckers.constBegin(); it != MyCheckers.constEnd(); ++it)
        {
#if defined(HAVE_ASPELL)
            if (aspell_speller_check(it.value(), word.toUtf8().constData(), -1))
#elif defined(HAVE_ENCHANT)
            QByteArray utf8Word = word.toUtf8();
            if (0 == enchant_dict_check(it.value(), utf8Word.constData(), utf8Word.size()))
#endif
            {
                isWordValid = true;
                break;
            }
        }
    return isWordValid;
}

QStringList SpellChecker::buildSuggestList(const QString &word)
{
    QStringList suggestWordList;

    int suggesterWordCount = m_spellcheckerConfiguration->suggesterWordCount();
    if (MyCheckers.size() > suggesterWordCount)
        suggesterWordCount = 1;
    else
        suggesterWordCount /= MyCheckers.size();

    int wordsForLanguage = 0;
    for (Checkers::const_iterator it = MyCheckers.constBegin(); it != MyCheckers.constEnd(); ++it)
    {
        wordsForLanguage = suggesterWordCount;
#if defined(HAVE_ASPELL)
        const AspellWordList *aspellTmpList = aspell_speller_suggest(it.value(), word.toUtf8().constData(), -1);

        if (!aspell_word_list_empty(aspellTmpList))
        {
            struct AspellStringEnumeration *aspellStringEnum = aspell_word_list_elements(aspellTmpList);

            while ((!aspell_string_enumeration_at_end(aspellStringEnum)) && wordsForLanguage)
            {
                if (MyCheckers.size() > 1)
                    suggestWordList.append(
                        QString::fromUtf8(aspell_string_enumeration_next(aspellStringEnum)) + " (" + it.key() + ")");
                else
                    suggestWordList.append(QString::fromUtf8(aspell_string_enumeration_next(aspellStringEnum)));

                --wordsForLanguage;
            }

            delete_aspell_string_enumeration(aspellStringEnum);
        }
#elif defined(HAVE_ENCHANT)
        size_t numberOfSuggs;
        QByteArray utf8Word = word.toUtf8();
        char **suggs = enchant_dict_suggest(it.value(), utf8Word.constData(), utf8Word.size(), &numberOfSuggs);

        if (suggs)
        {
            for (size_t i = 0; i < numberOfSuggs && wordsForLanguage; ++i)
            {
                if (MyCheckers.size() > 1)
                    suggestWordList.append(QString::fromUtf8(suggs[i]) + " (" + it.key() + ")");
                else
                    suggestWordList.append(QString::fromUtf8(suggs[i]));

                --wordsForLanguage;
            }

            enchant_dict_free_string_list(it.value(), suggs);
        }
#endif
    }

    return suggestWordList;
}
