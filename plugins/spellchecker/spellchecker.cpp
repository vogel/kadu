/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

#if defined(HAVE_ASPELL)
#define ASPELL_STATIC
#include <aspell.h>
#elif defined(HAVE_ENCHANT)
#include <enchant.h>
#elif defined(Q_OS_MAC)
#include "macspellchecker.h"
#endif

#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/message-dialog.h"
#include "misc/kadu-paths.h"

#include "configuration/spellchecker-configuration.h"
#include "highlighter.h"
#include "suggester.h"

#include "spellchecker.h"

#if defined(HAVE_ENCHANT)
typedef std::pair<SpellChecker::Checkers *, QStringList *> DescWrapper;

static void enchantDictDescribe(const char * const langTag, const char * const providerName,
		const char * const providerDesc, const char * const providerFile, void *userData)
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

static void enchantUsedDictDescribe(const char * const langTag, const char * const providerName,
		const char * const providerDesc, const char * const providerFile, void *userData)
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

SpellChecker::SpellChecker(QObject *parent) :
		ConfigurationUiHandler{parent},
		AvailableLanguagesList{},
		CheckedLanguagesList{}
{
#if defined(HAVE_ASPELL)
	// prepare configuration of spellchecker
	SpellConfig = new_aspell_config();
	aspell_config_replace(SpellConfig, "encoding", "utf-8");
	aspell_config_replace(SpellConfig, "sug-mode", "ultra");

#if defined(Q_OS_WIN32)
	aspell_config_replace(SpellConfig, "dict-dir", qPrintable(KaduPaths::instance()->dataPath() + QLatin1String("aspell")));
	aspell_config_replace(SpellConfig, "data-dir", qPrintable(KaduPaths::instance()->dataPath() + QLatin1String("aspell")));
	aspell_config_replace(SpellConfig, "prefix", qPrintable(KaduPaths::instance()->profilePath() + QLatin1String("dicts")));
#endif
#elif defined(HAVE_ENCHANT)
	Broker = enchant_broker_init();
#endif
}

SpellChecker::~SpellChecker()
{
	if (m_chatWidgetRepository)
		disconnect(m_chatWidgetRepository.data(), 0, this, 0);

	Highlighter::removeAll();

#if defined(HAVE_ASPELL)
	foreach (AspellSpeller *speller, MyCheckers)
		delete_aspell_speller(speller);
	delete_aspell_config(SpellConfig);
#elif defined(HAVE_ENCHANT)
	foreach (EnchantDict *dict, MyCheckers)
		enchant_broker_free_dict(Broker, dict);
	enchant_broker_free(Broker);
#elif defined(Q_OS_MAC)
	qDeleteAll(MyCheckers);
#endif
}

void SpellChecker::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (m_chatWidgetRepository)
		connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget *)), this, SLOT(chatWidgetAdded(ChatWidget *)));
}

QStringList SpellChecker::notCheckedLanguages()
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

QStringList SpellChecker::checkedLanguages()
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
#elif defined(Q_OS_MAC)
	MyCheckers.insert(name, new MacSpellChecker());
#endif

	if (!ok)
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("Could not find dictionary for %1 language.").arg(name)
				+ (qstrlen(errorMsg) > 0 ? QString(" %1: %2").arg(tr("Details"), errorMsg) : QString()));

		// remove this checker from configuration
		configurationWindowApplied();
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
#elif defined(Q_OS_MAC)
		delete checker.value();
#endif
		MyCheckers.erase(checker);
	}
}

void SpellChecker::buildCheckers()
{
#if defined(HAVE_ASPELL)
	foreach (AspellSpeller *speller, MyCheckers)
		delete_aspell_speller(speller);
#elif defined(HAVE_ENCHANT)
	foreach (EnchantDict *dict, MyCheckers)
		enchant_broker_free_dict(Broker, dict);
#elif defined(Q_OS_MAC)
	qDeleteAll(MyCheckers);
#endif
	MyCheckers.clear();

#if defined(HAVE_ASPELL)
	if (SpellcheckerConfiguration::instance()->accents())
		aspell_config_replace(SpellConfig, "ignore-accents", "true");
	else
		aspell_config_replace(SpellConfig, "ignore-accents", "false");

	if (SpellcheckerConfiguration::instance()->casesens())
		aspell_config_replace(SpellConfig, "ignore-case", "true");
	else
		aspell_config_replace(SpellConfig, "ignore-case", "false");
#endif

	foreach (const QString &checked, SpellcheckerConfiguration::instance()->checked())
		addCheckedLang(checked);
}

void SpellChecker::buildMarkTag()
{
	QTextCharFormat format;

	if (SpellcheckerConfiguration::instance()->bold())
		format.setFontWeight(600);
	if (SpellcheckerConfiguration::instance()->italic())
		format.setFontItalic(true);
	if (SpellcheckerConfiguration::instance()->underline())
	{
		format.setFontUnderline(true);
		format.setUnderlineColor(SpellcheckerConfiguration::instance()->color());
		format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	}
	format.setForeground(QBrush(SpellcheckerConfiguration::instance()->color()));

	Highlighter::setHighlightFormat(format);
	Highlighter::rehighlightAll();
}

void SpellChecker::chatWidgetAdded(ChatWidget *chat)
{
	if (!MyCheckers.isEmpty())
	{
		chat->getChatEditBox()->inputBox()->installEventFilter(Suggester::instance());
		new Highlighter(chat->edit()->document());
	}
}

void SpellChecker::configForward()
{
	if (!AvailableLanguagesList->selectedItems().isEmpty())
		configForward2(AvailableLanguagesList->selectedItems().at(0));
}

void SpellChecker::configBackward()
{
	if (!CheckedLanguagesList->selectedItems().isEmpty())
		configBackward2(CheckedLanguagesList->selectedItems().at(0));
}

void SpellChecker::configForward2(QListWidgetItem *item)
{
	QString langName = item->text();
	if (addCheckedLang(langName))
	{
		CheckedLanguagesList->addItem(langName);
		delete AvailableLanguagesList->takeItem(AvailableLanguagesList->row(item));
	}
}

void SpellChecker::configBackward2(QListWidgetItem *item)
{
	QString langName = item->text();
	AvailableLanguagesList->addItem(langName);
	delete CheckedLanguagesList->takeItem(CheckedLanguagesList->row(item));
	removeCheckedLang(langName);
}

void SpellChecker::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()),
			this, SLOT(configurationWindowApplied()));

#if !defined(HAVE_ASPELL)
	mainConfigurationWindow->widget()->widgetById("spellchecker/ignoreCase")->hide();
#endif

	ConfigGroupBox *optionsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "Spelling", "Spell Checker Options");

	QWidget *options = new QWidget(optionsGroupBox->widget());
	QGridLayout *optionsLayout = new QGridLayout(options);

	AvailableLanguagesList = new QListWidget(options);
	QPushButton *moveToChecked = new QPushButton(tr("Move to 'Checked'"), options);

	optionsLayout->addWidget(new QLabel(tr("Available languages"), options), 0, 0);
	optionsLayout->addWidget(AvailableLanguagesList, 1, 0);
	optionsLayout->addWidget(moveToChecked, 2, 0);

	CheckedLanguagesList = new QListWidget(options);
	QPushButton *moveToAvailable = new QPushButton(tr("Move to 'Available languages'"), options);

	optionsLayout->addWidget(new QLabel(tr("Checked"), options), 0, 1);
	optionsLayout->addWidget(CheckedLanguagesList, 1, 1);
	optionsLayout->addWidget(moveToAvailable, 2, 1);

	connect(moveToChecked, SIGNAL(clicked()), this, SLOT(configForward()));
	connect(moveToAvailable, SIGNAL(clicked()), this, SLOT(configBackward()));
	connect(CheckedLanguagesList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			this, SLOT(configBackward2(QListWidgetItem *)));
	connect(AvailableLanguagesList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			this, SLOT(configForward2(QListWidgetItem*)));

	optionsGroupBox->addWidgets(0, options);

	AvailableLanguagesList->setSelectionMode(QAbstractItemView::SingleSelection);
	CheckedLanguagesList->setSelectionMode(QAbstractItemView::SingleSelection);
	AvailableLanguagesList->addItems(notCheckedLanguages());
	CheckedLanguagesList->addItems(checkedLanguages());
}

void SpellChecker::configurationWindowApplied()
{
	SpellcheckerConfiguration::instance()->setChecked(checkedLanguages());
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
#elif defined(Q_OS_MAC)
			if (it.value()->isCorrect(word.toUtf8().constData()))
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

	int suggesterWordCount = SpellcheckerConfiguration::instance()->suggesterWordCount();
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

			while((!aspell_string_enumeration_at_end(aspellStringEnum)) && wordsForLanguage)
			{
				if (MyCheckers.size() > 1)
					suggestWordList.append(QString::fromUtf8(aspell_string_enumeration_next(aspellStringEnum)) + " (" + it.key() + ")");
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
#elif defined(Q_OS_MAC)
		suggestWordList.append(it.value()->suggestions(word));
#endif
	}

	return suggestWordList;
}

#include "moc_spellchecker.cpp"
