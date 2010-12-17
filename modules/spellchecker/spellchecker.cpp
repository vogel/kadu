/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifdef HAVE_ASPELL
#define ASPELL_STATIC
#include <aspell.h>
#else
#include <enchant++.h>
#endif

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"

#include "highlighter.h"

#include "spellchecker.h"

SpellChecker *spellcheck;

extern "C" KADU_EXPORT int spellchecker_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	spellcheck = new SpellChecker();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/spellchecker.ui"));
	MainConfigurationWindow::registerUiHandler(spellcheck);
	return 0;
}

extern "C" KADU_EXPORT void spellchecker_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/spellchecker.ui"));
	MainConfigurationWindow::unregisterUiHandler(spellcheck);
	delete spellcheck;
	spellcheck = 0;
}

#ifdef HAVE_ENCHANT
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
#endif

SpellChecker::SpellChecker()
{
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)),
			this, SLOT(chatCreated(ChatWidget *)));

#ifdef HAVE_ASPELL
	// prepare configuration of spellchecker
	SpellConfig = new_aspell_config();
	aspell_config_replace(SpellConfig, "encoding", "utf-8");

#ifdef Q_OS_WIN32
	aspell_config_replace(SpellConfig, "dict-dir", qPrintable(dataPath("aspell/dict")));
	aspell_config_replace(SpellConfig, "data-dir", qPrintable(dataPath("aspell/data")));
	aspell_config_replace(SpellConfig, "prefix", qPrintable(profilePath("dicts")));
#endif // Q_OS_WIN32
#endif // HAVE_ASPELL

	createDefaultConfiguration();
	configurationUpdated();
}

SpellChecker::~SpellChecker()
{
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)),
			this, SLOT(chatCreated(ChatWidget *)));

	Highlighter::removeAll();

#ifdef HAVE_ASPELL
	delete_aspell_config(SpellConfig);

	foreach (AspellSpeller *speller, MyCheckers)
		delete_aspell_speller(speller);
#else
	qDeleteAll(MyCheckers);
#endif
}

QStringList SpellChecker::notCheckedLanguages()
{
	QStringList result;

#ifdef HAVE_ASPELL
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
#else
	DescWrapper aWrapper(&MyCheckers, &result);
	enchant::Broker::instance()->list_dicts(enchantDictDescribe, &aWrapper);
#endif

	return result;
}

QStringList SpellChecker::checkedLanguages()
{
	QStringList result;
	for (Checkers::const_iterator it = MyCheckers.constBegin(); it != MyCheckers.constEnd(); ++it)
		result.append(it.key());
	return result;
}

bool SpellChecker::addCheckedLang(const QString &name)
{
	if (MyCheckers.contains(name))
		return true;

#ifdef HAVE_ASPELL
	aspell_config_replace(SpellConfig, "lang", name.toAscii().constData());

	// create spell checker using prepared configuration
	AspellCanHaveError *possibleErr = new_aspell_speller(SpellConfig);
	if (aspell_error_number(possibleErr) != 0)
	{
		MessageDialog::show("dialog-error", tr("Kadu"), aspell_error_message(possibleErr));
		return false;
	}
	else
		MyCheckers[name] = to_aspell_speller(possibleErr);
#else
	try
	{
		MyCheckers[name] = enchant::Broker::instance()->request_dict(name.toStdString());
	}
	catch (enchant::Exception &e)
	{
		MessageDialog::show("dialog-error", tr("Kadu"), e.what());
		return false;
	}
#endif

	if (MyCheckers.size() == 1)
		foreach (ChatWidget *chat, ChatWidgetManager::instance()->chats())
			chatCreated(chat);

	return true;
}

void SpellChecker::removeCheckedLang(const QString &name)
{
	Checkers::iterator checker = MyCheckers.find(name);
	if (checker != MyCheckers.end())
	{
#ifdef HAVE_ASPELL
		delete_aspell_speller(checker.value());
#else
		delete checker.value();
#endif
		MyCheckers.erase(checker);
	}
}

void SpellChecker::buildCheckers()
{
#ifdef HAVE_ASPELL
	foreach (AspellSpeller *speller, MyCheckers)
		delete_aspell_speller(speller);
#else
	qDeleteAll(MyCheckers);
#endif
	MyCheckers.clear();

#ifdef HAVE_ASPELL
	if (config_file.readBoolEntry("ASpell", "Accents", false))
		aspell_config_replace(SpellConfig, "ignore-accents", "true");
	else
		aspell_config_replace(SpellConfig, "ignore-accents", "false");

	if (config_file.readBoolEntry("ASpell", "Case", false))
		aspell_config_replace(SpellConfig, "ignore-case", "true");
	else
		aspell_config_replace(SpellConfig, "ignore-case", "false");
#endif

	// load languages to check from configuration
	QString checkedStr = config_file.readEntry("ASpell", "Checked", "pl");
	QStringList checkedList = checkedStr.split(',', QString::SkipEmptyParts);

	// create spell checkers for each language
	for (int i = 0; i < checkedList.count(); i++)
		addCheckedLang(checkedList[i]);
}

void SpellChecker::buildMarkTag()
{
	QTextCharFormat format;

	QColor colorMark("#FF0101");
	colorMark = config_file.readColorEntry("ASpell", "Color", &colorMark);

	if (config_file.readBoolEntry("ASpell", "Bold", false))
		format.setFontWeight(600);
	if (config_file.readBoolEntry("ASpell", "Italic", false))
		format.setFontItalic(true);
	if (config_file.readBoolEntry("ASpell", "Underline", false))
	{
		format.setFontUnderline(true);
		format.setUnderlineColor(colorMark);
		format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	}
	format.setForeground(QBrush(colorMark));

	Highlighter::setHighlightFormat(format);
	Highlighter::rehighlightAll();
}

void SpellChecker::chatCreated(ChatWidget *chat)
{
	if (!MyCheckers.isEmpty())
		new Highlighter(chat->edit()->document());
}

void SpellChecker::configForward()
{
	if (!AvailableLanguagesList->selectedItems().isEmpty())
		configForward2(AvailableLanguagesList->selectedItems()[0]);
}

void SpellChecker::configBackward()
{
	if (!CheckedLanguagesList->selectedItems().isEmpty())
		configBackward2(CheckedLanguagesList->selectedItems()[0]);
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

#ifndef HAVE_ASPELL
	mainConfigurationWindow->widget()->widgetById("spellchecker/ignoreCase")->hide();
#endif

	ConfigGroupBox *optionsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "SpellChecker", qApp->translate("@default", "Spell Checker Options"));

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

void SpellChecker::configurationUpdated()
{
	buildMarkTag();
	buildCheckers();
}

void SpellChecker::configurationWindowApplied()
{
	config_file.writeEntry("ASpell", "Checked", checkedLanguages().join(","));
}

void SpellChecker::createDefaultConfiguration()
{
	config_file.addVariable("ASpell", "Bold", "false");
	config_file.addVariable("ASpell", "Italic", "false");
	config_file.addVariable("ASpell", "Underline", "true");
	config_file.addVariable("ASpell", "Color", "#FF0101");
	config_file.addVariable("ASpell", "Checked", "pl");
	config_file.addVariable("ASpell", "Accents", "false");
	config_file.addVariable("ASpell", "Case", "false");
}

bool SpellChecker::checkWord(const QString &word)
{
	if (MyCheckers.isEmpty())
		return true;

	bool isWordValid = false;
	if (!word.contains(QRegExp("\\D")))
		isWordValid = true;
	else
		for (Checkers::const_iterator it = MyCheckers.constBegin(); it != MyCheckers.constEnd(); ++it)
#ifdef HAVE_ASPELL
			if (aspell_speller_check(it.value(), word.toUtf8().constData(), -1))
#else
			if (it.value()->check(word.toUtf8().constData()))
#endif
			{
				isWordValid = true;
				break;
			}

	return isWordValid;
}
