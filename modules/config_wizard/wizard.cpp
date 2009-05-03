/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include <stdlib.h>

#include "../sound/sound.h"

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "icons-manager.h"
#include "kadu.h"
#include "main_configuration_window.h"
#include "modules.h"
#include "wizard.h"

/**
 * @ingroup config_wizard
 * @{
 */

extern "C" KADU_EXPORT int config_wizard_init(bool firstLoad)
{
	kdebugf();
	wizardStarter = new WizardStarter();

	// TODO: 0.6.6
	if (config_file.readNumEntry("General", "UIN", 0) == 0 || config_file.readEntry("General", "Password").isEmpty())
		wizardStarter->start(0, false);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void config_wizard_close()
{
	kdebugf();
	if (wizardStarter)
	{
		delete wizardStarter;
		wizardStarter = 0;
	}
	kdebugf2();
}

WizardStarter::WizardStarter(QObject *parent)
	: QObject(parent)
{
	configWizardActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "configWizardAction",
		this, SLOT(start(QAction *, bool)),
		"ConfigurationWizard", tr("Configuration Wizard")
	);
	kadu->insertMenuActionDescription(0, configWizardActionDescription);
}

WizardStarter::~WizardStarter()
{
	kadu->removeMenuActionDescription(configWizardActionDescription);
	delete configWizardActionDescription;

	if (startWizardObj)
	{
		delete startWizardObj;
		startWizardObj = 0;
	}
}

void WizardStarter::start(QAction *sender, bool toggled)
{
	kdebugf();
	if (!startWizardObj)
	{
		startWizardObj = new Wizard();
		startWizardObj->wizardStart();
	}
	kdebugf2();
}

Wizard::Wizard(QWidget *parent)
	: QWizard(parent), registeringAccount(false), testingSound(false)
{
	kdebugf();
	setWindowTitle(tr("Kadu Wizard"));
#ifdef Q_OS_MAC
	/* MacOSX has it's own QWizard style which requires much more space
	 * than the other ones so we're forcing the ClassicStyle to unify
	 * the window sizes and look. Mac users will love us for that.
	 */
	setWizardStyle(QWizard::ClassicStyle);
#else
	setMinimumSize(710, 300);
#endif
	createGGAccountPage();
	createApplicationsPage();
	createSoundPage();

	connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
	connect(this, SIGNAL(rejected()), this, SLOT(rejectedSlot()));

	kdebugf2();
}

Wizard::~Wizard()
{
	kdebugf();
	kdebugf2();
}

bool Wizard::validateCurrentPage()
{
	return !(registeringAccount || testingSound);
}

/**
	naci�ni�cie zako�cz i zapisanie konfiguracji (o ile nie nast�pi�o wcze�niej)
**/
void Wizard::acceptedSlot()
{
	saveGGAccountOptions();
	saveApplicationsOptions();
	saveSoundOptions();

	startWizardObj = 0;
	deleteLater();
}

void Wizard::rejectedSlot()
{
	changeSoundModule(backupSoundModule);

	startWizardObj = 0;
	deleteLater();
}

void Wizard::closeEvent(QCloseEvent *e)
{
	QDialog::closeEvent(e);

	startWizardObj = 0;
	deleteLater();
}

/**
	wywo�anie wizarda z menu
**/
void Wizard::wizardStart()
{
	show();
}

/**
	po zaimportowaniu listy kontakt�w si� wywo�uje
**/
void WizardStarter::userListImported(bool ok, QList<UserListElement> list)
{
	kdebugf();

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	disconnect(gadu, SIGNAL(userListImported(bool, QList<UserListElement>)), this, SLOT(userListImported(bool, QList<UserListElement>)));

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
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	if (!gadu->doImportUserList())
	{
		MessageBox::msg(tr("User list couldn't be imported"));
		disconnect(gadu, SIGNAL(userListImported(bool, QList<UserListElement>)), this, SLOT(userListImported(bool, QList<UserListElement>)));
	}

	disconnect(gadu, SIGNAL(connected()), this, SLOT(connected()));
}

/**
	Ustawienie konta GG
**/
void Wizard::registerGGAccount()
{
	kdebugf();

	if (ggNewPassword->text() != ggReNewPassword->text())
	{
		MessageBox::msg(tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"New password\" and \"Retype new password\") "
			"should be the same!"));
		return;
	}

	if (ggNewPassword->text().isEmpty() || ggEMail->text().isEmpty())
	{
		MessageBox::msg(tr("Please fill out all fields"), false, "Warning");
		return;
	}

	if (ggEMail->text().find(HtmlDocument::mailRegExp()) == -1)
	{
		MessageBox::msg(tr("Email address you have entered is not valid"), false, "Warning");
		return;
	}

	registeringAccount = true;
	foreach(QWidget *widget, dontHaveNumberWidgets)
		widget->setEnabled(false);

	haveNumber->setEnabled(false);

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	connect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredGGAccount(bool, UinType)));
	gadu->registerAccount(ggEMail->text(), ggNewPassword->text());

	kdebugf2();
}

/**
	Zapisanie parametr�w nowego konta
**/
void Wizard::registeredGGAccount(bool ok, UinType uin)
{
	kdebugf();

	if (ok)
	{
		// TODO: 0.6.6
		config_file.writeEntry("General", "UIN", (int)uin);
		config_file.writeEntry("General", "Password", pwHash(ggNewPassword->text()));

		Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
		// gadu->changeID(QString::number(uin));
		kadu->setOnline();	//jak zarejestrowal to od razu sie laczy

		MessageBox::msg(tr("Registration was successful.\nYou UIN is: ") + QString::number(int(uin))+tr("\nAccount configuration was saved.\nPress Ok to continue"));

		haveNumber->setChecked(true);
		ggNumber->setText(QString::number((int)uin));
		ggPassword->setText(ggNewPassword->text());
	}
	else
	{
		MessageBox::msg(tr("An error has occured while registration. Please try again later."), false, "Warning");

		foreach(QWidget *widget, dontHaveNumberWidgets)
			widget->setEnabled(true);
	}

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	disconnect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredGGAccount(bool, UinType)));

	haveNumber->setEnabled(true);
	registeringAccount = false;

	kdebugf2();
}


/**
	sprawdzenie czy trzeba importowa� list� kontakt�w czy nie i ew. import
**/

void Wizard::tryImport()
{
	if (!ggImportContacts->isChecked())
		return;

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	connect(gadu, SIGNAL(userListImported(bool, QList<UserListElement>)),
			wizardStarter, SLOT(userListImported(bool, QList<UserListElement>)));

	if (gadu->currentStatus().isOffline())
	{
		connect(gadu, SIGNAL(connected()), wizardStarter, SLOT(connected()));
		kadu->setOnline(); //kaze sie polaczyc i podpina sie pod sygnal polaczenia sie z siecia
	} //jak polaczony to bez cyrkow robi import
	else if (!gadu->doImportUserList())
	{
		MessageBox::msg(tr("User list wasn't imported because of some error"));
		disconnect(gadu, SIGNAL(userListImported(bool, QList<UserListElement>)), wizardStarter, SLOT(userListImported(bool, QList<UserListElement>)));
	}
}

void Wizard::createGGAccountPage()
{
	kdebugf();

	QWizardPage *ggPage = new QWizardPage(this);
	ggPage->setTitle(tr("Gadu-gadu account"));

	QGridLayout *gridLayout = new QGridLayout(ggPage);
	gridLayout->setSpacing(5);

	gridLayout->setColumnStretch(0, 7);
	gridLayout->setColumnStretch(1, 1);
	gridLayout->setColumnStretch(2, 7);
	gridLayout->setColumnStretch(3, 7);

	QTextBrowser *descriptionPane = new QTextBrowser(ggPage);
	descriptionPane->setText(tr("<h3>Welcome in Kadu</h3><h4>the Gadu-gadu network client for *nix and MacOS X.</h4>"
		"<p>This is first time you launch Kadu. "
		"This wizard will help you to configure the basic settings of Kadu. "
		"If you are experienced Kadu user you may omit the wizard by clicking Cancel.</p>"
		"<p>Please enter your account data. If you don't have one, you can create new here.</p>"
		"<p>E-mail address is needed when you want to recover lost password to account</p>"));

	descriptionPane->setFixedWidth(200);
	gridLayout->addMultiCellWidget(descriptionPane, 0, 8, 0, 0);

	haveNumber = new QRadioButton(tr("I have a number"), ggPage);
	connect(haveNumber, SIGNAL(toggled(bool)), this, SLOT(haveNumberChanged(bool)));
	gridLayout->addMultiCellWidget(haveNumber, 0, 0, 2, 3);

	QLabel *ggNumberLabel = new QLabel(tr("Gadu-gadu number") + ":", ggPage);
	gridLayout->addWidget(ggNumberLabel, 1, 2, Qt::AlignRight);
	ggNumber = new QLineEdit(ggPage);
	gridLayout->addWidget(ggNumber, 1, 3);

	QLabel *ggPasswordLabel = new QLabel(tr("Gadu-gadu password") + ":", ggPage);
	gridLayout->addWidget(ggPasswordLabel, 2, 2, Qt::AlignRight);
	ggPassword = new QLineEdit(ggPage);
	ggPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggPassword, 2, 3);

	ggImportContacts = new QCheckBox(tr("Import contacts"), ggPage);
	ggImportContacts->setChecked(true);
	gridLayout->addMultiCellWidget(ggImportContacts, 3, 3, 2, 3);

	dontHaveNumber = new QRadioButton(tr("I don't have a number"), ggPage);
	gridLayout->addMultiCellWidget(dontHaveNumber, 4, 4, 2, 3);

	QLabel *ggNewPasswordLabel = new QLabel(tr("New password") + ":", ggPage);
	gridLayout->addWidget(ggNewPasswordLabel, 5, 2, Qt::AlignRight);
	ggNewPassword = new QLineEdit(ggPage);
	ggNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggNewPassword, 5, 3);

	QLabel *ggReNewPasswordLabel = new QLabel(tr("Retype password") + ":", ggPage);
	gridLayout->addWidget(ggReNewPasswordLabel, 6, 2, Qt::AlignRight);
	ggReNewPassword = new QLineEdit(ggPage);
	ggReNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggReNewPassword, 6, 3);

	QLabel *ggEMailLabel = new QLabel(tr("Your e-mail address") + ":", ggPage);
	gridLayout->addWidget(ggEMailLabel, 7, 2, Qt::AlignRight);
	ggEMail = new QLineEdit(ggPage);
	gridLayout->addWidget(ggEMail, 7, 3);

	ggRegisterAccount = new QPushButton(tr("Register"), ggPage);
	connect(ggRegisterAccount, SIGNAL(clicked()), this, SLOT(registerGGAccount()));
	gridLayout->addMultiCellWidget(ggRegisterAccount, 8, 8, 2, 3);

	QButtonGroup *haveNumberGroup = new QButtonGroup();
	haveNumberGroup->insert(haveNumber);
	haveNumberGroup->insert(dontHaveNumber);

	ggPage->setLayout(gridLayout);
 
	haveNumberWidgets.append(ggNumberLabel);
	haveNumberWidgets.append(ggNumber);
	haveNumberWidgets.append(ggPasswordLabel);
	haveNumberWidgets.append(ggPassword);
	haveNumberWidgets.append(ggImportContacts);
	dontHaveNumberWidgets.append(ggNewPasswordLabel);
	dontHaveNumberWidgets.append(ggNewPassword);
	dontHaveNumberWidgets.append(ggReNewPasswordLabel);
	dontHaveNumberWidgets.append(ggReNewPassword);
	dontHaveNumberWidgets.append(ggEMailLabel);
	dontHaveNumberWidgets.append(ggEMail);
	dontHaveNumberWidgets.append(ggRegisterAccount);

	loadGGAccountOptions();

	addPage(ggPage);

	kdebugf2();
}

void Wizard::haveNumberChanged(bool haveNumber)
{
	foreach(QWidget *widget, haveNumberWidgets)
		widget->setEnabled(haveNumber);
	foreach(QWidget *widget, dontHaveNumberWidgets)
		widget->setEnabled(!haveNumber);
}

void Wizard::loadGGAccountOptions()
{
	// TODO: 0.6.6
	QString uin = config_file.readEntry("General", "UIN");

	haveNumber->setChecked(true);
	haveNumberChanged(true);

	if (!uin.isEmpty())
	{		
		ggNumber->setText(uin);
		ggPassword->setText(pwHash(config_file.readEntry("General", "Password")));
	}
}

void Wizard::saveGGAccountOptions()
{
	config_file.writeEntry("General", "UIN", ggNumber->text());
	config_file.writeEntry("General", "Password", pwHash(ggPassword->text()));

	kadu->configurationUpdated();

	if (!ggNumber->text().isEmpty())
		tryImport();
}

/**
	opcje przegladarki www
**/
void Wizard::createApplicationsPage()
{
	kdebugf();

	QWizardPage *applicationsPage = new QWizardPage(this);
	applicationsPage->setTitle(tr("Applications"));

	QGridLayout *gridLayout = new QGridLayout(applicationsPage);
	gridLayout->setSpacing(5);

	gridLayout->setColumnStretch(0, 7);
	gridLayout->setColumnStretch(1, 1);
	gridLayout->setColumnStretch(2, 7);
	gridLayout->setColumnStretch(3, 14);
	gridLayout->setRowStretch(4, 100);

	QTextBrowser *descriptionPane = new QTextBrowser(applicationsPage);
	descriptionPane->setText(tr(
		"<p>Please setup Kadu for working with your favourite WWW browser and email program.</p>"
		"<p>Kadu will use these for opening various links from messages and user's descriptions</p>"));

	descriptionPane->setFixedWidth(200);

	gridLayout->addMultiCellWidget(descriptionPane, 0, 4, 0, 0);

	gridLayout->addWidget(new QLabel(tr("Choose your browser") + ":", applicationsPage), 0, 2, Qt::AlignRight);
	browserCombo = new QComboBox(applicationsPage);
	browserCombo->insertItem(tr("Specify path"));
	browserCombo->insertItem(tr("Konqueror"));
	browserCombo->insertItem(tr("Opera"));
	browserCombo->insertItem(tr("Opera (new tab)"));
	browserCombo->insertItem(tr("SeaMonkey"));
	browserCombo->insertItem(tr("Mozilla"));
	browserCombo->insertItem(tr("Mozilla Firefox"));
	browserCombo->insertItem(tr("Dillo"));
	browserCombo->insertItem(tr("Galeon"));
	browserCombo->insertItem(tr("Safari"));
	browserCombo->insertItem(tr("Camino"));
	connect(browserCombo, SIGNAL(activated(int)), this, SLOT(browserChanged(int)));
	gridLayout->addWidget(browserCombo, 0, 3);

	gridLayout->addWidget(new QLabel(tr("Custom browser") + ":", applicationsPage), 1, 2, Qt::AlignRight);
	browserCommandLineEdit = new QLineEdit(applicationsPage);
	gridLayout->addWidget(browserCommandLineEdit, 1, 3);
 
	gridLayout->addWidget(new QLabel(tr("Choose your e-mail client") + ":", applicationsPage), 2, 2, Qt::AlignRight);
	mailCombo = new QComboBox(applicationsPage);
	mailCombo->insertItem(tr("Specify path"));
	mailCombo->insertItem(tr("KMail"));
	mailCombo->insertItem(tr("Thunderbird"));
	mailCombo->insertItem(tr("SeaMonkey"));
	mailCombo->insertItem(tr("Evolution"));
	mailCombo->insertItem("Mail");
	connect(mailCombo, SIGNAL(activated(int)), this, SLOT(emailChanged(int)));
	gridLayout->addWidget(mailCombo, 2, 3);

	gridLayout->addWidget(new QLabel(tr("Custom e-mail client") + ":", applicationsPage), 3, 2, Qt::AlignRight);
	mailCommandLineEdit = new QLineEdit(applicationsPage);
	gridLayout->addWidget(mailCommandLineEdit, 3, 3);

	applicationsPage->setLayout(gridLayout);

	addPage(applicationsPage);

	loadApplicationsOptions();

	kdebugf2();
}

void Wizard::browserChanged(int index)
{
	QString browser = MainConfigurationWindow::getBrowserExecutable(index);
	browserCommandLineEdit->setEnabled(index == 0);
	browserCommandLineEdit->setText(browser);

	if (index != 0 && browser.isEmpty())
		if (!browserCombo->currentText().contains(tr("Not found")))
			browserCombo->changeItem(browserCombo->currentText() + " (" + tr("Not found") + ")", index);
}

void Wizard::emailChanged(int index)
{
	QString mail = MainConfigurationWindow::getEMailExecutable(index);

	mailCommandLineEdit->setEnabled(index == 0);
	mailCommandLineEdit->setText(mail);

	if (index != 0 && mail.isEmpty())
		if (!mailCombo->currentText().contains(tr("Not found")))
			mailCombo->changeItem(mailCombo->currentText() + " (" + tr("Not found") + ")", index);
}

// don't care for performance here
void Wizard::loadApplicationsOptions()
{
	QString browserIndexName = config_file.readEntry("Chat", "WebBrowserNo");
	QString browserName;

	int browserIndex = 0;
	int foundBrowserIndex = 0;
	while (!(browserName = MainConfigurationWindow::browserIndexToString(browserIndex)).isEmpty())
		if (browserName == browserIndexName)
		{
			foundBrowserIndex = browserIndex;
			break;
		}
		else
			browserIndex++;

	browserCombo->setCurrentItem(foundBrowserIndex);
	browserChanged(foundBrowserIndex);

	QString mailIndexName = config_file.readEntry("Chat", "EmailClientNo");
	QString mailName;

	int mailIndex = 0;
	int foundMailIndex = 0;
	while (!(mailName = MainConfigurationWindow::emailIndexToString(mailIndex)).isEmpty())
		if (mailName == mailIndexName)
		{
			foundMailIndex = mailIndex;
			break;
		}
		else
			mailIndex++;

	mailCombo->setCurrentItem(foundMailIndex);
	emailChanged(foundMailIndex);
}

void Wizard::saveApplicationsOptions()
{
	config_file.writeEntry("Chat", "WebBrowserNo", MainConfigurationWindow::browserIndexToString(browserCombo->currentItem()));
	config_file.writeEntry("Chat", "WebBrowser", browserCommandLineEdit->text());
	config_file.writeEntry("Chat", "EmailClientNo", MainConfigurationWindow::emailIndexToString(mailCombo->currentItem()));
	config_file.writeEntry("Chat", "MailClient", mailCommandLineEdit->text());
}

void Wizard::createSoundPage()
{
	kdebugf();

	QWizardPage *soundPage = new QWizardPage(this);
	soundPage->setTitle(tr("Sound"));

	QGridLayout *gridLayout = new QGridLayout(soundPage);
	gridLayout->setSpacing(5);

	gridLayout->setColumnStretch(0, 7);
	gridLayout->setColumnStretch(1, 1);
	gridLayout->setColumnStretch(2, 7);
	gridLayout->setColumnStretch(3, 7);
	gridLayout->setRowStretch(2, 100);

	QTextBrowser *descriptionPane = new QTextBrowser(soundPage);
	descriptionPane->setText(tr(
		"<p>Please select your sound driver for sound notifications. "
		"If you don't want sound notifications, use None driver.</p>"
		"<p>If you don't know which driver to use, just check every ony with Test sound button."
		"Don't forget to unmute your system before!</p>"));

	descriptionPane->setFixedWidth(200);

	gridLayout->addMultiCellWidget(descriptionPane, 0, 2, 0, 0);

	gridLayout->addWidget(new QLabel(tr("Sound system") + ":", soundPage), 0, 2, Qt::AlignRight);
	soundModuleCombo = new QComboBox(soundPage);
	gridLayout->addWidget(soundModuleCombo, 0, 3);

	soundTest = new QPushButton(tr("Test sound"), soundPage);
	gridLayout->addMultiCellWidget(soundTest, 1, 1, 2, 3);
	connect(soundTest, SIGNAL(clicked()), this, SLOT(testSound()));

	QStringList soundModules;
	ModuleInfo moduleInfo;

	QStringList moduleList = modules_manager->staticModules();
	foreach(const QString &moduleName, moduleList)
		if (modules_manager->moduleInfo(moduleName, moduleInfo))
			if (moduleInfo.provides.contains("sound_driver"))
			{
				soundModules.append(moduleName);
				break;
			}

	if (soundModules.size() == 0)
	{
		moduleList = modules_manager->installedModules();
		foreach(const QString &moduleName, moduleList)
			if (modules_manager->moduleInfo(moduleName, moduleInfo) && moduleInfo.provides.contains("sound_driver"))
				soundModules.append(moduleName);
	}

	// make alsa/ext/arts first in list

	if (soundModules.contains("arts_sound"))
	{
		soundModules.remove("arts_sound");
		soundModules.prepend("arts_sound");
	}
	if (soundModules.contains("ext_sound"))
	{
		soundModules.remove("ext_sound");
		soundModules.prepend("ext_sound");
	}
	if (soundModules.contains("alsa_sound"))
	{
		soundModules.remove("alsa_sound");
		soundModules.prepend("alsa_sound");
	}
	if (soundModules.contains("dshow_sound"))
	{
		soundModules.remove("dshow_sound");
		soundModules.prepend("dshow_sound");
	}
	soundModules.prepend("None");
	soundModuleCombo->insertStringList(soundModules);

	soundPage->setLayout(gridLayout);
	
	loadSoundOptions();

	addPage(soundPage);

	kdebugf2();
}

void Wizard::testSound()
{
#ifndef Q_OS_MAC
	sound_manager->stop();
#endif
	changeSoundModule(soundModuleCombo->currentText());

	testingSound = true;
	sound_manager->play(dataPath("kadu/themes/sounds/default/msg.wav"), true);
	testingSound = false;
}

void Wizard::loadSoundOptions()
{
	backupSoundModule = modules_manager->moduleProvides("sound_driver");

	if (!backupSoundModule.isEmpty())
		soundModuleCombo->setCurrentText(backupSoundModule);
	else
		soundModuleCombo->setCurrentItem(1); // just exclude "none"
}

void Wizard::saveSoundOptions()
{
	changeSoundModule(soundModuleCombo->currentText());
	modules_manager->saveLoadedModules();
}

void Wizard::changeSoundModule(const QString &newModule)
{
	QString currentSoundModule = modules_manager->moduleProvides("sound_driver");
	if (currentSoundModule != newModule)
	{
		if (modules_manager->moduleIsLoaded(currentSoundModule))
			modules_manager->deactivateModule(currentSoundModule);

		currentSoundModule = newModule;

		if (!currentSoundModule.isEmpty() && (currentSoundModule != "None"))
			modules_manager->activateModule(currentSoundModule);
	}
}

Wizard *startWizardObj = NULL;
WizardStarter *wizardStarter = NULL;

/** @} */
