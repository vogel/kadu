/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *	Wersja 0.3b 30.08, 00:05
 *	Autor : Tomasz "Dorr(egaray)" Rostañski
 *	zmieniona nazwa z start_wizard na config_wizard
 *
 *	UWAGA!! 	Wymaga Kadu 0.4.0 cvs z 29.08.2004 lub nowszego
 *	(wykorzystanie funkcji wydzielonych przez joia w kadu cvs)
 */

#include "wizard.h"
#include "debug.h"
#include "config_file.h"
#include "misc.h"
#include "gadu.h"
#include "kadu.h"
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <stdlib.h>
#include <unistd.h>
#include "addons.h"	//tu sa ustawienia kolorow, dymkow i panelu informacyjnego

extern "C" int config_wizard_init()
{
	kdebugf();
	startWizardObj=new Wizard();

	if (config_file.readEntry("General", "UIN", "0")=="0")
		startWizardObj->exec();
	
	kdebugf2();
	return 0;
}

extern "C" void config_wizard_close()
{
	kdebugf();
	delete startWizardObj;	

	kdebugf2();
}

/**
	Konstruktor - tworzy wszystkie okna 
**/
Wizard::Wizard( QWidget *parent, const char *name)
{	setCaption(tr("Kadu Wizard"));
	setFixedSize(470, 300);	/* ustawia rozmiar okna */

	showWelcomePage();		/* wyswietla powitanie */
	showLanguagePage();		/* ustawienie jezyka */
  	showGGNumberSelect();	/* wyswietla okno ustawien numerka */
	showGGCurrentNumberPage();
	showGGNewNumberPage();
	showGeneralOptionsPage(); /* inne */
	showChatOpionsPage();	/* ustawienia gadki */
	showColorsPage();			/* kolory */
	showHintsOptionsPage();	/* dymki */
	showInfoPanelPage();		/* wybor panelu */
	showQtStylePage();			/* Qt3 theme */
	showSoundOptionsPage();	/* dzwieki */
	showWWWOpionsPage();	/* ustawienie przegladarki */
	showGreetingsPage();		/* okno koncowe */

	QPushButton *finish_button = finishButton();
	connect(finish_button, SIGNAL(clicked()), this, SLOT(finishPressed()));
	QPushButton *next_button = nextButton();
	connect(next_button, SIGNAL(clicked()), this, SLOT(nextPressed()));
	QPushButton *back_button = backButton();
	connect(back_button, SIGNAL(clicked()), this, SLOT(backPressed()));
	helpButton()->hide();
	
	/*  to po to by byly tlumaczenia buttonow w wizardzie */
	finish_button->setText(tr("Finish"));
	next_button->setText(tr("Next >"));
	back_button->setText(tr("< Back"));
	cancelButton()->setText(tr("Cancel"));
	
	noNewAccount=false;	/* normalnie to jest false */
	menuPos=kadu->mainMenu()->insertItem(icons_manager.loadIcon("Configuration"), tr("Configuration Wizard"), this, SLOT(wizardStart()), 0, -1, 0);
	
	connect(gadu, SIGNAL(userListImported(bool, UserList&)), this, SLOT(userListImported(bool, UserList&)));
	connect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredAccount(bool, UinType)));
	//connect(gadu, SIGNAL(connected(), this, SLOT(connected())));
}

/**
	destruktor
**/
Wizard::~Wizard()
{	kadu->mainMenu()->removeItem(menuPos);
	disconnect(gadu, SIGNAL(userListImported(bool, UserList&)), this, SLOT(userListImported(bool, UserList&)));
	disconnect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredAccount(bool, UinType)));
	//disconnect(gadu, SIGNAL(connected(), this, SLOT(connected())));

	/* czyszczenie pamieci */
	delete(rb_haveNumber); delete(rb_dontHaveNumber); delete(l_ggNumber); delete(l_ggPassword); delete(l_ggNewPasssword); delete(l_ggNewPassswordRetyped); delete(l_email); delete(l_customBrowser);
	delete(c_importContacts); delete(c_waitForDelivery); delete(c_enterSendsMessage); delete(c_openOnNewMessage); delete(c_flashTitleOnNewMessage); delete(c_ignoreAnonyms);
	delete(c_logMessages); delete(c_logStatusChanges); delete(c_privateStatus); delete(c_showBlocked); delete(c_showBlocking); delete(c_startDocked); delete(c_enableSounds); delete(c_playWhilstChatting);
	delete(c_playWhenInvisible); delete(c_showInfoPanel); delete(cb_browser); delete(cb_browserOptions); delete(cb_hintsTheme); delete(cb_hintsType); delete(cb_colorTheme); delete(cb_iconTheme); 
	delete(cb_qtTheme); delete(cb_panelTheme); delete(preview); delete(preview2); delete(preview4); delete(iconPreview); delete(iconPreview2); delete(iconPreview3); delete(iconPreview4); delete(infoPreview);
	delete(welcomePage); delete(ggNumberSelect); delete(ggCurrentNumberPage); delete(ggNewNumberPage); delete(languagePage); delete(chatOptionsPage); delete(wwwOptionsPage); 
	delete(soundOptionsPage); delete(generalOptionsPage); delete(greetingsPage); delete(hintsOptionsPage); delete(colorsPage); delete(qtStylePage); delete(infoPanelPage); delete(c_showScrolls);
}

/**
	Wyswietla powitanie
**/
void Wizard::showWelcomePage()
{	welcomePage=new QVBox(this);
   	welcomePage->setSpacing(8);

	new QLabel(tr("<h2>Welcome in Kadu</h2><h3> the Gadu-gadu network client for *nix and MacOS X.</h3><br><br><font size=+1>This is first time you launch Kadu. This wizard will help you to configure the basic settings of Kadu. If you are experienced kadu user you may omit the wizard clicking Cancel. Otherwise click Next.</font>"), welcomePage);

	addPage(welcomePage, tr("Welcome"));
    setNextEnabled(welcomePage,true);
	setHelpEnabled(welcomePage, false);
}


/**
	Wybor opcji z numerkiem gg
**/
void Wizard::showGGNumberSelect()
{	ggNumberSelect=new QVBox(this);

	new QLabel(tr("<h3>Please decide if you want to use your old Gadu-gadu account and number or to create the new one</h3>"), ggNumberSelect);

	QButtonGroup *grp_numberSelection=new QButtonGroup(tr("Select account option"), ggNumberSelect);
	grp_numberSelection->setInsideMargin(10);
	grp_numberSelection->setColumns(1);
	grp_numberSelection->setInsideSpacing(6);

	rb_haveNumber=new QRadioButton(tr("I have a number"), grp_numberSelection);
	rb_haveNumber->setChecked(true);
	rb_dontHaveNumber=new QRadioButton(tr("I don't have one"), grp_numberSelection);

	addPage(ggNumberSelect, tr("Gadu-gadu account"));
}


/**
	Wyswietlenie ustawienia istniejacego konta
**/
void Wizard::showGGCurrentNumberPage()
{	ggCurrentNumberPage=new QVBox(this);
	
	new QLabel(tr("<h3>You decided to use your existing account.Please configure it</h3>"), ggCurrentNumberPage);
	QGroupBox *grp_haveNumber=new QGroupBox(tr("Please enter your account settings"), ggCurrentNumberPage);
	grp_haveNumber->setInsideMargin(10);
	grp_haveNumber->setColumns(2);
	grp_haveNumber->setInsideSpacing(4);

	new QLabel(tr("Gadu-gadu number"), grp_haveNumber);
	l_ggNumber=new QLineEdit(grp_haveNumber);
	l_ggNumber->setText(config_file.readEntry("General", "UIN", "0"));
	new QLabel(tr("Gadu-gadu password"),grp_haveNumber);
	l_ggPassword=new QLineEdit(grp_haveNumber);
	l_ggPassword->setEchoMode(QLineEdit::Password);
	l_ggPassword->setText(pwHash(config_file.readEntry("General", "Password", "")));
	c_importContacts = new QCheckBox(tr("Import contacts"), grp_haveNumber);
	c_importContacts->setChecked(false);
	//c_importContacts->setEnabled(false);	//na razie zablokowane

	addPage(ggCurrentNumberPage, tr("Gadu-gadu account"));
}


/**
	Zakladanie nowego konta
**/
void Wizard::showGGNewNumberPage()
{	ggNewNumberPage=new QVBox(this);
	
	new QLabel(tr("<h3>Please enter your valid e-mail address and password you want to secure your new Gadu-gadu number<h3>"), ggNewNumberPage);

	QGroupBox *grp_dontHaveNumber=new QGroupBox("", ggNewNumberPage);
	grp_dontHaveNumber->setInsideMargin(10);
	grp_dontHaveNumber->setColumns(2);
	grp_dontHaveNumber->setInsideSpacing(4);
	
	new QLabel(tr("Password"), grp_dontHaveNumber);
	l_ggNewPasssword=new QLineEdit(grp_dontHaveNumber);
	l_ggNewPasssword->setEchoMode(QLineEdit::Password);
	new QLabel(tr("Re-type password"), grp_dontHaveNumber);
	l_ggNewPassswordRetyped=new QLineEdit(grp_dontHaveNumber);
	l_ggNewPassswordRetyped->setEchoMode(QLineEdit::Password);
	new QLabel(tr("Your e-mail address"), grp_dontHaveNumber);
	l_email=new QLineEdit(grp_dontHaveNumber);

	addPage(ggNewNumberPage, tr("Gadu-gadu account"));
}


/**
	Wybor jezyka
**/
void Wizard::showLanguagePage()
{	languagePage = new QVBox(this);

	new QLabel(tr("<h3>Please select language version of Kadu you want to use</h3>"), languagePage);

	QGroupBox *grp_language=new QGroupBox (tr("Language selection"), languagePage);
	grp_language->setInsideMargin(10);
	grp_language->setColumns(2);
	grp_language->setInsideSpacing(4);
	new QLabel (tr("Choose Kadu language"), grp_language);
	QComboBox *cb_language = new QComboBox (grp_language);
	cb_language->insertItem(tr("English"));
	cb_language->insertItem(tr("Polish"));
	cb_language->insertItem(tr("Italian"));
	cb_language->insertItem(tr("German"));

	QString lang=config_file.readEntry("General", "Language", "en");

	if (lang=="pl") cb_language->setCurrentItem(1);
		else if (lang=="it") cb_language->setCurrentItem(2);
		else if (lang=="de") cb_language->setCurrentItem(3);

	connect (cb_language, SIGNAL(activated(int)), this, SLOT(setLanguage(int)));

	addPage(languagePage, tr("Language"));
}


/**
	Opcje chata
**/
void Wizard::showChatOpionsPage()
{	chatOptionsPage=new QVBox(this);
	
	new QLabel(tr("<h3>Please setup your chat options</h3>"), chatOptionsPage);

	QGroupBox *grp_chatOptions = new QGroupBox(tr("Chat options"), chatOptionsPage);
	grp_chatOptions->setInsideMargin(10);
	grp_chatOptions->setColumns(1);
	grp_chatOptions->setInsideSpacing(4);

	c_waitForDelivery = new QCheckBox (tr("Message acknowledgements (wait for delivery)"), grp_chatOptions);
	c_enterSendsMessage = new QCheckBox (tr("\"Enter\" key in chat sends message by default"), grp_chatOptions);
	c_openOnNewMessage = new QCheckBox (tr("Open chat window on new message"), grp_chatOptions);
	c_flashTitleOnNewMessage = new QCheckBox (tr("Flash chat title on new message"), grp_chatOptions);
	c_ignoreAnonyms = new QCheckBox (tr("Ignore messages from anonymous users"), grp_chatOptions);
	
	c_waitForDelivery->setChecked(config_file.readBoolEntry("Chat", "MessageAcks", true));
	c_enterSendsMessage->setChecked(config_file.readBoolEntry("Chat", "AutoSend", true));
	c_openOnNewMessage->setChecked(config_file.readBoolEntry("Chat", "OpenChatOnMessage",true));
	c_flashTitleOnNewMessage->setChecked(config_file.readBoolEntry("Chat", "BlinkChatTitle", true));
	c_ignoreAnonyms->setChecked(config_file.readBoolEntry("Chat", "IgnoreAnonymousUsers", false));

	addPage(chatOptionsPage, tr("Chat"));
}


/**
	opcje przegladarki www
**/
void Wizard::showWWWOpionsPage()
{	wwwOptionsPage=new QVBox(this);
	
	new QLabel(tr("<h3>Please setup Kadu for working with your favourite WWW browser</h3>"), wwwOptionsPage);

	QGroupBox *grp_wwwOptions = new QGroupBox(tr("WWW options"), wwwOptionsPage);
	grp_wwwOptions->setInsideMargin(10);
	grp_wwwOptions->setColumns(2);
	grp_wwwOptions->setInsideSpacing(4);
	new QLabel (tr("Choose your browser"), grp_wwwOptions);
	cb_browser = new QComboBox (grp_wwwOptions);
	new QLabel (tr("Browser options"), grp_wwwOptions);
	cb_browserOptions = new QComboBox (grp_wwwOptions);
	new QLabel (tr("Custom Web browser"), grp_wwwOptions);
	l_customBrowser = new QLineEdit (grp_wwwOptions);

	ChatSlots::initBrowserOptions(cb_browser, cb_browserOptions, l_customBrowser);
	l_customBrowser->setEnabled(!cb_browser->currentItem());
	l_customBrowser->setText(config_file.readEntry("Chat", "WebBrowser"));

	connect(cb_browser, SIGNAL(activated (int)), this, SLOT(findAndSetWebBrowser(int)));
	connect(cb_browserOptions, SIGNAL(activated (int)), this, SLOT(findAndSetBrowserOption(int)));

	addPage(wwwOptionsPage, tr("WWW"));
}


/**
	opcje dzwieku
**/
void Wizard::showSoundOptionsPage()
{	soundOptionsPage = new QVBox(this);

	new QLabel(tr("<h3>Please setup sounds</h3>"), soundOptionsPage);

	QGroupBox *grp_soundOptions = new QGroupBox(tr("Sounds"), soundOptionsPage);
	grp_soundOptions->setInsideMargin(10);
	grp_soundOptions->setColumns(1);
	grp_soundOptions->setInsideSpacing(4);
	c_enableSounds = new QCheckBox (tr("Play sounds"), grp_soundOptions);
	c_playWhilstChatting = new QCheckBox (tr("Play sounds from a person whilst chatting"), grp_soundOptions);
	c_playWhenInvisible = new QCheckBox (tr("Play chat sounds only when window is invisible"), grp_soundOptions);

	c_enableSounds->setChecked(config_file.readBoolEntry("Sounds", "PlaySound", true));
	c_playWhilstChatting->setChecked(config_file.readBoolEntry("Sounds", "PlaySoundChat", true));
	c_playWhenInvisible->setChecked(config_file.readBoolEntry("Sounds", "PlaySoundChatInvisible", true));

	addPage(soundOptionsPage, tr("Sound"));
}


/**
	opcje ogolne
**/
void Wizard::showGeneralOptionsPage()
{	generalOptionsPage = new QVBox(this);

	new QLabel(tr("<h3>Please setup general options</h3>"), generalOptionsPage);

	QGroupBox *grp_otherOptions = new QGroupBox(tr("General"), generalOptionsPage);
	grp_otherOptions->setInsideMargin(10);
	grp_otherOptions->setColumns(1);
	grp_otherOptions->setInsideSpacing(4);
	c_privateStatus = new QCheckBox (tr("Private status"), grp_otherOptions);
	c_showBlocking = new QCheckBox (tr("Show contacts blocking you"), grp_otherOptions);
	c_showBlocked = new QCheckBox (tr("Show contacts that you are blocking"), grp_otherOptions);
	c_startDocked = new QCheckBox (tr("Start docked"), grp_otherOptions);

	c_privateStatus->setChecked(config_file.readBoolEntry("General", "PrivateStatus", false));
	c_showBlocked->setChecked(config_file.readBoolEntry("General", "ShowBlocked", true));
	c_showBlocking->setChecked(config_file.readBoolEntry("General", "ShowBlocking", true));
	c_startDocked->setChecked(config_file.readBoolEntry("General", "RunDocked", true));
	
	QGroupBox *grp_historyOptions = new QGroupBox(tr("History options"), generalOptionsPage);
	grp_historyOptions->setInsideMargin(10);
	grp_historyOptions->setColumns(1);
	grp_historyOptions->setInsideSpacing(4);
	c_logMessages = new QCheckBox (tr("Don't log messages"), grp_historyOptions);
	c_logStatusChanges = new QCheckBox (tr("Don't log status changes"), grp_historyOptions);
	
	c_logMessages->setChecked(!config_file.readBoolEntry("History", "Logging", true));
	c_logStatusChanges->setChecked(config_file.readBoolEntry("History", "DontSaveStatusChanges", false));

	addPage(generalOptionsPage, tr("General"));
}


/**
	opcje dymkow
**/
void Wizard::showHintsOptionsPage()
{	hintsOptionsPage = new QVBox(this);

	new QLabel(tr("<h3>Please setup hints options</h3>"), hintsOptionsPage);

	QGroupBox *grp_hintsOptions = new QGroupBox(tr("Hints"), hintsOptionsPage);
	grp_hintsOptions->setInsideMargin(10);
	grp_hintsOptions->setColumns(2);
	grp_hintsOptions->setInsideSpacing(4);

	new QLabel(tr("Please choose hints design"), grp_hintsOptions);
	cb_hintsTheme = new QComboBox(grp_hintsOptions);
	for (int i=0; i<hintColorNumber; ++i)
		cb_hintsTheme->insertItem(tr(hintColorsNames[i]));

	new QLabel(tr("Preview"), grp_hintsOptions);

	preview = new QLabel (tr("<b>User</b> changed status to <b>Busy</b>"), grp_hintsOptions);
	preview->setFont(QFont("sans", 10));	//<-----------------------------------
	preview->setPaletteForegroundColor(hintColors[0][1]);
	preview->setPaletteBackgroundColor(hintColors[0][0]);
	preview->setAlignment(Qt::AlignCenter);
	preview->setFixedWidth(260);
	preview->setAutoResize(true);

	new QLabel("", grp_hintsOptions);
	
	preview2 = new QLabel (tr("<b>Error</b>: (192.168.0.1) Disconnection has occured"), grp_hintsOptions);
	preview2->setFont(QFont("sans", 10));	//<-----------------------------------
	preview2->setPaletteForegroundColor(hintColors[0][1]);
	preview2->setPaletteBackgroundColor(hintColors[0][0]);
	preview2->setAlignment(Qt::AlignCenter);
	preview2->setFixedWidth(260);
	preview2->setAutoResize(true);

	QGroupBox *grp_hintsOptions2 = new QGroupBox(tr("Hints construction"), hintsOptionsPage);
	grp_hintsOptions2->setInsideMargin(10);
	grp_hintsOptions2->setColumns(2);
	grp_hintsOptions2->setInsideSpacing(4);
	
	new QLabel(tr("Please choose hints type"), grp_hintsOptions2);
	cb_hintsType = new QComboBox(grp_hintsOptions2);
	for (int i=0; i<hintsNumber; ++i)
		cb_hintsType->insertItem(tr(hintLookName[i]));
	
	new QLabel(tr("Preview"), grp_hintsOptions2);
	preview4 = new QLabel (toDisplay(hintLook[0]), grp_hintsOptions2);
	preview4->setFont(QFont("sans", 10));	//<----------------------------------
	preview4->setPaletteForegroundColor(hintColors[0][1]);
	preview4->setPaletteBackgroundColor(hintColors[0][0]);
	preview4->setAlignment(Qt::AlignCenter);
	preview4->setFixedWidth(200);
	preview4->setAutoResize(true);

	connect(cb_hintsTheme, SIGNAL(activated (int)), this, SLOT(previewHintsTheme(int)));
	connect(cb_hintsType, SIGNAL(activated (int)), this, SLOT(previewHintsType(int)));

	QString hintConstruction=config_file.readEntry("Hints", "NotifyHintSyntax", "");
	if (hintConstruction != "")
		{	int i;
			for (i=0; i<hintsNumber; ++i)
				if (hintConstruction==hintLook[i])
					{	cb_hintsType->setCurrentItem(i);
						preview4->setText(toDisplay(hintLook[i]));
						break;
					}
			if (i==hintsNumber)
				{	cb_hintsType->insertItem(tr("Custom"));
					cb_hintsType->setCurrentItem(i);
					customHint=hintConstruction;
					preview4->setText(toDisplay(hintConstruction));
				}	
		}
		
	addPage(hintsOptionsPage, tr("Hints"));
}


/**
	kolorki i ikonki
**/
void Wizard::showColorsPage()
{	colorsPage = new QVBox(this);
	colorsPage->setSpacing(8);

	new QLabel(tr("<h3>Choose color theme and icons for Kadu</h3>"), colorsPage);

	QGroupBox *grp_colorOptions = new QGroupBox(tr("Colors"),  colorsPage);
	grp_colorOptions->setInsideMargin(10);
	grp_colorOptions->setColumns(2);
	grp_colorOptions->setInsideSpacing(4);
	
	new QLabel(tr("Please choose Kadu design"), grp_colorOptions);
	cb_colorTheme = new QComboBox(grp_colorOptions);

	for (int i=0; i<colorNumber; ++i)
		cb_colorTheme->insertItem(tr(colorsNames[i]));

	QGroupBox *grp_iconsOptions = new QGroupBox(tr("Icons"),  colorsPage);
	grp_iconsOptions->setInsideMargin(10);
	grp_iconsOptions->setColumns(2);
	grp_iconsOptions->setInsideSpacing(4);
	
	new QLabel(tr("Please choose icon theme"), grp_iconsOptions);
	cb_iconTheme = new QComboBox(grp_iconsOptions);
	cb_iconTheme->insertStringList(icons_manager.themes());
	cb_iconTheme->setCurrentText(config_file.readEntry("Look", "IconTheme"));
	if (icons_manager.themes().contains("default"))
	cb_iconTheme->changeItem(tr("Default"), icons_manager.themes().findIndex("default"));
		
	QHBox *grp_icons = new QHBox (grp_iconsOptions);
	iconPreview = new QLabel(grp_icons);
	iconPreview2 = new QLabel(grp_icons);
	iconPreview3 = new QLabel(grp_icons);
	iconPreview4 = new QLabel(grp_icons);

	iconPreview->setPixmap(icons_manager.loadIcon("Online"));
	iconPreview2->setPixmap(icons_manager.loadIcon("BusyWithDescription"));
	iconPreview3->setPixmap(icons_manager.loadIcon("InvisibleWithMobile"));
	iconPreview4->setPixmap(icons_manager.loadIcon("OfflineWithDescriptionMobile"));

	connect(cb_colorTheme, SIGNAL(activated (int)), this, SLOT(previewColorTheme(int)));
	connect(cb_iconTheme, SIGNAL(activated (int)), this, SLOT(previewIconTheme(int)));

	addPage(colorsPage, tr("Colors and icons"));
}


/**
	panel informacyjny
**/
void Wizard::showInfoPanelPage()
{	infoPanelPage = new QVBox(this);
	infoPanelPage->setSpacing(8);

	new QLabel(tr("<h3>Choose your Info panel look</h3>"), infoPanelPage);

	QGroupBox *grp_infoPanelOptions = new QGroupBox(tr("Information panel Theme"),  infoPanelPage);
	grp_infoPanelOptions->setInsideMargin(10);
	grp_infoPanelOptions->setColumns(2);
	grp_infoPanelOptions->setInsideSpacing(4);

	QVBox *grp_checks = new QVBox(grp_infoPanelOptions);
	c_showInfoPanel = new QCheckBox (tr("Show information panel"), grp_checks);
	c_showInfoPanel->setChecked(config_file.readBoolEntry("Look", "ShowInfoPanel", true));
	c_showScrolls = new QCheckBox (tr("Show vertical scrollbar"), grp_checks);
	c_showScrolls->setChecked(config_file.readBoolEntry("Look", "PanelVerticalScrollbar", true));

	cb_panelTheme = new QComboBox(grp_infoPanelOptions);

	for (int i=0; i<panelNumber; ++i)
		cb_panelTheme->insertItem(tr(panelLookName[i]));

	new QLabel(tr("Preview"), grp_infoPanelOptions);
	
	//infoPreview = new KaduTextBrowser (grp_infoPanelOptions);	//-- przymiarka pod zmiane podgladu
	infoPreview = new QLabel (toDisplay(panelLook[0]), grp_infoPanelOptions);
	infoPreview->setFrameStyle(QFrame::Box | QFrame::Plain);
    	infoPreview->setLineWidth(1);
	infoPreview->setAlignment(Qt::AlignVCenter | Qt::WordBreak | Qt::DontClip);
	infoPreview->setAutoResize(true);
	infoPreview->setMaximumWidth(230);
	/*	//--przymiarka pod j.w.
	if (c_showScrolls->isChecked())
		infoPreview->setVScrollBarMode(QScrollView::Auto);
	else 	infoPreview->setVScrollBarMode(QScrollView::AlwaysOff);
	*/
	connect(cb_panelTheme, SIGNAL(activated (int)), this, SLOT(previewPanelTheme(int)));
	//connect(c_showScrolls, SIGNAL(toggled(bool)), this, SLOT(addScrolls(bool)));	//--j.w.
	
	QString panelConstruction=config_file.readEntry("Look", "PanelContents", "");
	if (panelConstruction != "")
		{	int i;
			for (i=0; i<panelNumber; ++i)
				if (panelConstruction==toSave(panelLook[i]))
					{	cb_panelTheme->setCurrentItem(i);
						//infoPreview->setText(QString::fromUtf8(toDisplay(panelLook[i]).ascii()));
						infoPreview->setText(toDisplay(panelLook[i]));
						break;
					}
			if (i==panelNumber)
				{	cb_panelTheme->insertItem(tr("Custom"));
					cb_panelTheme->setCurrentItem(i);
					customPanel=panelConstruction;
					//infoPreview->setText(QString::fromUtf8(toDisplay(panelConstruction).ascii()));
					infoPreview->setText(toDisplay(panelConstruction));
				}	
		}
	addPage(infoPanelPage, tr("Info panel Look"));
}


/**
	wybor stylu qt
**/
void Wizard::showQtStylePage()
{	qtStylePage = new QVBox(this);
	qtStylePage->setSpacing(8);

	new QLabel(tr("<h3>Choose Qt3 theme for Kadu</h3>"), qtStylePage);

	QGroupBox *grp_qtOptions = new QGroupBox(tr("Qt Theme"),  qtStylePage);
	grp_qtOptions->setInsideMargin(10);
	grp_qtOptions->setColumns(2);
	grp_qtOptions->setInsideSpacing(4);
	
	new QLabel(tr("Please choose Qt3 design for Kadu"), grp_qtOptions);
	cb_qtTheme = new QComboBox(grp_qtOptions);

	QStringList sl_themes=QStyleFactory::keys();
	cb_qtTheme->insertStringList(sl_themes);
	if(!sl_themes.contains(QApplication::style().name()))
		cb_qtTheme->setCurrentText(tr("Unknown"));
	else
		cb_qtTheme->setCurrentText(QApplication::style().name());
	
	connect(cb_qtTheme, SIGNAL(activated(int)), this, SLOT(previewQtTheme(int)));

	addPage(qtStylePage, tr("Qt Look"));
}


/**
	pozegnanie
**/
void Wizard::showGreetingsPage()
{	greetingsPage = new QVBox(this);
	greetingsPage->setSpacing(8);

	new QLabel(tr("<h2>Congratulations</h2><h3>You have just configured Kadu</h3>You can adjust settings using the configuration icon on toolbar or simply by pressing F2. It's recomended to read the documentation before changing some advanced settings. If you have questions or problems with Kadu look at <b>www.kadu.net/forum</b> and be our guest.<br><h3>Enjoi using Kadu ;)<br>Kadu Team</h3>"), greetingsPage);
	
	addPage(greetingsPage, tr("Congratulations"));
	setFinishEnabled(greetingsPage, TRUE);
}


/**
	ustawia jezyk Kadu
**/
void Wizard::setLanguage(int languageId)	/* ustawia jezyk */
{	QString language;
	switch (languageId)
		{	case 1:	language="pl";	break;
			case 2:	language="it";	break;
			case 3:	language="de";	break;
			default: language="en";
		}
	config_file.writeEntry("General", "Language", language);
}


/**
	Ustawienie konta GG
**/
void Wizard::setGaduAccount()
{	kdebugf();
	if (rb_haveNumber->isChecked())
		{	config_file.writeEntry("General", "UIN", l_ggNumber->text());
			config_file.writeEntry("General", "Password", pwHash(l_ggPassword->text()));
		}
	else
		{	bool isOk=true;
			if (l_ggNewPasssword->text() != l_ggNewPassswordRetyped->text())
				{	QMessageBox::information(0, tr("Kadu Wizard"), tr("Error data typed in required fields.\n\nPasswords typed in "
						"both fields (\"New password\" and \"Retype new password\") "
						"should be the same!"), tr("OK"), 0, 0, 1);
					isOk=false;
				}
			if (!l_ggNewPasssword->text().length())
				{	QMessageBox::warning(this, "Kadu Wizard", tr("Please fill out all fields"), tr("OK"), 0, 0, 1);
					isOk=false;
				}
			if (isOk) gadu->registerAccount(l_email->text(), l_ggNewPasssword->text());
			//connect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredAccount(bool, UinType)));
		}
	kdebugf2();
}


/**
	Zapisanie parametrow nowego konta
**/
void Wizard::registeredAccount(bool ok, UinType uin)
{	kdebugf();
	if (ok) 
		{	config_file.writeEntry("General", "UIN", (int)uin);
			config_file.writeEntry("General", "Password", pwHash(l_ggNewPasssword->text()));
			gadu->status().setOnline();	//jak zarejestrowal to od razu sie laczy
		}
	else QMessageBox::warning(0, tr("Register user"),
				tr("An error has occured while registration. Please try again later."), tr("OK"), 0, 0, 1);
	kdebugf2();
}


/**
	sprawdzenie czy trzeba importowac liste kontaktow czy nie i ew. import
**/
void Wizard::tryImport()
{	kdebugf();
	if (c_importContacts->isChecked())
		{	if (gadu->status().isOffline())
				{	gadu->status().setOnline();	//kaze sie polaczyc i podpina sie pod sygnal polaczenia sie z siecia
					connect(gadu, SIGNAL(connected()), this, SLOT(connected()));
				}	//jak polaczony to bez cyrkow robi import
			else if (!gadu->doImportUserList()) QMessageBox::information(0, tr("Kadu Wizard"), tr("User list wasn't imported becouse of some error"), tr("OK"), 0, 0, 1);
		}
	kdebugf2();
}


/**
	ustawienia chata
**/
void Wizard::setChatOptions()
{	kdebugf();
	config_file.writeEntry("Chat", "MessageAcks", c_waitForDelivery->isChecked());
	config_file.writeEntry("Chat", "AutoSend", c_enterSendsMessage->isChecked());
	config_file.writeEntry("Chat", "OpenChatOnMessage", c_openOnNewMessage->isChecked());
	config_file.writeEntry("Chat", "BlinkChatTitle", c_flashTitleOnNewMessage->isChecked());
	config_file.writeEntry("Chat", "IgnoreAnonymousUsers", c_ignoreAnonyms->isChecked());
	kdebugf2();
}


/**
	ustawienia dzwieku
**/
void Wizard::setSoundOptions()
{	kdebugf();
	config_file.writeEntry("Sounds", "PlaySound", c_enableSounds->isChecked());
	config_file.writeEntry("Sounds", "PlaySoundChat", c_playWhilstChatting->isChecked());
	config_file.writeEntry("Sounds", "PlaySoundChatInvisible", c_playWhenInvisible->isChecked());
	kdebugf2();
}


/**
	ustawienia ogolne
**/
void Wizard::setGeneralOptions()
{	kdebugf();
	config_file.writeEntry("General", "PrivateStatus", c_privateStatus->isChecked());
	config_file.writeEntry("General", "ShowBlocked", c_showBlocked->isChecked());
	config_file.writeEntry("General", "ShowBlocking", c_showBlocking->isChecked());
	config_file.writeEntry("General", "RunDocked", c_startDocked->isChecked());
	config_file.writeEntry("History", "Logging", !c_logMessages->isChecked());
	config_file.writeEntry("History", "DontSaveStatusChanges", c_logStatusChanges->isChecked());
	kdebugf2();
}


/**
	zapis przegladarki
**/
void Wizard::setBrowser()
{	kdebugf();
	config_file.writeEntry("Chat", "WebBrowser", l_customBrowser->text());
	config_file.writeEntry("Chat", "WebBrowserNo", cb_browser->currentItem());
	kdebugf2();
}


/**
	zmiana wartosci w combo z wyborem przegladarki - szuka jej
**/
void Wizard::findAndSetWebBrowser(int selectedBrowser)
{
	ChatSlots::findBrowser(selectedBrowser, cb_browser, cb_browserOptions, l_customBrowser);
	//if (selectedBrowser!=0)
	//	l_customBrowser->setEnabled(false);
	l_customBrowser->setEnabled(!selectedBrowser);
}


/**
	wybiera opcje dla przegladarki
**/
void Wizard::findAndSetBrowserOption(int selectedOption)
{
	ChatSlots::setBrowserOption(selectedOption, l_customBrowser, cb_browser->currentItem());
}


/**
	podglad szaty graficznej dymkow
**/
void Wizard::previewHintsTheme(int hintsThemeID)
{	preview->setPaletteForegroundColor(QColor(hintColors[hintsThemeID][1]));
	preview->setPaletteBackgroundColor(QColor(hintColors[hintsThemeID][0]));
	preview2->setPaletteForegroundColor(QColor(hintColors[hintsThemeID][1]));
	preview2->setPaletteBackgroundColor(QColor(hintColors[hintsThemeID][0]));
	preview4->setPaletteForegroundColor(QColor(hintColors[hintsThemeID][1]));
	preview4->setPaletteBackgroundColor(QColor(hintColors[hintsThemeID][0]));
}


/**
	podglad typu dymkow
**/
void Wizard::previewHintsType(int hintsTypeID)
{	if (hintsTypeID==hintsNumber)
		preview4->setText(toDisplay(customHint));
	else 
		preview4->setText(toDisplay(hintLook[hintsTypeID]));
}


/**
	zapisuje ustawienia dymkow
**/
void Wizard::setHints()
{	kdebugf();
	QColor bg_color, fg_color;
	bg_color=preview->paletteBackgroundColor();
	fg_color=preview->paletteForegroundColor();

	config_file.writeEntry("Hints", "HintBlocking_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintBusyD_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintBusy_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintError_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintInvisibleD_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintInvisible_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintMessage_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintNewChat_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintNewMessage_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintOfflineD_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintOffline_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintOnlineD_bgcolor", bg_color);
	config_file.writeEntry("Hints", "HintOnline_bgcolor", bg_color);
	
	config_file.writeEntry("Hints", "HintBlocking_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintBusyD_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintBusy_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintError_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintInvisibleD_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintInvisible_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintMessage_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintNewChat_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintNewMessage_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintOfflineD_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintOffline_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintOnlineD_fgcolor", fg_color);
	config_file.writeEntry("Hints", "HintOnline_fgcolor", fg_color);

	config_file.writeEntry("Hints", "NotifyHintUseSyntax", true);
	
	if (cb_hintsType->currentItem()==0)
		{	config_file.writeEntry("Hints", "NotifyHintUseSyntax", false);
			config_file.writeEntry("Hints", "NotifyHintSyntax", "");
		}
	else
		{	if (cb_hintsType->currentItem()==hintsNumber) config_file.writeEntry("Hints", "NotifyHintSyntax", tr(customHint));
			else config_file.writeEntry("Hints", "NotifyHintSyntax", tr(hintLook[cb_hintsType->currentItem()]));
		}
	kdebugf2();
}


/**
	podglad kolorkow i ich zapis
**/
void Wizard::previewColorTheme(int colorThemeID)
{	kdebugf();
	config_file.writeEntry("Look", "ChatMyBgColor", colors[colorThemeID][0]);
	config_file.writeEntry("Look", "ChatMyFontColor", colors[colorThemeID][1]);
	config_file.writeEntry("Look", "ChatUsrBgColor", colors[colorThemeID][2]);
	config_file.writeEntry("Look", "ChatUsrFontColor", colors[colorThemeID][3]);
	config_file.writeEntry("Look", "InfoPanelBgColor", colors[colorThemeID][4]);
	config_file.writeEntry("Look", "InfoPanelFgColor", colors[colorThemeID][5]);
	config_file.writeEntry("Look", "UserboxBgColor", colors[colorThemeID][6]);
	config_file.writeEntry("Look", "UserboxFgColor", colors[colorThemeID][7]);
	
	kadu->changeAppearance();
	kdebugf2();
}


/**
	podglad ikonek
**/
void Wizard::previewIconTheme(int iconThemeID)
{	QString iconName=cb_iconTheme->currentText();
	if (iconName == tr("Default"))
	    iconName= "default";
	
	icons_manager.clear();
	icons_manager.setTheme(iconName);
	
	QString path=icons_manager.iconPath("Online");
	for (int i=0; i<cb_iconTheme->count(); ++i)
		if (i!=iconThemeID) path.replace(cb_iconTheme->text(i), cb_iconTheme->text(iconThemeID));
	path.replace(tr("Default"), "default");
	iconPreview->setPixmap(path);
	path.remove("online.png");
	iconPreview2->setPixmap(path+"busy_d.png");
	iconPreview3->setPixmap(path+"invisible_m.png");
	iconPreview4->setPixmap(path+"offline_d_m.png");
}


/**
	zapisuje ustawienie ikonek (i kolorkow)
**/
void Wizard::setColorsAndIcons()
{	kdebugf();
	QString newIconTheme=cb_iconTheme->currentText();
	QString oldIconTheme=config_file.readEntry("Look", "IconTheme", "default");
	if (newIconTheme!=oldIconTheme)
		{	newIconTheme.replace(tr("Default"), "default");
			icons_manager.clear();
			icons_manager.setTheme(newIconTheme);
			ToolBar::refreshIcons();
			UserBox::userboxmenu->refreshIcons();
			icons_manager.refreshMenus();
			kadu->changeAppearance();	
		}
	else 	newIconTheme.replace(tr("Default"), "default");
		
	config_file.writeEntry("Look", "IconTheme", newIconTheme);
	kdebugf2();
}


/**
	podglad panelu
**/
void Wizard::previewPanelTheme(int panelThemeID)
{	if (panelThemeID==panelNumber)
		//infoPreview->setText(QString::fromUtf8(toDisplay(customPanel.ascii())));
		infoPreview->setText(toDisplay(customPanel));
	else 
		//infoPreview->setText(QString::fromUtf8(toDisplay(panelLook[cb_panelTheme->currentItem()]).ascii()));
		infoPreview->setText(toDisplay(panelLook[cb_panelTheme->currentItem()]));
}


/**
	zapisanie wygladu panelu
**/
void Wizard::setPanelTheme()
{	kdebugf();
	config_file.writeEntry("Look", "ShowInfoPanel", c_showInfoPanel->isChecked());
	config_file.writeEntry("Look", "PanelVerticalScrollbar", c_showScrolls->isChecked());
	if (cb_panelTheme->currentItem()==panelNumber)
		config_file.writeEntry("Look", "PanelContents", customPanel);
	else
		config_file.writeEntry("Look", "PanelContents", toSave(panelLook[cb_panelTheme->currentItem()]));
	kdebugf2();
}


/**
	podglad stylu i jego zapis
**/
void Wizard::previewQtTheme(int themeID)
{	QString new_style=cb_qtTheme->text(themeID);
	if(new_style!=tr("Unknown") && new_style != QApplication::style().name())
		{	QApplication::setStyle(new_style);
			config_file.writeEntry("Look", "QtStyle", new_style);
		}
}


/**
	nacisniecie zakoncz i zapisanie konfiguracji (o ile nie nastapilo wczesniej)
**/
void Wizard::finishPressed()
{	kdebugf();
	setChatOptions();
	setSoundOptions();
	setGeneralOptions();
	setBrowser();
	setHints();
	setColorsAndIcons();
	setPanelTheme();
	tryImport();
	kdebugf2();
}


/**
	nacisniecie dalej - zrobione obejscie miedzy istniejacym a nowym kontem gg
**/
void Wizard::nextPressed()
{	if (noNewAccount)
		{	if (currentPage()==welcomePage) QWizard::showPage(languagePage);
			else if (currentPage()==ggNumberSelect) QWizard::showPage(ggCurrentNumberPage);
			else if (currentPage()==ggNewNumberPage) QWizard::showPage(generalOptionsPage);
		}
	else if ((currentPage()==ggCurrentNumberPage) && (rb_dontHaveNumber->isChecked())) 
		QWizard::showPage(ggNewNumberPage);	
	else if ((currentPage()==ggNewNumberPage) && (rb_haveNumber->isChecked())) 
		QWizard::showPage(generalOptionsPage);
	
	if ((currentPage()==generalOptionsPage) && (gadu->status().isOffline()))		//jesli przeszedl jedno pole dalej niz konf. konta i nie polaczony
		setGaduAccount();	//to zapisuje ustawienia konta
}


/**
	nacisniecie cofnij - to co w dalej ale do tylu
**/
void Wizard::backPressed()
{	if (noNewAccount)
		{	if (currentPage()==ggNewNumberPage)	QWizard::showPage(ggCurrentNumberPage);
			else if (currentPage()==ggNumberSelect)	
				{	setBackEnabled(languagePage, false);
					QWizard::showPage(languagePage);
				}
		}
	else 	if ((currentPage()==ggCurrentNumberPage) && (rb_dontHaveNumber->isChecked()))
		QWizard::showPage(ggNumberSelect);	
	else if ((currentPage()==ggNewNumberPage) && (rb_haveNumber->isChecked()))
		QWizard::showPage(ggCurrentNumberPage);	
}


/**
	wywolanie wizarda z menu
**/
void Wizard::wizardStart()
{	noNewAccount=true;
	QWizard::showPage(languagePage);
	setBackEnabled(languagePage, false);
	exec();
	noNewAccount=false;
}


/**
	wykorzystywane przy wyswietelaniu podgladu panelu inf. i dymkow
**/
QString Wizard::toDisplay(QString s)
{	s.replace("%s", tr("Busy"));
	s.replace("%d", tr("My description"));
	s.replace("%i", "192.168.0.1");
	s.replace("%n", "Jimbo");
	s.replace("%a", "jimbo");
	s.replace("%f", tr("Mark"));
	s.replace("%r", tr("Smith"));
	s.replace("%m", "+4812345679");
	s.replace("%u", "999999");
	s.replace("%g", tr("Friends"));
	s.replace("%o", "");
	s.replace("%v", "host.serwer.net");
	s.replace("%p", "80");
	s.replace("%e", "jimbo@mail.server.net");
	s.replace("[", "");
	s.replace("]", "");
	s.replace("changed status to", tr("changed status to"));
	s.replace("You are not on the list", tr("You are not on the list"));

	int i; 
	for (i=0; i<s.contains("$KADU_CONF"); ++i)
		s.replace("$KADU_CONF", ggPath());
	
	for (i=0; i<s.contains("$KADU_SHARE"); ++i)
		s.replace("$KADU_SHARE", dataPath("kadu"));
	
	for (i=0; i<s.contains("$HOME"); ++i)
		s.replace("$HOME", getenv("HOME"));
	
	return s;
}


/**
	przy zapisie panelu - zamienia sciezke na odpowiednia - pomocne gdy sie nie ma kadu w /usr
**/
QString Wizard::toSave(QString s)
{	
	s.replace("You are not on the list", tr("You are not on the list"));

	int i; 
	for (i=0; i<s.contains("$KADU_SHARE"); ++i)
		s.replace("$KADU_SHARE", dataPath("kadu"));
	
	for (i=0; i<s.contains("$KADU_CONF"); ++i)
		s.replace("$KADU_CONF", ggPath());
	
	for (i=0; i<s.contains("$HOME"); ++i)
		s.replace("$HOME", getenv("HOME"));
	
	return s;
}


/**
	po zaimportowaniu listy kontaktow sie wywoluje
**/
void Wizard::userListImported(bool ok, UserList& userList)
{	kdebugf();

	userlist.merge(userList);
	userlist.writeToFile();	//zaraz zapisuje liste bo kadu zaraz pewnie poleci

	kadu->userbox()->clear();
	kadu->userbox()->clearUsers();

	for (UserList::const_iterator i = userlist.begin(); i != userlist.end(); ++i)
		kadu->userbox()->addUser((*i).altNick());

	UserBox::all_refresh();
	
	kdebugf2();
}


/**
	po polaczeniu sie z siecia robi import - podpinane tylko gdy kadu nie jest polaczone w momencie nacisniecia Finish
**/
void Wizard::connected()
{	if (!gadu->doImportUserList()) 
		QMessageBox::information(0, tr("Kadu Wizard"), tr("User list wasn't imported becouse of some error"), tr("OK"), 0, 0, 1);
	disconnect(gadu, SIGNAL(connected()), this, SLOT(connected()));
}


Wizard *startWizardObj;
