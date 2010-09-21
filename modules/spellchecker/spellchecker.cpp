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

#define ASPELL_STATIC
#include <aspell.h>

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

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

SpellChecker* spellcheck;

extern "C" KADU_EXPORT int spellchecker_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	spellcheck = new SpellChecker();

	// use configuration settings to create spellcheckers for languages
	if (!spellcheck->buildCheckers())
	{
		delete spellcheck;
		return 1;
	}
	else
	{
		MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/spellchecker.ui"));
		MainConfigurationWindow::registerUiHandler(spellcheck);
		return 0;
	}
}

extern "C" KADU_EXPORT void spellchecker_close()
{
	if (spellcheck)
	{
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/spellchecker.ui"));
		MainConfigurationWindow::unregisterUiHandler(spellcheck);
		delete spellcheck;
	}
}

SpellChecker::SpellChecker()
{
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));

	// prepare configuration of spellchecker
	spellConfig = new_aspell_config();
	aspell_config_replace(spellConfig, "encoding", "utf-8");

#ifdef Q_OS_WIN32
	aspell_config_replace(spellConfig, "dict-dir", qPrintable(dataPath("aspell/dict")));
	aspell_config_replace(spellConfig, "data-dir", qPrintable(dataPath("aspell/data")));
	aspell_config_replace(spellConfig, "prefix", qPrintable(profilePath("dicts")));
#endif

	createDefaultConfiguration();
	// load mark settings
	buildMarkTag();
}

SpellChecker::~SpellChecker()
{
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));

	delete_aspell_config(spellConfig);

	foreach(AspellSpeller *speller, checkers.values())
		delete_aspell_speller(speller);
}

QStringList SpellChecker::notCheckedLanguages()
{
	QStringList result;
	AspellDictInfoList* dlist;
	AspellDictInfoEnumeration* dels;
	const AspellDictInfo* entry;

	/* the returned pointer should _not_ need to be deleted */
	dlist = get_aspell_dict_info_list(spellConfig);

	dels = aspell_dict_info_list_elements(dlist);
	while ((entry = aspell_dict_info_enumeration_next(dels)) != 0)
	{
		if (checkers.find(entry->name) == checkers.end())
			result.push_back(entry->name);
	}
	delete_aspell_dict_info_enumeration(dels);

	return result;
}

QStringList SpellChecker::checkedLanguages()
{
	QStringList result;
	for (Checkers::Iterator it = checkers.begin(); it != checkers.end(); it++)
		result.append(it.key());

	return result;
}

bool SpellChecker::addCheckedLang(QString &name)
{
	if (checkers.find(name) != checkers.end())
		return true;

	aspell_config_replace(spellConfig, "lang", name.toAscii());

	// create spell checker using prepared configuration
	AspellCanHaveError* possibleErr = new_aspell_speller(spellConfig);
	if (aspell_error_number(possibleErr) != 0)
	{
		MessageDialog::msg(aspell_error_message(possibleErr));
		return false;
	}
	else
		checkers[name] = to_aspell_speller(possibleErr);

	if (checkers.size() == 1)
	{
		foreach(ChatWidget *chat, ChatWidgetManager::instance()->chats())
			chatCreated(chat);
	}

	return true;
}

void SpellChecker::removeCheckedLang(QString &name)
{
	Checkers::Iterator checker = checkers.find(name);
	if (checker != checkers.end())
	{
		delete_aspell_speller(checker.value());
		checkers.remove(name);
	}
}

bool SpellChecker::buildCheckers()
{
	foreach(AspellSpeller *speller, checkers.values())
		delete_aspell_speller(speller);

	checkers.clear();

	// load languages to check from configuration
	QString checkedStr = config_file.readEntry("ASpell", "Checked", "pl");
	QStringList checkedList = checkedStr.split(',', QString::SkipEmptyParts);

	if (config_file.readBoolEntry("ASpell", "Accents", false))
		aspell_config_replace(spellConfig, "ignore-accents", "true");
	else
		aspell_config_replace(spellConfig, "ignore-accents", "false");

	if (config_file.readBoolEntry( "ASpell", "Case", false))
		aspell_config_replace(spellConfig, "ignore-case", "true");
	else
		aspell_config_replace(spellConfig, "ignore-case", "false");

	// create aspell checkers for each language
	for (int i = 0; i < checkedList.count(); i++)
	{
		addCheckedLang(checkedList[i]);
		/*
		if ( !addCheckedLang( checkedList[i] ) )
		{
			delete_aspell_config( spellConfig );
			delete config;
			return false;
		}
		*/
	}
	return true;
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
	if (checkers.size() > 0)
		new Highlighter(chat->edit()->document());
}

void SpellChecker::configForward()
{
	if (availList->selectedItems().count() > 0)
		configForward2(availList->selectedItems()[0]);
}

void SpellChecker::configBackward()
{
	if (checkList->selectedItems().count() > 0)
		configBackward2(checkList->selectedItems()[0]);
}

void SpellChecker::configForward2(QListWidgetItem *it)
{
	QString langName = it->text();
	if (addCheckedLang(langName))
	{
		checkList->addItem(langName);
		delete availList->takeItem(availList->row(it));
	}
}

void SpellChecker::configBackward2(QListWidgetItem *it)
{
	QString langName = it->text();
	availList->addItem(langName);
	delete checkList->takeItem(checkList->row(it));
	removeCheckedLang(langName);
}

void SpellChecker::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	ConfigGroupBox *optionsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "SpellChecker", tr("ASpell options"));

	QWidget *options = new QWidget(optionsGroupBox->widget());
	QGridLayout *optionsLayout = new QGridLayout(options);

	availList = new QListWidget(options);
	QPushButton *moveToCheckList = new QPushButton(tr("Move to 'Checked'"), options);

	optionsLayout->addWidget(new QLabel(tr("Available languages"), options), 0, 0);
	optionsLayout->addWidget(availList, 1, 0);
	optionsLayout->addWidget(moveToCheckList, 2, 0);

	checkList = new QListWidget(options);
	QPushButton *moveToAvailList = new QPushButton(tr("Move to 'Available languages'"), options);

	optionsLayout->addWidget(new QLabel(tr("Checked"), options), 0, 1);
	optionsLayout->addWidget(checkList, 1, 1);
	optionsLayout->addWidget(moveToAvailList, 2, 1);

	connect(moveToCheckList, SIGNAL(clicked()), this, SLOT(configForward()));
	connect(moveToAvailList, SIGNAL(clicked()), this, SLOT(configBackward()));
	connect(checkList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(configBackward2(QListWidgetItem *)));
	connect(availList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(configForward2(QListWidgetItem*)));

	optionsGroupBox->addWidgets(0, options);

	availList->setSelectionMode(QAbstractItemView::SingleSelection);
	checkList->setSelectionMode(QAbstractItemView::SingleSelection);
	availList->addItems(notCheckedLanguages());
	checkList->addItems(checkedLanguages());
}

void SpellChecker::configurationUpdated()
{
	buildMarkTag();
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

bool SpellChecker::checkWord(QString word)
{
	bool isWordValid = checkers.size() == 0;
	if (QRegExp("\\D").indexIn(word) == -1)
		isWordValid = true;
	else
	{
		for (Checkers::Iterator it = checkers.begin(); it != checkers.end(); it++)
		{
			if (aspell_speller_check(it.value(), word.toUtf8(), -1))
			{
				isWordValid = true;
				break;
			}
		}
	}

	return isWordValid;
}
