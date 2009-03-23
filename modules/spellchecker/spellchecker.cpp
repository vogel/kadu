/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 ***************************************************************************/
#define ASPELL_STATIC
#include <aspell.h>

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>

#include "chat_manager.h"
#include "chat_widget.h"
#include "config_file.h"
#include "custom_input.h"
#include "message_box.h"

#include "highlighter.h"
#include "spellchecker.h"

#define MODULE_SPELLCHECKER_VERSION 0.3

SpellChecker* spellcheck;

extern "C" KADU_EXPORT int spellchecker_init(bool firstLoad)
{
	spellcheck = new SpellChecker();

	// use configuration settings to create spellcheckers for languages
	if (!spellcheck->buildCheckers())
	{
		delete spellcheck;
		return 1;
	}
	else
	{
		MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/spellchecker.ui"), spellcheck);
		return 0;
	}
}

extern "C" KADU_EXPORT void spellchecker_close()
{
	if (spellcheck)
	{
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/spellchecker.ui"), spellcheck);
		delete spellcheck;
	}
}

SpellChecker::SpellChecker()
{
	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));

	// prepare configuration of spellchecker
	spellConfig = new_aspell_config();
	aspell_config_replace(spellConfig, "encoding", "utf-8");

#ifdef Q_OS_WIN32
	aspell_config_replace(spellConfig, "dict-dir", qPrintable(dataPath("aspell/dict")));
	aspell_config_replace(spellConfig, "data-dir", qPrintable(dataPath("aspell/data")));
	aspell_config_replace(spellConfig, "prefix", qPrintable(ggPath("dicts")));
#endif

	import_0_5_0_Configuration();

	createDefaultConfiguration();
	// load mark settings
	buildMarkTag();
}

SpellChecker::~SpellChecker()
{
	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));

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

	aspell_config_replace(spellConfig, "lang", name.ascii());

	// create spell checker using prepared configuration
	AspellCanHaveError* possibleErr = new_aspell_speller(spellConfig);
	if (aspell_error_number(possibleErr) != 0)
	{
		MessageBox::msg(aspell_error_message(possibleErr));
		return false;
	}
	else
		checkers[name] = to_aspell_speller(possibleErr);

	if (checkers.size() == 1)
	{
		foreach(ChatWidget *chat, chat_manager->chats())
			chatCreated(chat);
	}

	return true;
}

void SpellChecker::removeCheckedLang(QString& name)
{
	Checkers::Iterator checker = checkers.find(name);
	if (checker != checkers.end())
	{
		delete_aspell_speller(checker.data());
		checkers.erase(name);
	}
}

bool SpellChecker::buildCheckers()
{
	foreach(AspellSpeller *speller, checkers.values())
		delete_aspell_speller(speller);

	checkers.clear();

	// load languages to check from configuration
	QString checkedStr = config_file.readEntry("ASpell", "Checked", "pl");
	QStringList checkedList = QStringList::split(',', checkedStr);

	if (config_file.readBoolEntry("ASpell", "Accents", false))
		aspell_config_replace(spellConfig, "ignore-accents", "true");
	else
		aspell_config_replace(spellConfig, "ignore-accents", "false");

	if (config_file.readBoolEntry( "ASpell", "Case", false))
		aspell_config_replace(spellConfig, "ignore-case", "true");
	else
		aspell_config_replace(spellConfig, "ignore-case", "false");

	// create aspell checkers for each language
	for (unsigned int i = 0; i < checkedList.count(); i++)
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
		format.setUnderlineColor(QBrush(colorMark));
		format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	}
	format.setForeground(QBrush(colorMark));

	Highlighter::setHighlightFormat(format);
	Highlighter::rehighlightAll();
}

void SpellChecker::chatCreated(ChatWidget *chat)
{
	if (checkers.size() > 0)
	{
		Highlighter *highlighter = new Highlighter(chat->edit()->document());
	}
}

void SpellChecker::configForward()
{
	if (availList->selectedItems().count() > 0)
		configForward2(checkList->selectedItems()[0]);
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
		QListWidgetItem *item = availList->takeItem(availList->row(it));
		delete item;
	}
}

void SpellChecker::configBackward2(QListWidgetItem *it)
{
	QString langName = it->text();
	availList->addItem(langName);
	QListWidgetItem *item = checkList->takeItem(checkList->row(it));
	delete item;
	removeCheckedLang(langName);
}

void SpellChecker::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	ConfigGroupBox *optionsGroupBox = mainConfigurationWindow->configGroupBox("Chat", "SpellChecker", tr("ASpell options"));

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

void SpellChecker::import_0_5_0_Configuration()
{
	ConfigFile *oldConfig = new ConfigFile(ggPath("spellchecker.conf"));
	config_file.addVariable("ASpell", "Checked", oldConfig->readEntry("ASpell", "Checked"));
	config_file.addVariable("ASpell", "Color", oldConfig->readEntry("ASpell", "Color"));
	config_file.addVariable("ASpell", "Bold", oldConfig->readEntry("ASpell", "Bold"));
	config_file.addVariable("ASpell", "Italic", oldConfig->readEntry("ASpell", "Italic"));
	config_file.addVariable("ASpell", "Underline", oldConfig->readEntry("ASpell", "Underline"));
	config_file.addVariable("ASpell", "Accents", oldConfig->readEntry("ASpell", "Accents"));
	config_file.addVariable("ASpell", "Case", oldConfig->readEntry("ASpell", "Case"));
	delete oldConfig;
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
			if (aspell_speller_check(it.data(), word.toUtf8(), -1))
			{
				isWordValid = true;
				break;
			}
		}
	}
	
	return isWordValid;
}
