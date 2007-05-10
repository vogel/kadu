/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qtooltip.h>

#include <stdlib.h>

#include "chat_manager.h"
#include "chat_manager_slots.h"
#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "message_box.h"
#include "misc.h"

ChatManagerSlots::ChatManagerSlots(QObject* parent, const char* name)
	: QObject(parent, name)
{
}

void ChatManagerSlots::setMailPath(int mailNumber)
{
	kdebugf();

	QLineEdit *mailPathEdit = ConfigDialog::getLineEdit("Chat", "Custom Mail client");

	switch (mailNumber)
	{
		case 0:
		{
			mailPathEdit->setBackgroundMode(PaletteBase);
			mailPathEdit->setReadOnly(false);
			return;
		}
		case 1:
		{
			mailPathEdit->setText("kmail %1");
			mailPathEdit->setBackgroundMode(PaletteButton);
			mailPathEdit->setReadOnly(true);
			break;
		}
		case 2:
		{
			mailPathEdit->setText("thunderbird -compose mailto:%1");
			mailPathEdit->setBackgroundMode(PaletteButton);
			mailPathEdit->setReadOnly(true);
			break;
		}
		case 3:
		{
			mailPathEdit->setText("seamonkey -compose mailto:%1");
			mailPathEdit->setBackgroundMode(PaletteButton);
			mailPathEdit->setReadOnly(true);
			break;
		}
		default:
			return;
	}

	findMailClient(mailNumber);
		
	kdebugf2();
}

void ChatManagerSlots::findMailClient(const unsigned int mailNumber)
{
	kdebugf();
	QStringList searchPath = QStringList::split(":", QString(getenv("PATH")));
	QString mailClient;
	switch (mailNumber)
	{
		case 1:
		{
			searchPath.append("/opt/kde/bin");
			searchPath.append("/opt/kde3/bin");
			mailClient = "kmail";
			break;
		}
		case 2:
		{
			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append("/usr/local/Thunderbird");
			searchPath.append("/usr/local/thunderbird");
			searchPath.append("/opt/thunderbird");
			mailClient = "thunderbird";
			break;
		}
		case 3:
		{
			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append("/usr/local/Seamonkey");
			searchPath.append("/usr/local/seamonkey");
			searchPath.append("/opt/seamonkey");
			mailClient = "seamonkey";
			break;
		}
		default:
			return;
	}

	QString mailClientPath;
	CONST_FOREACH(path, searchPath)
	{
		if (QFile::exists((*path) + '/' + mailClient))
		{
			mailClientPath = (*path);
			break;
		}
	}

	if (mailClientPath.isEmpty())
	{
		MessageBox::msg(tr("Selected mail client was not found in your system. The path to it doesn't exist in $PATH variable.\nYou may add it to $PATH or specify location using Specify path option"));

		ConfigDialog::getComboBox("Chat", "Choose your mail client")->setCurrentItem(0);
		setMailPath(0);

		return;
	}

	QLineEdit *mailPathEdit = ConfigDialog::getLineEdit("Chat", "Custom Mail client");
	mailPathEdit->setText(mailClientPath + '/' + mailPathEdit->text());

	kdebugf2();
}

void ChatManagerSlots::setMailClients(QComboBox *mailCombo)
{
	mailCombo->insertItem(tr("Specify path"));
	mailCombo->insertItem("KMail");
	mailCombo->insertItem("Thunderbird");
	mailCombo->insertItem("SeaMonkey");
}

void ChatManagerSlots::initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
{
	/*
		UWAGA: w tej funkcji NIE WOLNO korzystaæ z klasy ConfigDialog
		(joi)
	*/
	kdebugf();
	browserCombo->insertItem(tr("Specify path"));
	browserCombo->insertItem("Konqueror");
	browserCombo->insertItem("Opera");
	browserCombo->insertItem("Mozilla");
	browserCombo->insertItem("Mozilla Firefox");
	browserCombo->insertItem("Dillo");
	browserCombo->insertItem("Galeon");
	browserCombo->insertItem("Safari");
	QString browserCommandLine=browserPath->text();
	browserOptionsCombo->setEnabled(false);

	int browserNumber=config_file.readNumEntry("Chat", "WebBrowserNo", 0);
	browserCombo->setCurrentItem(browserNumber);
	browserOptionsCombo->clear();
	switch (browserNumber)
	{
		case 1: 	//konqueror
		{
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			if (browserCommandLine.find("newTab", 0, true) != -1)
				browserOptionsCombo->setCurrentItem(1);
			else
				browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 2: 	//opera
		{
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->insertItem(tr("Open in background tab"));
			if (browserCommandLine.find("-newpage", 0, true) != -1)	//jak znajdzie ta opcje to podswietla odpowiedni wpis w combo
				browserOptionsCombo->setCurrentItem(1);
			else if (browserCommandLine.find("-backgroundpage", 0, true) != -1)
				browserOptionsCombo->setCurrentItem(2);
			else
				browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);	//uaktywniamy combo
			break;
		}
		case 3:		//mozilla
		case 4: 	//firefox
		{
			browserOptionsCombo->insertItem(tr("Open in new window"));		//dodajemy pozycje combo
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			if (browserCommandLine.find("new-tab", 0, true) != -1)	//i wyszukujemy ktora opcje zaznaczyc
				browserOptionsCombo->setCurrentItem(1);
			else
				browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
	}

	browserPath->setText(config_file.readEntry("Chat", "WebBrowser"));//potrzebne dla modu³u start_wizard
	if (browserCombo->currentItem() != 0)
	{
		browserPath->setReadOnly(true);
		browserPath->setBackgroundMode(PaletteButton);
	}
	else
	{
		browserPath->setReadOnly(false);
		browserPath->setBackgroundMode(PaletteBase);
	}
	kdebugf2();
}

void ChatManagerSlots::onCreateTabChat()
{
	kdebugf();
	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	cb_emoticons_theme->insertStringList(emoticons->themes());
	cb_emoticons_theme->setCurrentText(config_file.readEntry("Chat", "EmoticonsTheme"));

	if ((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") == EMOTS_NONE)
		(cb_emoticons_theme)->setEnabled(false);

	//ustawienie pól w combo wyboru przegladarki
	QComboBox *browserCombo= ConfigDialog::getComboBox("Chat", "Choose your browser");
	QComboBox *browserOptionsCombo=ConfigDialog::getComboBox("Chat", "Browser options");
	QLineEdit *browserPath= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	initBrowserOptions(browserCombo, browserOptionsCombo, browserPath);

	//podpiecie pod zmiane w combo
	connect(browserCombo, SIGNAL(activated (int)), this, SLOT(findAndSetWebBrowser(int)));
	connect(browserOptionsCombo, SIGNAL(activated (int)), this, SLOT(findAndSetBrowserOption(int)));

	//ustawienie pól w combo wyboru klienta poczty i podpiêcie pod zmianê
	QComboBox *mailCombo = ConfigDialog::getComboBox("Chat", "Choose your mail client");
	setMailClients(mailCombo);
	connect(mailCombo, SIGNAL(activated(int)), this, SLOT(setMailPath(int)));
	//ustawienie wybranego klienta w combo i wpisanie jego ¶cie¿ki do pola edycji
	unsigned int mailClientNo = config_file.readUnsignedNumEntry("Chat", "MailClientNo", 0);
	mailCombo->setCurrentItem(mailClientNo);
	setMailPath(mailClientNo);

	onPruneChat(config_file.readBoolEntry("Chat", "ChatPrune"));

	QCheckBox *c_foldlink = ConfigDialog::getCheckBox("Chat", "Automatically fold links");
	QHGroupBox *h_fold = ConfigDialog::getHGroupBox("Chat", "Link folding");
	onFoldLink(config_file.readBoolEntry("Chat", "FoldLink"));

	QToolTip::add(h_fold, tr("URLs longer than this value will be shown truncated to this length"));
	QToolTip::add(c_foldlink, tr("This will show a long URL as http://www.start...end.com/\nto protect the chat window from a mess"));
	ConfigDialog::getSpinBox("Chat", "Max image size")->setSuffix(" kB");

	QSpinBox *blockCloseTime=ConfigDialog::getSpinBox("Chat", "Max time to block window close");
	blockCloseTime->setEnabled(config_file.readBoolEntry("Chat", "ChatCloseTimer"));
	blockCloseTime->setSuffix(" s");
	QCheckBox *shortcutSends = ConfigDialog::getCheckBox("Chat", "\"%1\" in chat sends message by default");
	shortcutSends->setText(shortcutSends->text().arg(config_file.readEntry("ShortCuts", "chat_newline")));

	kdebugf2();
}

void ChatManagerSlots::onCreateTabLook()
{
	kdebugf();
	//deaktywacja opcji wylaczenia separatorow
	QCheckBox *b_noHeadersRepeat= ConfigDialog::getCheckBox("Look", "Remove chat header repetitions");

	QSpinBox *s_headersSeparatorHeight= ConfigDialog::getSpinBox("Look", "Chat header separators height:");
	QSpinBox *s_noHeadersInterval= ConfigDialog::getSpinBox("Look", "Interval between header removal:");

	QSpinBox *s_SeparatorHeight= ConfigDialog::getSpinBox("Look", "Message separators height:");

	s_headersSeparatorHeight->setEnabled(b_noHeadersRepeat->isChecked());
	s_noHeadersInterval->setEnabled(b_noHeadersRepeat->isChecked());

	//dodanie suffiksu w spinboksach
	s_headersSeparatorHeight->setSuffix(" px");
	s_noHeadersInterval->setSuffix(" min");
	s_SeparatorHeight->setSuffix(" px");

	//opcja usuwania czasu z serwera
	QSpinBox *s_noServerTimeDiff= ConfigDialog::getSpinBox("Look", "Maximum time difference");
	
	s_noServerTimeDiff->setEnabled(ConfigDialog::getCheckBox("Look", "Remove server time")->isChecked());
	s_noServerTimeDiff->setSuffix(" s");

	onChatThemeChanged(ConfigDialog::getComboBox("Look", "Select chat style")->currentText());
	updatePreview();

	kdebugf2();
}

void ChatManagerSlots::onChatThemeChanged(const QString &name)
{
	kdebugf();
	ConfigDialog::getTextEdit("Look", "Full chat style:")->setEnabled(name == qApp->translate("Chat", "Custom"));
}

void ChatManagerSlots::onBlockClose(bool toggled)
{
	ConfigDialog::getSpinBox("Chat", "Max time to block window close")->setEnabled(toggled);
}

void ChatManagerSlots::onPruneChat(bool toggled)
{
	ConfigDialog::getHGroupBox("Chat", "Message pruning")->setEnabled(toggled);
}

void ChatManagerSlots::onFoldLink(bool toggled)
{
	ConfigDialog::getHGroupBox("Chat", "Link folding")->setEnabled(toggled);
}

void ChatManagerSlots::onRemoveHeaders(bool toggled)
{
	ConfigDialog::getSpinBox("Look", "Chat header separators height:")->setEnabled(toggled);
	ConfigDialog::getSpinBox("Look", "Interval between header removal:")->setEnabled(toggled);
}

void ChatManagerSlots::onRemoveServerTime(bool toggled)
{
	ConfigDialog::getSpinBox("Look", "Maximum time difference")->setEnabled(toggled);
}

void ChatManagerSlots::onApplyTabLook()
{
	kdebugf();
/*
	Aby unikn±c problemów z niepoprawnymi localesami i pozniejszymi
	k³opotami które moga wynikn±c z tego, musimy zamienic dwie mozliwe
	mozliwo¶ci na _puste_pole_ przez co uzyskamy ze kadu i tak bedzie
	dynamicznie reagowac na zmiany localesów nie zaleznie jaka wersja
	by³a zapisana przed ustawieniem ustawien domyslnych(moze nie za
	dobrze to wyjasnione, ale konieczne. Nie dotyczy to dwóch zmiennych
	config.panelsyntax i config.conferencesyntax, bo pierwotnie zawieraj
	TYLKO sam± sk³adnie)
*/

	QLineEdit *e_chatsyntax= ConfigDialog::getLineEdit("Look", "Chat window title syntax:");
	QLineEdit *e_conferenceprefix= ConfigDialog::getLineEdit("Look", "Conference window title prefix:");

	if (e_chatsyntax->text() == tr("Chat with ")+"%a (%s[: %d])" || e_chatsyntax->text() == "Chat with %a (%s[: %d])")
		config_file.writeEntry("Look", "ChatContents", QString::null);

	if (e_conferenceprefix->text() == tr("Conference with ") || e_conferenceprefix->text() == "Conference with ")
		config_file.writeEntry("Look", "ConferencePrefix", QString::null);

	kdebugf2();
}

void ChatManagerSlots::onApplyTabChat()
{
	kdebugf();

	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	config_file.writeEntry("Chat", "EmoticonsTheme",cb_emoticons_theme->currentText());
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	config_file.writeEntry("Chat", "WebBrowserNo", ConfigDialog::getComboBox("Chat", "Choose your browser")->currentItem());
	config_file.writeEntry("Chat", "MailClientNo", ConfigDialog::getComboBox("Chat", "Choose your mail client")->currentItem());

	chat_manager->changeAppearance();

	if (config_file.readBoolEntry("Chat", "RememberPosition"))
	{
		userlist->addPerContactNonProtocolConfigEntry("chat_geometry", "ChatGeometry");
		userlist->addPerContactNonProtocolConfigEntry("chat_vertical_sizes", "VerticalSizes");
	}
	else
	{
		userlist->removePerContactNonProtocolConfigEntry("chat_geometry");
		userlist->removePerContactNonProtocolConfigEntry("chat_vertical_sizes");
	}

	bool msgTitle = config_file.readBoolEntry("Chat","NewMessagesInChatTitle");
	bool blnTitle = config_file.readBoolEntry("Chat","BlinkChatTitle");
	CONST_FOREACH(chat, chat_manager->chats()) // set options for all chats...
	{
		(*chat)->setShowNewMessagesNum(msgTitle);
		(*chat)->setBlinkChatTitle(blnTitle);

		unsigned int newMsgs = (*chat)->getNewMessagesNum();

		// for chats with waiting messages we also trigger apropriate slots...
		if (blnTitle && (newMsgs > 0)) 
			(*chat)->changeTitle();
		else if (msgTitle && (newMsgs > 0))
			(*chat)->showNewMessagesNumInTitle();
	}

	KaduActions["autoSendAction"]->setAllOn(config_file.readBoolEntry("Chat", "AutoSend"));

	kdebugf2();
}

void ChatManagerSlots::chooseColor(const char* name, const QColor& color)
{
	kdebugf();
	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
	if (QString(name)=="own_bg_color")
		preview1->setPaletteBackgroundColor(color);
	else if (QString(name)=="his_bg_color")
		preview2->setPaletteBackgroundColor(color);
	else if (QString(name)=="own_font_color")
		preview1->setPaletteForegroundColor(color);
	else if (QString(name)=="his_font_color")
		preview2->setPaletteForegroundColor(color);
	else
		kdebugm(KDEBUG_ERROR, "chooseColor: label '%s' not known!\n", name);
	kdebugf2();
}

void ChatManagerSlots::chooseFont(const char* name, const QFont& font)
{
	kdebugf();
	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
	if (QString(name)=="chat_font_box")
	{
		preview1->setFont(font);
		preview2->setFont(font);
	}
	kdebugf2();
}

void ChatManagerSlots::chooseEmoticonsStyle(int index)
{
	ConfigDialog::getComboBox("Chat","Emoticons theme")->setEnabled(index!=0);
}

void ChatManagerSlots::updatePreview()
{
	kdebugf();
	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
	preview1->setFont(config_file.readFontEntry("Look", "ChatFont"));
	preview1->setPaletteForegroundColor(config_file.readColorEntry("Look", "ChatMyFontColor"));
	preview1->setPaletteBackgroundColor(config_file.readColorEntry("Look", "ChatMyBgColor"));
	preview1->setAlignment(Qt::AlignLeft);
	preview2->setFont(config_file.readFontEntry("Look", "ChatFont"));
	preview2->setPaletteForegroundColor(config_file.readColorEntry("Look", "ChatUsrFontColor"));
	preview2->setPaletteBackgroundColor(config_file.readColorEntry("Look", "ChatUsrBgColor"));
	preview2->setAlignment(Qt::AlignLeft);
	kdebugf2();
}

void ChatManagerSlots::findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
{
	kdebugf();
	/*
		UWAGA1: obs³uga mozilli i firefoksa jest dosyæ skomplikowana, wiêc przy rozbudowie tej funkcji
		nale¿y najpierw zrozumieæ jej dzia³anie dla tych dwóch przegl±darek, bo mo¿na siê naci±æ...

		UWAGA2: w tej funkcji NIE WOLNO korzystaæ z klasy ConfigDialog
	*/
	browserOptionsCombo->setEnabled(false);	//blokujemy combo
	browserOptionsCombo->clear(); //czyscimy combo z opcjami

	if (selectedBrowser==0)
	{
		browserPath->setReadOnly(false);
		browserPath->setBackgroundMode(PaletteBase);
		return;
	}
	else
	{
		browserPath->setReadOnly(true);
		browserPath->setBackgroundMode(PaletteButton);
	}

	QString homePath=getenv("HOME");
	QString browserName;

	QStringList searchPath=QStringList::split(":", QString(getenv("PATH")));

	switch (selectedBrowser)
	{
		case 1: //konqueror
		{
			browserName="dcop";
			searchPath.append("/opt/kde/bin");
			searchPath.append("/opt/kde3/bin");
			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 2://opera
		{
			browserName="opera";
			searchPath.append("/opt/opera");
			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->insertItem(tr("Open in background tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 3: //mozilla
		{
			browserName="mozilla-xremote-client";

			QStringList dirList=QDir("/usr/lib").entryList("mozilla*", QDir::All, QDir::Name|QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/"+(*dir));

			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append(homePath+"/Mozilla");
			searchPath.append(homePath+"/mozilla");
			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 4:	//firefox
		{
			browserName = "mozilla-xremote-client";

			QStringList dirList = QDir("/usr/lib").entryList("mozilla-firefox*", QDir::All, QDir::Name|QDir::Reversed);
			CONST_FOREACH(dir, dirList)
			{
				searchPath.append("/usr/lib/" + (*dir));
				if (QFile::exists("/usr/lib/" + (*dir) + "/mozilla-firefox-xremote-client"))
					//jeste¶my na debianie, gdzie zmienili nazwê skryptu, grrr :|
					browserName = "mozilla-firefox-xremote-client";
			}

			dirList = QDir("/usr/lib").entryList("firefox*", QDir::All, QDir::Name|QDir::Reversed);
			CONST_FOREACH(dir, dirList)
			{
				searchPath.append("/usr/lib/" + (*dir));
				if (QFile::exists("/usr/lib/" + (*dir) + "/firefox-xremote-client"))
					//jeste¶my na ubuntu, gdzie tak¿e zmienili nazwê skryptu!
					browserName = "firefox-xremote-client";
			}

			searchPath.append("/usr/lib/MozillaFirefox");
			searchPath.append("/usr/local/Firefox");
			searchPath.append("/usr/local/firefox");
			searchPath.append("/opt/firefox");
			searchPath.append(homePath+"/Firefox");
			searchPath.append(homePath+"/firefox");

			dirList=QDir("/usr/lib").entryList("mozilla*", QDir::All, QDir::Name|QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/"+(*dir));

			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 5: browserName="dillo"; break;
		case 6: browserName="galeon"; break;
		case 7: 	//safari - mac
		{
			browserName="Safari.app";
			searchPath.append("/Applications");
			break;
		}
		default:
		{
			kdebugf2();
			return;
		}
	}
	QFile browserFile;
	QString path, testPath;

	bool browserFound=false;
	QStringList::iterator dir=searchPath.begin();
	QStringList::iterator endDir=searchPath.end();

	kdebugm(KDEBUG_INFO, "search path: %s\n", searchPath.join(" ").local8Bit().data());
	while (!browserFound && dir!=endDir)
	{
		testPath = (*dir) + '/' + browserName;
		if (QFile::exists(testPath))
		{
			if (selectedBrowser==1) //konqueror
			{
				if (browserName=="kfmclient")
					path.replace("kfmclient", testPath);
				else
				{
					path="ok=0;for i in `dcop|grep konqueror`; do shown=`dcop $i konqueror-mainwindow#1 shown`; if [ \"$shown\" == \"true\" ];then dcop $i KonquerorIface openBrowserWindow \"%1\" && ok=1; fi; if [ \"$ok\" == \"1\" ]; then break; fi done; if [ \"$ok\" != \"1\" ]; then kfmclient openURL \"%1\"; fi;";
					path.replace("dcop", testPath);
					browserName="kfmclient";
					dir=searchPath.begin();
					continue;
				}
			}
			else if (selectedBrowser==3) //mozilla
			{
				if (browserName=="mozilla")
					path=path+testPath+" \"%1\"";
				else
				{
					path=testPath+" -a mozilla \"openURL(%1,new-window)\" || ";
					browserName="mozilla";
					dir=searchPath.begin();
					continue;
				}
			}
			else if (selectedBrowser==4) //firefox
			{
				if (browserName=="firefox")
					path=path+testPath+" \"%1\"";
				else
				{
					path=testPath+" \"openURL(%1,new-window)\" || ";
					browserName="firefox";
					dir=searchPath.begin();
					continue;
				}
			}
			else if (selectedBrowser == 7) //safari
				path = "open " + testPath;
			else
				path=testPath;
			browserPath->setText(path);
			browserFound=true;
			kdebugm(KDEBUG_INFO, "browser found! '%s'\n", path.local8Bit().data());
		}
		dir++;
	}
	if (!browserFound)
	{
		MessageBox::msg(tr("I didn't find the browser you selected! The path to it doesn't exists in $PATH variable. \nYou may add it to $PATH or specify location using Specify path option."));
		browserCombo->setCurrentItem(0);	//ustawiamy na default
		browserOptionsCombo->clear();	//czyscimy opcje
		browserOptionsCombo->setEnabled(false); //wylaczamy combo z opcjami
		browserPath->setReadOnly(false); //wlaczamy mozliwosc edycji
		browserPath->setBackgroundMode(PaletteBase); //ustawiamy kolor
//		browserPath->clear(); 	//no i czyscimy LineEdita
	}
	kdebugf2();
}

void ChatManagerSlots::findAndSetWebBrowser(int selectedBrowser)
{
	kdebugf();

	QComboBox *browserCombo=ConfigDialog::getComboBox("Chat", "Choose your browser");
	QComboBox *browserOptionsCombo=ConfigDialog::getComboBox("Chat", "Browser options");
	QLineEdit *browserPath= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	findBrowser(selectedBrowser, browserCombo, browserOptionsCombo, browserPath);

	kdebugf2();
}

void ChatManagerSlots::setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser)
{
	kdebugf();
	/*
		UWAGA: w tej funkcji NIE WOLNO korzystaæ z klasy ConfigDialog
	*/
	QString browserPath=browserPathEdit->text();
	switch(chosenBrowser)
	{
		case 1: //Konqueror
		{
			if (selectedOption==1)
				browserPath.replace("KonquerorIface openBrowserWindow", "konqueror-mainwindow#1 newTab");
			else
				browserPath.replace("konqueror-mainwindow#1 newTab", "KonquerorIface openBrowserWindow");
			browserPathEdit->setText(browserPath);
			break;
		}
		case 2:		//Opera
		{
			browserPath.remove(" -newwindow");
			browserPath.remove(" -newpage");
			browserPath.remove(" -backgroundpage");
			switch(selectedOption)
			{
				case 0: browserPath.append(" -newwindow"); break;
				case 1: browserPath.append(" -newpage"); break;
				case 2: browserPath.append(" -backgroundpage"); break;
			}
			browserPathEdit->setText(browserPath);
			break;
		}
		case 3: 	//Mozilla
		case 4:		//Firefox
		{
			if (selectedOption==1)
				browserPath.replace("new-window", "new-tab");
			else
				browserPath.replace("new-tab", "new-window");
			browserPathEdit->setText(browserPath);
			break;
		}
	}
	kdebugf2();
}

void ChatManagerSlots::findAndSetBrowserOption(int selectedOption)
{
	kdebugf();
	setBrowserOption(selectedOption,
		ConfigDialog::getLineEdit("Chat", "Custom Web browser"),
		ConfigDialog::getComboBox("Chat", "Choose your browser")->currentItem());
	kdebugf2();
}

void ChatManagerSlots::chooseBackgroundFile()
{
	QLineEdit *bg_path = ConfigDialog::getLineEdit("Look", "Chat background image");
	QWidget w;
	ImageDialog *bg_dlg = new ImageDialog(&w);
	bg_dlg->setDir(bg_path->text());
	bg_dlg->setCaption(tr("Insert image"));
	if (bg_dlg->exec() == QDialog::Accepted)
		bg_path->setText(bg_dlg->selectedFile());
	delete bg_dlg;
}

ChatManagerSlots* ChatManager::chatslots=NULL;
