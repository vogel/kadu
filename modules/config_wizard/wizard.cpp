/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qprocess.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qpopupmenu.h>

#include <stdlib.h>
#include <unistd.h>

#include "addons.h"
#include "action.h"
#include "chat_manager_slots.h"
#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "kadu_text_browser.h"
#include "message_box.h"
#include "modules.h"
#include "userbox.h"
#include "wizard.h"

/**
 * @ingroup config_wizard
 * @{
 */
unsigned int informationPanelCount = sizeof(informationPanelSyntax) / sizeof(informationPanelSyntax[0]);
unsigned int hintCount = sizeof(hintSyntax) / sizeof(hintSyntax[0]);
unsigned int hintColorCount = sizeof(hintColors) / sizeof(hintColors[0]);
unsigned int kaduColorCount = sizeof(kaduColors) / sizeof(kaduColors[0]);
QString currentColors[8];	//tu przechowujemy aktualny zestaw kolorow
QString currentHints[13][2]; //a tu aktualne kolorki hintow
bool registered = false;	//potrzebne przy blokowaniu/odblokowywaniu przyciska Dalej

extern "C" int config_wizard_init()
{
	kdebugf();
	wizardStarter = new WizardStarter(NULL, "wizardStarter");

	if (config_file.readNumEntry("General", "UIN", 0) == 0 || config_file.readEntry("General", "Password").isEmpty())
		wizardStarter->start();

	kdebugf2();
	return 0;
}

extern "C" void config_wizard_close()
{
	kdebugf();
	if (wizardStarter)
	{
		delete wizardStarter;
		wizardStarter = NULL;
	}
	kdebugf2();
}

WizardStarter::WizardStarter(QObject *parent, const char *name)
	: QObject(parent, name),
	menuPos(kadu->mainMenu()->insertItem(icons_manager->loadIcon("ConfigurationWizard"),
		tr("Configuration Wizard"), this, SLOT(start()), 0, -1, 0))
{
}

WizardStarter::~WizardStarter()
{
	kadu->mainMenu()->removeItem(menuPos);
	if (startWizardObj)
	{
		delete startWizardObj;
		startWizardObj = NULL;
	}
}

void WizardStarter::start()
{
	kdebugf();
	startWizardObj = new Wizard(NULL, "startWizardObj");
	startWizardObj->wizardStart();
	kdebugf2();
}

Wizard::Wizard(QWidget *parent, const char *name, bool modal)
	: QWizard(parent, name, modal),
	noNewAccount(false), welcomePage(0), ggNumberSelect(0), ggCurrentNumberPage(0),
	ggNewNumberPage(0), languagePage(0), chatOptionsPage(0), wwwOptionsPage(0),
	soundOptionsPage(0), generalOptionsPage(0), greetingsPage(0), hintsOptionsPage(0),
	colorsPage(0), qtStylePage(0), infoPanelPage(0), rb_haveNumber(0), rb_dontHaveNumber(0),
	l_ggNumber(0), l_ggPassword(0), l_ggNewPasssword(0), l_ggNewPassswordRetyped(0),
	l_email(0), l_customBrowser(0), c_importContacts(0), c_waitForDelivery(0),
	c_enterSendsMessage(0), c_openOnNewMessage(0), c_flashTitleOnNewMessage(0),
	c_ignoreAnonyms(0), c_logMessages(0), c_logStatusChanges(0), c_privateStatus(0),
	c_showBlocked(0), c_showBlocking(0), c_startDocked(0), c_enableSounds(0),
	c_playWhilstChatting(0), c_playWhenInvisible(0), c_showInfoPanel(0), c_showScrolls(0),
	cb_browser(0), cb_browserOptions(0), cb_hintsTheme(0), cb_hintsType(0),
	cb_colorTheme(0), cb_iconTheme(0), cb_qtTheme(0), cb_panelTheme(0), cb_soundModule(0),
	preview(0), preview2(0), preview4(0), iconPreview(0), iconPreview2(0),
	iconPreview3(0), iconPreview4(0), moduleInfo(0), customHint(), customPanel(),
	infoPreview(0), registerAccount(0)
{
	kdebugf();
	setCaption(tr("Kadu Wizard"));
	setMinimumSize(470, 300);	/* ustawia rozmiar okna */

	createWelcomePage();		/* powitanie */
	createLanguagePage();		/* ustawienie jezyka */
	createGGNumberSelect();		/* wyswietla okno ustawien numerka */
	createGGCurrentNumberPage();
	createGGNewNumberPage();
	createGeneralOptionsPage(); /* inne */
	createChatOpionsPage();		/* ustawienia gadki */
	createColorsPage();			/* kolory */
	createHintsOptionsPage();	/* dymki */
	createInfoPanelPage();		/* wybor panelu */
	createQtStylePage();		/* Qt3 theme */
	createSoundOptionsPage();	/* dzwieki */
	createWWWOpionsPage();		/* ustawienie przegladarki */
	createGreetingsPage();		/* okno koncowe */

	QPushButton *finish_button = finishButton();
	connect(finish_button, SIGNAL(clicked()), this, SLOT(finishClicked()));
	finish_button->setText(tr("Finish"));

	QPushButton *next_button = nextButton();
	connect(next_button, SIGNAL(clicked()), this, SLOT(nextClicked()));
	next_button->setText(tr("Next >"));

	QPushButton *back_button = backButton();
	connect(back_button, SIGNAL(clicked()), this, SLOT(backClicked()));
	back_button->setText(tr("< Back"));

	cancelButton()->setText(tr("Cancel"));
	connect(cancelButton(), SIGNAL(clicked()), this, SLOT(cancelClicked()));

	helpButton()->hide();

	kdebugf2();
}

Wizard::~Wizard()
{
	kdebugf();
	disconnect(cb_panelTheme, SIGNAL(activated (int)), this, SLOT(previewPanelTheme(int)));
	disconnect(c_showScrolls, SIGNAL(toggled(bool)), this, SLOT(addScrolls(bool)));
	kdebugf2();
}

/**
	naci¶niêcie zakoñcz i zapisanie konfiguracji (o ile nie nast±pi³o wcze¶niej)
**/
void Wizard::finishClicked()
{
	kdebugf();
	setGeneralOptions();
	setChatOptions();
	setColorsAndIcons();
	setHints();
	setPanelTheme();
	setSoundOptions();
	setBrowser();
	tryImport();

	startWizardObj = NULL;
	deleteLater();

	kdebugf2();
}

void Wizard::cancelClicked()
{
	kdebugf();
	startWizardObj = NULL;
	deleteLater();
	kdebugf2();
}

void Wizard::closeEvent(QCloseEvent *e)
{
	kdebugf();
	QWizard::closeEvent(e);
	startWizardObj = NULL;
	deleteLater();
}

/**
	naci¶niêcie dalej - zrobione obej¶cie miêdzy istniej±cym a nowym kontem gg
**/
void Wizard::nextClicked()
{
	kdebugf();
	if (noNewAccount)
	{
		if (currentPage() == welcomePage)
			QWizard::showPage(languagePage);
		else if (currentPage() == ggNumberSelect)
			QWizard::showPage(ggCurrentNumberPage);
		else if (currentPage() == ggNewNumberPage)
			QWizard::showPage(generalOptionsPage);
	}
	else if (currentPage() == ggCurrentNumberPage && rb_dontHaveNumber->isChecked())
	{
		QWizard::showPage(ggNewNumberPage);
		if (!registered)
			nextButton()->setEnabled(false);
	}
	else if (currentPage() == ggNewNumberPage && rb_haveNumber->isChecked())
		QWizard::showPage(generalOptionsPage);

	if (currentPage() == generalOptionsPage && rb_haveNumber->isChecked())		//jesli przeszedl jedno pole dalej niz konf. konta
		setOldGaduAccount();	//to zapisuje ustawienia konta
	kdebugf2();
}

/**
	naci¶niêcie cofnij - to co w dalej ale do ty³u
**/
void Wizard::backClicked()
{
	kdebugf();
	if (noNewAccount)
	{
		if (currentPage() == ggNewNumberPage)
			QWizard::showPage(ggCurrentNumberPage);
		else if (currentPage() == ggNumberSelect)
		{
			setBackEnabled(languagePage, false);
			QWizard::showPage(languagePage);
		}
	}
	else if (currentPage() == ggCurrentNumberPage && rb_dontHaveNumber->isChecked())
		QWizard::showPage(ggNumberSelect);
	else if (currentPage() == ggNewNumberPage && rb_haveNumber->isChecked())
		QWizard::showPage(ggCurrentNumberPage);
	kdebugf2();
}

/**
	wywo³anie wizarda z menu
**/
void Wizard::wizardStart()
{
	kdebugf();
	//exec();	//exec otwiera modalnie Wizarda
	show();		//otwiera Wizarda ale nie modalnie
	kdebugf2();
}

/**
	po zaimportowaniu listy kontaktów siê wywo³uje
**/
void WizardStarter::userListImported(bool ok, QValueList<UserListElement> list)
{
	kdebugf();
	disconnect(gadu, SIGNAL(userListImported(bool, QValueList<UserListElement>)),
				this, SLOT(userListImported(bool, QValueList<UserListElement>)));

	if (!ok)
	{
		kdebugf2();
		return;
	}

	userlist->merge(list);
	userlist->writeToConfig();
	kdebugf2();
}

/**
	po polaczeniu sie z siecia robi import - podpinane tylko gdy kadu nie jest polaczone w momencie nacisniecia Finish
**/
void WizardStarter::connected()
{
	if (!gadu->doImportUserList())
	{
		MessageBox::msg(tr("User list wasn't imported because of some error"));
		disconnect(gadu, SIGNAL(userListImported(bool, QValueList<UserListElement>)),
					this, SLOT(userListImported(bool, QValueList<UserListElement>)));
	}

	disconnect(gadu, SIGNAL(connected()), this, SLOT(connected()));
}

void Wizard::setOldGaduAccount()
{
	config_file.writeEntry("General", "UIN", l_ggNumber->text());
	config_file.writeEntry("General", "Password", pwHash(l_ggPassword->text()));
	gadu->changeID(l_ggNumber->text());
}

/**
	Ustawienie konta GG
**/
void Wizard::setGaduAccount()
{
	kdebugf();
	//registerAccount->setEnabled(false);	//blokuje przycisk Register
	//disconnect(registerAccount, SIGNAL(clicked()), this, SLOT(setGaduAccount()));
	bool isOk = true;
	if (l_ggNewPasssword->text() != l_ggNewPassswordRetyped->text())
	{
		MessageBox::msg(tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"New password\" and \"Retype new password\") "
			"should be the same!"));
		isOk = false;
	}
	if (l_ggNewPasssword->text().isEmpty())
	{
		MessageBox::wrn(tr("Please fill all fields"));
		isOk = false;
	}
	if (isOk)
	{
		gadu->registerAccount(l_email->text(), l_ggNewPasssword->text());
		registerAccount->setEnabled(false);
		connect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredAccount(bool, UinType)));
	}
	kdebugf2();
}

/**
	Zapisanie parametrów nowego konta
**/
void Wizard::registeredAccount(bool ok, UinType uin)
{
	kdebugf();
	if (ok)
	{
		config_file.writeEntry("General", "UIN", (int)uin);
		config_file.writeEntry("General", "Password", pwHash(l_ggNewPasssword->text()));
		gadu->changeID(QString::number(uin));
		gadu->status().setOnline();	//jak zarejestrowal to od razu sie laczy
		MessageBox::msg(tr("Registration was successful.\nYou UIN is: ")+QString::number(int(uin))+tr("\nAccount configuration was saved.\nPress Ok to continue"));
		registered = true;
	}
	else
	{
		MessageBox::wrn(tr("An error has occured while registration. Please try again later."));
		registerAccount->setEnabled(true);
	}
	disconnect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredAccount(bool, UinType)));
	nextButton()->setEnabled(true); //odblokowuje Next >
	kdebugf2();
}


/**
	sprawdzenie czy trzeba importowaæ listê kontaktów czy nie i ew. import
**/
void Wizard::tryImport()
{
	kdebugf();
	if (c_importContacts->isChecked())
	{
		connect(gadu, SIGNAL(userListImported(bool, QValueList<UserListElement>)),
				wizardStarter, SLOT(userListImported(bool, QValueList<UserListElement>)));
		if (gadu->status().isOffline())
		{
			connect(gadu, SIGNAL(connected()), wizardStarter, SLOT(connected()));
			gadu->status().setOnline();	//kaze sie polaczyc i podpina sie pod sygnal polaczenia sie z siecia
		}	//jak polaczony to bez cyrkow robi import
		else if (!gadu->doImportUserList())
		{
			MessageBox::msg(tr("User list wasn't imported because of some error"));
			disconnect(gadu, SIGNAL(userListImported(bool, QValueList<UserListElement>)),
						wizardStarter, SLOT(userListImported(bool, QValueList<UserListElement>)));
		}
	}
	kdebugf2();
}

/**
	Powitanie
**/
void Wizard::createWelcomePage()
{
	kdebugf();
	welcomePage = new QVBox(this);
	welcomePage->setSpacing(8);

	new QLabel(tr("<h2>Welcome in Kadu</h2><h3> the Gadu-gadu network client for *nix "
					"and MacOS X.</h3><br><br><font size=+1>This is first time you launch Kadu. "
					"This wizard will help you to configure the basic settings of Kadu. "
					"If you are experienced Kadu user you may omit the wizard by clicking Cancel. "
					"Otherwise click Next.</font>"), welcomePage);

	addPage(welcomePage, tr("Welcome"));
	setNextEnabled(welcomePage, true);
	setHelpEnabled(welcomePage, false);
	kdebugf2();
}

/**
	Wybor opcji z numerkiem gg
**/
void Wizard::createGGNumberSelect()
{
	kdebugf();
	ggNumberSelect = new QVBox(this);

	new QLabel(tr("<h3>Please decide if you want to use your old Gadu-gadu account and "
					"number or to create the new one</h3>"), ggNumberSelect);

	QButtonGroup *grp_numberSelection = new QButtonGroup(tr("Select account option"), ggNumberSelect);
	grp_numberSelection->setInsideMargin(10);
	grp_numberSelection->setColumns(1);
	grp_numberSelection->setInsideSpacing(6);

	rb_haveNumber = new QRadioButton(tr("I have a number"), grp_numberSelection);
	rb_haveNumber->setChecked(true);
	rb_dontHaveNumber = new QRadioButton(tr("I don't have one"), grp_numberSelection);

	addPage(ggNumberSelect, tr("Gadu-gadu account"));
	kdebugf2();
}

/**
	Wyswietlenie ustawienia istniejacego konta
**/
void Wizard::createGGCurrentNumberPage()
{
	kdebugf();
	ggCurrentNumberPage = new QVBox(this);

	new QLabel(tr("<h3>You decided to use your existing account. Please configure it</h3>"), ggCurrentNumberPage);
	QGroupBox *grp_haveNumber = new QGroupBox(tr("Please enter your account settings"), ggCurrentNumberPage);
	grp_haveNumber->setInsideMargin(10);
	grp_haveNumber->setColumns(2);
	grp_haveNumber->setInsideSpacing(4);

	new QLabel(tr("Gadu-gadu number"), grp_haveNumber);
	l_ggNumber = new QLineEdit(grp_haveNumber);
	l_ggNumber->setText(config_file.readEntry("General", "UIN"));
	new QLabel(tr("Gadu-gadu password"),grp_haveNumber);
	l_ggPassword = new QLineEdit(grp_haveNumber);
	l_ggPassword->setEchoMode(QLineEdit::Password);
	l_ggPassword->setText(pwHash(config_file.readEntry("General", "Password")));
	c_importContacts = new QCheckBox(tr("Import contacts"), grp_haveNumber);
	c_importContacts->setChecked(true);

	addPage(ggCurrentNumberPage, tr("Gadu-gadu account"));
	kdebugf2();
}

/**
	Zakladanie nowego konta
**/
void Wizard::createGGNewNumberPage()
{
	kdebugf();
	ggNewNumberPage = new QVBox(this);

	new QLabel(tr("<h3>Please enter your valid e-mail address and password you want "
				"to secure your new Gadu-gadu number</h3>\nPassword must contain at least 5 signs (only letters and numbers)"), ggNewNumberPage);

	QGroupBox *grp_dontHaveNumber = new QGroupBox(QString::null, ggNewNumberPage);
	grp_dontHaveNumber->setInsideMargin(10);
	grp_dontHaveNumber->setColumns(2);
	grp_dontHaveNumber->setInsideSpacing(4);

	new QLabel(tr("Password"), grp_dontHaveNumber);
	l_ggNewPasssword = new QLineEdit(grp_dontHaveNumber);
	l_ggNewPasssword->setEchoMode(QLineEdit::Password);
	new QLabel(tr("Re-type password"), grp_dontHaveNumber);
	l_ggNewPassswordRetyped = new QLineEdit(grp_dontHaveNumber);
	l_ggNewPassswordRetyped->setEchoMode(QLineEdit::Password);
	new QLabel(tr("Your e-mail address"), grp_dontHaveNumber);
	l_email=new QLineEdit(grp_dontHaveNumber);

	registerAccount = new QPushButton(tr("Register"), ggNewNumberPage);
	connect(registerAccount, SIGNAL(clicked()), this, SLOT(setGaduAccount()));

	addPage(ggNewNumberPage, tr("Gadu-gadu account"));
	kdebugf2();
}

/**
	Wybor jezyka
**/
void Wizard::createLanguagePage()
{
	kdebugf();
	languagePage = new QVBox(this);

	new QLabel(tr("<h3>Please select language version of Kadu you want to use</h3>"), languagePage);

	QGroupBox *grp_language = new QGroupBox (tr("Language selection"), languagePage);
	grp_language->setInsideMargin(10);
	grp_language->setColumns(2);
	grp_language->setInsideSpacing(4);
	new QLabel (tr("Choose Kadu language"), grp_language);
	QComboBox *cb_language = new QComboBox (grp_language);
	cb_language->insertItem(tr("English"));
	cb_language->insertItem(tr("Polish"));
	cb_language->insertItem(tr("Italian"));
	cb_language->insertItem(tr("German"));
	cb_language->insertItem(tr("French"));

	QString lang = config_file.readEntry("General", "Language");

	if (lang == "pl")		cb_language->setCurrentItem(1);
	else if (lang == "it")	cb_language->setCurrentItem(2);
	else if (lang == "de")	cb_language->setCurrentItem(3);
	else if (lang == "fr")	cb_language->setCurrentItem(4);

	connect (cb_language, SIGNAL(activated(int)), this, SLOT(setLanguage(int)));

	addPage(languagePage, tr("Language"));
	kdebugf2();
}

/**
	Opcje chata
**/
void Wizard::createChatOpionsPage()
{
	kdebugf();
	chatOptionsPage = new QVBox(this);

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
	kdebugf2();
}

/**
	opcje przegladarki www
**/
void Wizard::createWWWOpionsPage()
{
	kdebugf();
	wwwOptionsPage = new QVBox(this);

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

	ChatManagerSlots::initBrowserOptions(cb_browser, cb_browserOptions, l_customBrowser);
	l_customBrowser->setEnabled(!cb_browser->currentItem());
	l_customBrowser->setText(config_file.readEntry("Chat", "WebBrowser"));

	connect(cb_browser, SIGNAL(activated (int)), this, SLOT(findAndSetWebBrowser(int)));
	connect(cb_browserOptions, SIGNAL(activated (int)), this, SLOT(findAndSetBrowserOption(int)));

	addPage(wwwOptionsPage, tr("WWW"));
	kdebugf2();
}

/**
	opcje dzwieku
**/
void Wizard::createSoundOptionsPage()
{
	kdebugf();
	soundOptionsPage = new QVBox(this);

	new QLabel(tr("<h3>Please setup sounds</h3>"), soundOptionsPage);

	QGroupBox *soundModuleOptions = new QGroupBox(tr("Sound module"), soundOptionsPage);
	soundModuleOptions->setInsideMargin(10);
	soundModuleOptions->setColumns(2);
	soundModuleOptions->setInsideSpacing(4);

	new QLabel (tr("Choose sound module"),soundModuleOptions);

	cb_soundModule = new QComboBox(soundModuleOptions);
	cb_soundModule->insertItem(tr("None"));
	cb_soundModule->insertItem("alsa_sound");
	cb_soundModule->insertItem("ao_sound");
	cb_soundModule->insertItem("arts_sound");
	cb_soundModule->insertItem("dsp_sound");
	cb_soundModule->insertItem("esd_sound");
	cb_soundModule->insertItem("ext_sound");
	cb_soundModule->insertItem("nas_sound");

	connect(cb_soundModule, SIGNAL(activated (int)), this, SLOT(setSoundModule(int)));

	new QLabel (tr("<b>Description:</b>"), soundModuleOptions);
	moduleInfo = new QLabel (tr("Kadu uses various sound modules to play its sounds. "
								"Choosing appropriate module is quite important. "
								"Browse installed sound modules and choose the best for you."),
							soundModuleOptions);
	moduleInfo->setFrameStyle(QFrame::Plain);
	moduleInfo->setAlignment(Qt::WordBreak);
	moduleInfo->setFixedWidth(300);

	if (modules_manager->moduleIsLoaded("alsa_sound"))
	{
		cb_soundModule->setCurrentItem(1);
		moduleInfo->setText(tr("This module play sounds using ALSA - Advanced Linux Sound Architecture driver."));
	}
	else if (modules_manager->moduleIsLoaded("ao_sound"))
	{
		cb_soundModule->setCurrentItem(2);
		moduleInfo->setText(tr("This module uses libao to produce sounds. "
								"This is third-party module not supported by Kadu Team."));
	}
	else if (modules_manager->moduleIsLoaded("arts_sound"))
	{
		cb_soundModule->setCurrentItem(3);
		moduleInfo->setText(tr("This module uses KDE's aRts daemon which mixes "
								"multiple digital sources at real time.<br> When your "
								"soundcard doesn't handle that, this module is for you."));
	}
	else if (modules_manager->moduleIsLoaded("dsp_sound"))
	{
		cb_soundModule->setCurrentItem(4);
		moduleInfo->setText(tr("This module uses OSS kernel module to produce sounds."));
	}
	else if (modules_manager->moduleIsLoaded("esd_sound"))
	{
		cb_soundModule->setCurrentItem(5);
		moduleInfo->setText(tr("This module uses GNOME's Enlightened Sound Daemon "
							"which mixes multiple digital sources at real time.<br> "
							"When your soundcard doesn't handle that, this module is for you."));
	}
	else if (modules_manager->moduleIsLoaded("ext_sound"))
	{
		cb_soundModule->setCurrentItem(6);
		moduleInfo->setText(tr("This module uses external application to produce sounds. "
								"You must specify path to external program in configuration."));
	}
	else if (modules_manager->moduleIsLoaded("nas_sound"))
	{
		cb_soundModule->setCurrentItem(7);
		moduleInfo->setText(tr("This module uses Network Audio System to produce sounds. "
								"Use it when you use NAS sound system."));
	}

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
	kdebugf2();
}

/**
	opcje ogolne
**/
void Wizard::createGeneralOptionsPage()
{
	kdebugf();
	generalOptionsPage = new QVBox(this);

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
	kdebugf2();
}

/**
	opcje dymków
**/
void Wizard::createHintsOptionsPage()
{
	kdebugf();
	hintsOptionsPage = new QVBox(this);

	new QLabel(tr("<h3>Please setup hints options</h3>"), hintsOptionsPage);

	QGroupBox *grp_hintsOptions = new QGroupBox(tr("Hints"), hintsOptionsPage);
	grp_hintsOptions->setInsideMargin(10);
	grp_hintsOptions->setColumns(2);
	grp_hintsOptions->setInsideSpacing(4);

	new QLabel(tr("Please choose hints design"), grp_hintsOptions);
	cb_hintsTheme = new QComboBox(grp_hintsOptions);
	unsigned int i;
	for (i = 0; i < hintColorCount; ++i)
		cb_hintsTheme->insertItem(tr(hintColorsNames[i]));

	cb_hintsTheme->insertItem(tr("Current")); //wlasne ustawienie
	cb_hintsTheme->setCurrentItem(i);
	//teraz musimy je zapamietac
	currentHints[0][0] = config_file.readEntry("Hints", "HintBlocking_bgcolor", "#f0f0f0");
	currentHints[1][0] = config_file.readEntry("Hints", "HintBusyD_bgcolor", "#f0f0f0");
	currentHints[2][0] = config_file.readEntry("Hints", "HintBusy_bgcolor", "#f0f0f0");
	currentHints[3][0] = config_file.readEntry("Hints", "HintError_bgcolor", "#f0f0f0");
	currentHints[4][0] = config_file.readEntry("Hints", "HintInvisibleD_bgcolor", "#f0f0f0");
	currentHints[5][0] = config_file.readEntry("Hints", "HintInvisible_bgcolor", "#f0f0f0");
	currentHints[6][0] = config_file.readEntry("Hints", "HintMessage_bgcolor", "#f0f0f0");
	currentHints[7][0] = config_file.readEntry("Hints", "HintNewChat_bgcolor", "#f0f0f0");
	currentHints[8][0] = config_file.readEntry("Hints", "HintNewMessage_bgcolor", "#f0f0f0");
	currentHints[9][0] = config_file.readEntry("Hints", "HintOfflineD_bgcolor", "#f0f0f0");
	currentHints[10][0] = config_file.readEntry("Hints", "HintOffline_bgcolor", "#f0f0f0");
	currentHints[11][0] = config_file.readEntry("Hints", "HintOnlineD_bgcolor", "#f0f0f0");
	currentHints[12][0] = config_file.readEntry("Hints", "HintOnline_bgcolor", "#f0f0f0");
	currentHints[0][1] = config_file.readEntry("Hints", "HintBlocking_fgcolor", "#000000");
	currentHints[1][1] = config_file.readEntry("Hints", "HintBusyD_fgcolor", "#000000");
	currentHints[2][1] = config_file.readEntry("Hints", "HintBusy_fgcolor", "#000000");
	currentHints[3][1] = config_file.readEntry("Hints", "HintError_fgcolor", "#000000");
	currentHints[4][1] = config_file.readEntry("Hints", "HintInvisibleD_fgcolor", "#000000");
	currentHints[5][1] = config_file.readEntry("Hints", "HintInvisible_fgcolor", "#000000");
	currentHints[6][1] = config_file.readEntry("Hints", "HintMessage_fgcolor", "#000000");
	currentHints[7][1] = config_file.readEntry("Hints", "HintNewChat_fgcolor", "#000000");
	currentHints[8][1] = config_file.readEntry("Hints", "HintNewMessage_fgcolor", "#000000");
	currentHints[9][1] = config_file.readEntry("Hints", "HintOfflineD_fgcolor", "#000000");
	currentHints[10][1] = config_file.readEntry("Hints", "HintOffline_fgcolor", "#000000");
	currentHints[11][1] = config_file.readEntry("Hints", "HintOnlineD_fgcolor", "#000000");
	currentHints[12][1] = config_file.readEntry("Hints", "HintOnline_fgcolor", "#000000");


	new QLabel(tr("Preview"), grp_hintsOptions);

	preview = new QLabel (toDisplay(tr("<b>User</b> changed status to <b>Busy</b>")), grp_hintsOptions);
//	preview->setFont(QFont("sans", 10));	//<-----------------------------------
	preview->setPaletteForegroundColor(currentHints[2][1]);
	preview->setPaletteBackgroundColor(currentHints[2][0]);
	preview->setAlignment(Qt::AlignCenter);
	preview->setFixedWidth(260);
	preview->setAutoResize(true);

	new QLabel(QString::null, grp_hintsOptions);

	preview2 = new QLabel (tr("<b>Error</b>: (192.168.0.1) Disconnection has occured"), grp_hintsOptions);
//	preview2->setFont(QFont("sans", 10));	//<-----------------------------------
	preview2->setPaletteForegroundColor(currentHints[3][1]);
	preview2->setPaletteBackgroundColor(currentHints[3][0]);
	preview2->setAlignment(Qt::AlignCenter);
	preview2->setFixedWidth(260);
	preview2->setAutoResize(true);

	QGroupBox *grp_hintsOptions2 = new QGroupBox(tr("Hints construction"), hintsOptionsPage);
	grp_hintsOptions2->setInsideMargin(10);
	grp_hintsOptions2->setColumns(2);
	grp_hintsOptions2->setInsideSpacing(4);

	new QLabel(tr("Please choose hints type"), grp_hintsOptions2);
	cb_hintsType = new QComboBox(grp_hintsOptions2);
	for (unsigned int i = 0; i < hintCount; ++i)
		cb_hintsType->insertItem(tr(hintSyntaxName[i]));

	new QLabel(tr("Preview"), grp_hintsOptions2);
	preview4 = new QLabel (toDisplay(hintSyntax[0]), grp_hintsOptions2);
//	preview4->setFont(QFont("sans", 10));	//<----------------------------------
	preview2->setPaletteForegroundColor(currentHints[2][1]);
	preview2->setPaletteBackgroundColor(currentHints[2][0]);
	preview4->setAlignment(Qt::AlignCenter);
	preview4->setFixedWidth(260);
	preview4->setAutoResize(true);

	connect(cb_hintsTheme, SIGNAL(activated(int)), this, SLOT(previewHintsTheme(int)));
	connect(cb_hintsType, SIGNAL(activated(int)), this, SLOT(previewHintsType(int)));

	QString hintConstruction = config_file.readEntry("Hints", "NotifyHintSyntax");
	if (!hintConstruction.isEmpty())
	{
		unsigned int i;
		for (i = 0; i < hintCount; ++i)
			if (hintConstruction == hintSyntax[i])
			{
				cb_hintsType->setCurrentItem(i);
				preview4->setText(toDisplay(hintSyntax[i]));
				break;
			}
		if (i == hintCount)
		{
			cb_hintsType->insertItem(tr("Custom"));
			cb_hintsType->setCurrentItem(i);
			customHint=hintConstruction;
			preview4->setText(toDisplay(hintConstruction));
		}
	}

	addPage(hintsOptionsPage, tr("Hints"));
	kdebugf2();
}

/**
	kolorki i ikonki
**/
void Wizard::createColorsPage()
{
	kdebugf();
	colorsPage = new QVBox(this);
	colorsPage->setSpacing(8);

	new QLabel(tr("<h3>Choose color theme and icons for Kadu</h3>"), colorsPage);

	QGroupBox *grp_colorOptions = new QGroupBox(tr("Colors"), colorsPage);
	grp_colorOptions->setInsideMargin(10);
	grp_colorOptions->setColumns(2);
	grp_colorOptions->setInsideSpacing(4);

	new QLabel(tr("Please choose Kadu design"), grp_colorOptions);
	cb_colorTheme = new QComboBox(grp_colorOptions);
	unsigned int i;
	for (i = 0; i < kaduColorCount; ++i)
		cb_colorTheme->insertItem(tr(kaduColorNames[i]));

	cb_colorTheme->insertItem(tr("Current"));	//zeby nie stracic wlasnego ustawienia kolorow przez zabawe combo
	cb_colorTheme->setCurrentItem(i);
	//zapamietujemy ustawienia kolorkow:
	currentColors[0] = config_file.readEntry("Look", "ChatMyBgColor");
	currentColors[1] = config_file.readEntry("Look", "ChatMyFontColor");
	currentColors[2] = config_file.readEntry("Look", "ChatUsrBgColor");
	currentColors[3] = config_file.readEntry("Look", "ChatUsrFontColor");
	currentColors[4] = config_file.readEntry("Look", "InfoPanelBgColor");
	currentColors[5] = config_file.readEntry("Look", "InfoPanelFgColor");
	currentColors[6] = config_file.readEntry("Look", "UserboxBgColor");
	currentColors[7] = config_file.readEntry("Look", "UserboxFgColor");

	QGroupBox *grp_iconsOptions = new QGroupBox(tr("Icons"), colorsPage);
	grp_iconsOptions->setInsideMargin(10);
	grp_iconsOptions->setColumns(2);
	grp_iconsOptions->setInsideSpacing(4);

	new QLabel(tr("Please choose icon theme"), grp_iconsOptions);
	cb_iconTheme = new QComboBox(grp_iconsOptions);
	cb_iconTheme->insertStringList(icons_manager->themes());
	cb_iconTheme->setCurrentText(config_file.readEntry("Look", "IconTheme"));
	if (icons_manager->themes().contains("default"))
	cb_iconTheme->changeItem(tr("Default"), icons_manager->themes().findIndex("default"));

	QHBox *grp_icons = new QHBox (grp_iconsOptions);
	iconPreview = new QLabel(grp_icons);
	iconPreview2 = new QLabel(grp_icons);
	iconPreview3 = new QLabel(grp_icons);
	iconPreview4 = new QLabel(grp_icons);

	iconPreview->setPixmap(icons_manager->loadIcon("Online"));
	iconPreview2->setPixmap(icons_manager->loadIcon("BusyWithDescription"));
	iconPreview3->setPixmap(icons_manager->loadIcon("InvisibleWithMobile"));
	iconPreview4->setPixmap(icons_manager->loadIcon("OfflineWithDescriptionMobile"));

	connect(cb_colorTheme, SIGNAL(activated (int)), this, SLOT(previewColorTheme(int)));
	connect(cb_iconTheme, SIGNAL(activated (int)), this, SLOT(previewIconTheme(int)));

	addPage(colorsPage, tr("Colors and icons"));
	kdebugf2();
}

/**
	panel informacyjny
**/
void Wizard::createInfoPanelPage()
{
	kdebugf();
	infoPanelPage = new QVBox(this);
	infoPanelPage->setSpacing(8);

	new QLabel(tr("<h3>Choose your information panel look</h3>"), infoPanelPage);

	QGroupBox *grp_infoPanelOptions = new QGroupBox(tr("Information panel theme"), infoPanelPage);
	grp_infoPanelOptions->setInsideMargin(10);
	grp_infoPanelOptions->setColumns(2);
	grp_infoPanelOptions->setInsideSpacing(4);

	QVBox *grp_checks = new QVBox(grp_infoPanelOptions);
	c_showInfoPanel = new QCheckBox (tr("Show information panel"), grp_checks);
	c_showInfoPanel->setChecked(config_file.readBoolEntry("Look", "ShowInfoPanel", true));
	c_showScrolls = new QCheckBox (tr("Show vertical scrollbar"), grp_checks);
	c_showScrolls->setChecked(config_file.readBoolEntry("Look", "PanelVerticalScrollbar", true));

	cb_panelTheme = new QComboBox(grp_infoPanelOptions);

	for (unsigned int i = 0; i < informationPanelCount; ++i)
		cb_panelTheme->insertItem(tr(informationPanelName[i]));

	new QLabel(tr("Preview"), grp_infoPanelOptions);

	infoPreview = new KaduTextBrowser (grp_infoPanelOptions);	//-- przymiarka pod zmiane podgladu
	infoPreview->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
	infoPreview->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
	infoPreview->setFrameStyle(QFrame::Box | QFrame::Plain);
	infoPreview->setLineWidth(1);
	infoPreview->setAlignment(Qt::AlignVCenter | Qt::WordBreak | Qt::DontClip);
	infoPreview->setMaximumWidth(240);

	if (c_showScrolls->isChecked())
		infoPreview->setVScrollBarMode(QScrollView::AlwaysOn); //zeby bylo je widac nawet przy krotkich panelach
	else
		infoPreview->setVScrollBarMode(QScrollView::AlwaysOff);

	connect(cb_panelTheme, SIGNAL(activated (int)), this, SLOT(previewPanelTheme(int)));
	connect(c_showScrolls, SIGNAL(toggled(bool)), this, SLOT(addScrolls(bool)));	//--j.w.

	QString panelConstruction = config_file.readEntry("Look", "PanelContents");
	if (!panelConstruction.isEmpty())
	{
		UserListElement el;
		unsigned int i;
		for (i = 0; i < informationPanelCount; ++i)
			if (panelConstruction==toSave(informationPanelSyntax[i]))
			{
				cb_panelTheme->setCurrentItem(i);
				infoPreview->setText(KaduParser::parse(toDisplay(informationPanelSyntax[i]), el));
				break;
			}
		if (i == informationPanelCount)
		{
			cb_panelTheme->insertItem(tr("Custom"));
			cb_panelTheme->setCurrentItem(i);
			customPanel=panelConstruction;
			infoPreview->setText(KaduParser::parse(toDisplay(panelConstruction), el));
		}
	}
	addPage(infoPanelPage, tr("Information panel look"));
	kdebugf2();
}

/**
	wybór stylu qt
**/
void Wizard::createQtStylePage()
{
	kdebugf();
	qtStylePage = new QVBox(this);
	qtStylePage->setSpacing(8);

	new QLabel(tr("<h3>Choose Qt theme for Kadu</h3>"), qtStylePage);

	QGroupBox *grp_qtOptions = new QGroupBox(tr("Qt theme"), qtStylePage);
	grp_qtOptions->setInsideMargin(10);
	grp_qtOptions->setColumns(2);
	grp_qtOptions->setInsideSpacing(4);

	new QLabel(tr("Please choose Qt design for Kadu"), grp_qtOptions);
	cb_qtTheme = new QComboBox(grp_qtOptions);

	QStringList sl_themes = QStyleFactory::keys();
	cb_qtTheme->insertStringList(sl_themes);
	if(!sl_themes.contains(QApplication::style().name()))
		cb_qtTheme->setCurrentText(tr("Unknown"));
	else
		cb_qtTheme->setCurrentText(QApplication::style().name());

	connect(cb_qtTheme, SIGNAL(activated(int)), this, SLOT(previewQtTheme(int)));

	addPage(qtStylePage, tr("Qt Look"));
	kdebugf2();
}

/**
	po¿egnanie
**/
void Wizard::createGreetingsPage()
{
	kdebugf();
	greetingsPage = new QVBox(this);
	greetingsPage->setSpacing(8);

	new QLabel(tr("<h2>Congratulations</h2><h3>You have just configured Kadu</h3>"
					"You can adjust settings using the configuration icon on toolbar "
					"or simply by pressing F2. It's recomended to read the documentation "
					"before changing some advanced settings. If you have questions "
					"or problems with Kadu look at <b>www.kadu.net/forum</b> and be "
					"our guest.<br><h3>Enjoy using Kadu ;)<br>Kadu Team</h3>"), greetingsPage);

	addPage(greetingsPage, tr("Congratulations"));
	setFinishEnabled(greetingsPage, TRUE);
	kdebugf2();
}

#include <stdlib.h>
/**
	ustawia jêzyk Kadu
**/
void Wizard::setLanguage(int languageId)	/* ustawia jêzyk */
{
	kdebugf();
	QString language;
	switch (languageId)
	{
		case 1:	language = "pl";	break;
		case 2:	language = "it";	break;
		case 3:	language = "de";	break;
		case 4:	language = "fr";	break;
		default: language = "en";
	}
	if (config_file.readEntry("General", "Language") != language)
	{
		config_file.writeEntry("General", "Language", language);
		config_file.sync();
		(new QProcess(dataPath() + "../bin/kadu"))->start();
		qApp->processEvents();
		qApp->exit(0);
	}
	kdebugf2();
}

/**
	ustawienia chata
**/
void Wizard::setChatOptions()
{
	kdebugf();
	config_file.writeEntry("Chat", "MessageAcks", c_waitForDelivery->isChecked());
	config_file.writeEntry("Chat", "AutoSend", c_enterSendsMessage->isChecked());
	config_file.writeEntry("Chat", "OpenChatOnMessage", c_openOnNewMessage->isChecked());
	config_file.writeEntry("Chat", "BlinkChatTitle", c_flashTitleOnNewMessage->isChecked());
	config_file.writeEntry("Chat", "IgnoreAnonymousUsers", c_ignoreAnonyms->isChecked());
	kdebugf2();
}

/**
	ustawienia d¼wiêku
**/
void Wizard::setSoundOptions()
{
	kdebugf();
	config_file.writeEntry("Sounds", "PlaySound", c_enableSounds->isChecked());
	config_file.writeEntry("Sounds", "PlaySoundChat", c_playWhilstChatting->isChecked());
	config_file.writeEntry("Sounds", "PlaySoundChatInvisible", c_playWhenInvisible->isChecked());

	if (modules_manager->moduleIsLoaded("alsa_sound") && (cb_soundModule->currentText() != "alsa_sound")) modules_manager->deactivateModule("alsa_sound", false);
	else if (modules_manager->moduleIsLoaded("ao_sound") && (cb_soundModule->currentText() != "ao_sound")) modules_manager->deactivateModule("ao_sound", false);
	else if (modules_manager->moduleIsLoaded("arts_sound") && (cb_soundModule->currentText() != "arts_sound")) modules_manager->deactivateModule("arts_sound", false);
	else if (modules_manager->moduleIsLoaded("dsp_sound") && (cb_soundModule->currentText() != "dsp_sound")) modules_manager->deactivateModule("dsp_sound", false);
	else if (modules_manager->moduleIsLoaded("esd_sound") && (cb_soundModule->currentText() != "esd_sound")) modules_manager->deactivateModule("esd_sound", false);
	else if (modules_manager->moduleIsLoaded("ext_sound") && (cb_soundModule->currentText() != "ext_sound")) modules_manager->deactivateModule("ext_sound", false);
	else if (modules_manager->moduleIsLoaded("nas_sound") && (cb_soundModule->currentText() != "nas_sound")) modules_manager->deactivateModule("nas_sound", false);

	//jak wybrany modul != zaden to probuje zaladowac
	if ((cb_soundModule->currentText() != tr("None")) && (!modules_manager->moduleIsLoaded(cb_soundModule->currentText())))
		modules_manager->activateModule(cb_soundModule->currentText());

	kdebugf2();
}

/**
	ustawienia ogólne
**/
void Wizard::setGeneralOptions()
{
	kdebugf();
	config_file.writeEntry("General", "PrivateStatus", c_privateStatus->isChecked());
	config_file.writeEntry("General", "ShowBlocked", c_showBlocked->isChecked());
	config_file.writeEntry("General", "ShowBlocking", c_showBlocking->isChecked());
	config_file.writeEntry("General", "RunDocked", c_startDocked->isChecked());
	config_file.writeEntry("History", "Logging", !c_logMessages->isChecked());
	config_file.writeEntry("History", "DontSaveStatusChanges", c_logStatusChanges->isChecked());
	kdebugf2();
}

/**
	zapis przegl±darki
**/
void Wizard::setBrowser()
{
	kdebugf();
	config_file.writeEntry("Chat", "WebBrowser", l_customBrowser->text());
	config_file.writeEntry("Chat", "WebBrowserNo", cb_browser->currentItem());
	kdebugf2();
}

/**
	zmiana warto¶ci w combo z wyborem przegl±darki - szuka jej
**/
void Wizard::findAndSetWebBrowser(int selectedBrowser)
{
	ChatManagerSlots::findBrowser(selectedBrowser, cb_browser, cb_browserOptions, l_customBrowser);
	l_customBrowser->setEnabled(!selectedBrowser);
}

/**
	wybiera opcje dla przegl±darki
**/
void Wizard::findAndSetBrowserOption(int selectedOption)
{
	ChatManagerSlots::setBrowserOption(selectedOption, l_customBrowser, cb_browser->currentItem());
}

/**
	podgl±d szaty graficznej dymków
**/
void Wizard::previewHintsTheme(int hintsThemeID)
{
	if (cb_hintsTheme->currentText() == tr("Current"))
	{
		preview->setPaletteForegroundColor(QColor(currentHints[2][1]));
		preview->setPaletteBackgroundColor(QColor(currentHints[2][0]));
		preview2->setPaletteForegroundColor(QColor(currentHints[3][1]));
		preview2->setPaletteBackgroundColor(QColor(currentHints[3][0]));
		preview4->setPaletteForegroundColor(QColor(currentHints[2][1]));
		preview4->setPaletteBackgroundColor(QColor(currentHints[2][0]));
	}
	else
	{
		preview->setPaletteForegroundColor(QColor(hintColors[hintsThemeID][1]));
		preview->setPaletteBackgroundColor(QColor(hintColors[hintsThemeID][0]));
		preview2->setPaletteForegroundColor(QColor(hintColors[hintsThemeID][1]));
		preview2->setPaletteBackgroundColor(QColor(hintColors[hintsThemeID][0]));
		preview4->setPaletteForegroundColor(QColor(hintColors[hintsThemeID][1]));
		preview4->setPaletteBackgroundColor(QColor(hintColors[hintsThemeID][0]));
	}
}

/**
	podgl±d typu dymków
**/
void Wizard::previewHintsType(int hintsTypeID)
{
	if (hintsTypeID == int(hintCount))
		preview4->setText(toDisplay(customHint));
	else
		preview4->setText(toDisplay(hintSyntax[hintsTypeID]));
}

/**
	zapisuje ustawienia dymków
**/
void Wizard::setHints()
{
	kdebugf();

	if (cb_hintsTheme->currentText() == tr("Current"))
	{
		config_file.writeEntry("Hints", "HintBlocking_bgcolor", currentHints[0][0]);
		config_file.writeEntry("Hints", "HintBusyD_bgcolor", currentHints[1][0]);
		config_file.writeEntry("Hints", "HintBusy_bgcolor", currentHints[2][0]);
		config_file.writeEntry("Hints", "HintError_bgcolor", currentHints[3][0]);
		config_file.writeEntry("Hints", "HintInvisibleD_bgcolor", currentHints[4][0]);
		config_file.writeEntry("Hints", "HintInvisible_bgcolor", currentHints[5][0]);
		config_file.writeEntry("Hints", "HintMessage_bgcolor", currentHints[6][0]);
		config_file.writeEntry("Hints", "HintNewChat_bgcolor", currentHints[7][0]);
		config_file.writeEntry("Hints", "HintNewMessage_bgcolor", currentHints[8][0]);
		config_file.writeEntry("Hints", "HintOfflineD_bgcolor", currentHints[9][0]);
		config_file.writeEntry("Hints", "HintOffline_bgcolor", currentHints[10][0]);
		config_file.writeEntry("Hints", "HintOnlineD_bgcolor", currentHints[11][0]);
		config_file.writeEntry("Hints", "HintOnline_bgcolor", currentHints[12][0]);

		config_file.writeEntry("Hints", "HintBlocking_fgcolor", currentHints[0][1]);
		config_file.writeEntry("Hints", "HintBusyD_fgcolor", currentHints[1][1]);
		config_file.writeEntry("Hints", "HintBusy_fgcolor", currentHints[2][1]);
		config_file.writeEntry("Hints", "HintError_fgcolor", currentHints[3][1]);
		config_file.writeEntry("Hints", "HintInvisibleD_fgcolor", currentHints[4][1]);
		config_file.writeEntry("Hints", "HintInvisible_fgcolor", currentHints[5][1]);
		config_file.writeEntry("Hints", "HintMessage_fgcolor", currentHints[6][1]);
		config_file.writeEntry("Hints", "HintNewChat_fgcolor", currentHints[7][1]);
		config_file.writeEntry("Hints", "HintNewMessage_fgcolor", currentHints[8][1]);
		config_file.writeEntry("Hints", "HintOfflineD_fgcolor", currentHints[9][1]);
		config_file.writeEntry("Hints", "HintOffline_fgcolor", currentHints[10][1]);
		config_file.writeEntry("Hints", "HintOnlineD_fgcolor", currentHints[11][1]);
		config_file.writeEntry("Hints", "HintOnline_fgcolor", currentHints[12][1]);
	}
	else
	{
		QColor bg_color, fg_color;
		bg_color = preview->paletteBackgroundColor();
		fg_color = preview->paletteForegroundColor();

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
	}
	config_file.writeEntry("Hints", "NotifyHintUseSyntax", true);

	if (cb_hintsType->currentItem() == 0)
	{
		config_file.writeEntry("Hints", "NotifyHintUseSyntax", false);
		config_file.writeEntry("Hints", "NotifyHintSyntax", QString::null);
	}
	else
	{
		if (cb_hintsType->currentItem() == int(hintCount))
			config_file.writeEntry("Hints", "NotifyHintSyntax", tr(customHint.ascii()));
		else
			config_file.writeEntry("Hints", "NotifyHintSyntax", tr(hintSyntax[cb_hintsType->currentItem()]));
	}
	kdebugf2();
}

/**
	podgl±d kolorków i ich zapis
**/
void Wizard::previewColorTheme(int colorThemeID)
{
	kdebugf();

	if (cb_colorTheme->currentText() == tr("Current"))
	{
		config_file.writeEntry("Look", "ChatMyBgColor", currentColors[0]);
		config_file.writeEntry("Look", "ChatMyFontColor", currentColors[1]);
		config_file.writeEntry("Look", "ChatUsrBgColor", currentColors[2]);
		config_file.writeEntry("Look", "ChatUsrFontColor", currentColors[3]);
		config_file.writeEntry("Look", "InfoPanelBgColor", currentColors[4]);
		config_file.writeEntry("Look", "InfoPanelFgColor", currentColors[5]);
		config_file.writeEntry("Look", "UserboxBgColor", currentColors[6]);
		config_file.writeEntry("Look", "UserboxFgColor", currentColors[7]);
	}
	else
	{
		config_file.writeEntry("Look", "ChatMyBgColor", kaduColors[colorThemeID][0]);
		config_file.writeEntry("Look", "ChatMyFontColor", kaduColors[colorThemeID][1]);
		config_file.writeEntry("Look", "ChatUsrBgColor", kaduColors[colorThemeID][2]);
		config_file.writeEntry("Look", "ChatUsrFontColor", kaduColors[colorThemeID][3]);
		config_file.writeEntry("Look", "InfoPanelBgColor", kaduColors[colorThemeID][4]);
		config_file.writeEntry("Look", "InfoPanelFgColor", kaduColors[colorThemeID][5]);
		config_file.writeEntry("Look", "UserboxBgColor", kaduColors[colorThemeID][6]);
		config_file.writeEntry("Look", "UserboxFgColor", kaduColors[colorThemeID][7]);
	}
	kadu->changeAppearance();
	kdebugf2();
}

/**
	podgl±d ikonek
**/
void Wizard::previewIconTheme(int iconThemeID)
{
	QString iconName = cb_iconTheme->currentText();
	if (iconName == tr("Default"))
		iconName = "default";

	icons_manager->clear();
	icons_manager->setTheme(iconName);

	QString path = icons_manager->iconPath("Online");
	for (int i = 0, count = cb_iconTheme->count(); i < count; ++i)
		if (i != iconThemeID)
			path.replace(cb_iconTheme->text(i), cb_iconTheme->text(iconThemeID));
	path.replace(tr("Default"), "default");
	iconPreview->setPixmap(path);
	path.remove("online.png");
	iconPreview2->setPixmap(path+"busy_d.png");
	iconPreview3->setPixmap(path+"invisible_m.png");
	iconPreview4->setPixmap(path+"offline_d_m.png");
}

/**
	zapisuje ustawienie ikonek (i kolorków)
**/
void Wizard::setColorsAndIcons()
{
	kdebugf();
	QString newIconTheme = cb_iconTheme->currentText();
	QString oldIconTheme = config_file.readEntry("Look", "IconTheme", "default");
	if (newIconTheme != oldIconTheme)
	{
		newIconTheme.replace(tr("Default"), "default");
		icons_manager->clear();
		icons_manager->setTheme(newIconTheme);
		KaduActions.refreshIcons();
		UserBox::userboxmenu->refreshIcons();
		icons_manager->refreshMenus();
		kadu->changeAppearance();
	}
	else
		newIconTheme.replace(tr("Default"), "default");

	config_file.writeEntry("Look", "IconTheme", newIconTheme);
	kdebugf2();
}

/**
	podgl±d panelu
**/
void Wizard::previewPanelTheme(int panelThemeID)
{
	kdebugf();
	QString panelLook;
	UserListElement el;

	if (panelThemeID == int(informationPanelCount))
		panelLook = customPanel;
	else
		panelLook = informationPanelSyntax[cb_panelTheme->currentItem()];

	if (panelLook.contains("background=", false) == 0)	//to nam zapewnia odswie¿enie tla jesli wczesniej byl obrazek
		infoPreview->setText("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor")+"\"></body>");

	infoPreview->setText(KaduParser::parse(toDisplay(panelLook), el));
	kdebugf2();
}

/**
	zapisanie wygl±du panelu
**/
void Wizard::setPanelTheme()
{
	kdebugf();
	UserListElement el;
	config_file.writeEntry("Look", "ShowInfoPanel", c_showInfoPanel->isChecked());
	kadu->showdesc(config_file.readBoolEntry("Look", "ShowInfoPanel"));
	config_file.writeEntry("Look", "PanelVerticalScrollbar", c_showScrolls->isChecked());
	if (cb_panelTheme->currentItem() == int(informationPanelCount))
		config_file.writeEntry("Look", "PanelContents", customPanel);
	else
		config_file.writeEntry("Look", "PanelContents", toSave(informationPanelSyntax[cb_panelTheme->currentItem()]));
	kdebugf2();
}


/**
	podgl±d stylu i jego zapis
**/
void Wizard::previewQtTheme(int themeID)
{
	QString new_style = cb_qtTheme->text(themeID);
	if (new_style != tr("Unknown") && new_style != QApplication::style().name())
	{
		QApplication::setStyle(new_style);
		config_file.writeEntry("Look", "QtStyle", new_style);
	}
}

/**
	wykorzystywane przy wy¶wietelaniu podgl±du panelu inf. i dymków
**/
QString Wizard::toDisplay(QString s)
{
	kdebugf();
	s = toSave(s);		//wpierw zamieniam sciezki, potem ustawiam przykladowe dane
	s.replace("%t.png", "Busy.png");	//to obrazek do RonK2 - nie tlumaczymy
	s.replace("%s", tr("Busy"));
	s.replace("%d", tr("My description"));
	s.replace("%i", "192.168.0.1");
	s.replace("%n", "Jimbo");
	s.replace("%a", "jimbo");
	s.replace("%f", tr("Mark"));
	s.replace("%r", tr("Smith"));
	s.replace("%m", "+4812345679");
	s.replace("%u", "999999");
	s.replace("%g", tr("Friends"));
	s.remove("%o");
	s.replace("%v", "host.server.net");
	s.replace("%p", "80");
	s.replace("%e", "jimbo@mail.server.net");
	s.remove("[");
	s.remove("]");
	s.replace("changed status to", tr("changed status to"));

	kdebugf2();
	return s;
}

/**
	przy zapisie panelu - zamienia ¶cie¿kê na odpowiedni± - pomocne gdy siê nie ma kadu w /usr
**/
QString Wizard::toSave(QString s)
{
	s.replace("You are not on the list", tr("You are not on the list"));

	int i;
	for (i = 0; i < s.contains("$KADU_SHARE"); ++i)
		s.replace("$KADU_SHARE", dataPath("kadu"));

	for (i = 0; i < s.contains("$KADU_CONF"); ++i)
		s.replace("$KADU_CONF", ggPath());

	for (i = 0; i < s.contains("$HOME"); ++i)
		s.replace("$HOME", getenv("HOME"));

	return s;
}

/**
	wybor modulu dzwiekowego - wypisanie infa na temat modulu + ew. sprawdzenie czy modul jest dostepny
**/
void Wizard::setSoundModule(int comboPos)
{
	QString moduleName = cb_soundModule->text(comboPos);
	if (moduleName == "alsa_sound")
		moduleInfo->setText(tr("This module play sounds using ALSA - Advanced Linux Sound Architecture driver."));
	else if (moduleName == "arts_sound")
		moduleInfo->setText(tr("This module uses KDE's aRts daemon which mixes multiple "
							"digital sources at real time.<br> When your soundcard "
							"doesn't handle that, this module is for you."));
	else if (moduleName == "dsp_sound")
		moduleInfo->setText(tr("This module uses OSS kernel module to produce sounds."));
	else if (moduleName == "esd_sound")
		moduleInfo->setText(tr("This module uses GNOME's Enlightened Sound Daemon "
							"which mixes multiple digital sources at real time.<br> "
							"When your soundcard doesn't handle that, this module is for you."));
	else if (moduleName == "nas_sound")
		moduleInfo->setText(tr("This module uses Network Audio System to produce "
							"sounds. Use it when you use NAS sound system."));
	else if (moduleName == "ao_sound")
		moduleInfo->setText(tr("This module uses libao to produce sounds. "
							"This is third-party module not supported by Kadu Team."));
	else if (moduleName == "ext_sound")
		moduleInfo->setText(tr("This module uses external application to produce "
							"sounds. You must specify path to external program "
							"in configuration."));
	else
		moduleInfo->setText(tr("There will be no sounds in Kadu"));
	if ((moduleName != tr("None")) && (!modules_manager->moduleIsInstalled(moduleName)))
	{
		moduleInfo->setText(tr("<font color=red>Module is not installed! "
							"If you want to use it download and install "
							"it first.</font><br>")+moduleInfo->text());
		setNextEnabled(soundOptionsPage, false);
	}
	else
		setNextEnabled(soundOptionsPage, true);
}

/**
	wlaczenie/wylaczenie Scroolli w podgladzie panelu informacyjnego
**/
void Wizard::addScrolls(bool enableScrolls)
{
	kdebugf();
	if (enableScrolls)
		infoPreview->setVScrollBarMode(QScrollView::AlwaysOn);	//zeby bylo widac nawet przy krotkich panelach
	else
		infoPreview->setVScrollBarMode(QScrollView::AlwaysOff);
	kdebugf2();
}

Wizard *startWizardObj = NULL;
WizardStarter *wizardStarter = NULL;

/** @} */

