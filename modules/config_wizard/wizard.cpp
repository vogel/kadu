/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include <stdlib.h>
#include <unistd.h>

#include "../sound/sound.h"

#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "icons_manager.h"
#include "kadu.h"
#include "main_configuration_window.h"
#include "modules.h"
#include "wizard.h"

/**
 * @ingroup config_wizard
 * @{
 */

extern "C" int config_wizard_init()
{
	kdebugf();
	wizardStarter = new WizardStarter();

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
		wizardStarter = 0;
	}
	kdebugf2();
}

WizardStarter::WizardStarter(QObject *parent) : QObject(parent)
{
	QMenu *MainMenu = kadu->mainMenu();

	menuPos = MainMenu->insertItem(icons_manager->loadIcon("ConfigurationWizard"), tr("Configuration Wizard"), this, SLOT(start()), 0, -1, 0);
	//icons_manager->registerMenuItem(MainMenu, tr("Configuration Wizard"), "ConfigurationWizard");
}

WizardStarter::~WizardStarter()
{
	kadu->mainMenu()->removeItem(menuPos);
	if (startWizardObj)
	{
		delete startWizardObj;
		startWizardObj = 0;
	}
}

void WizardStarter::start()
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
	: QDialog(parent)
{
	kdebugf();
	setWindowTitle(tr("Kadu Wizard"));
	setMinimumSize(510, 300);

	layout = new QGridLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(10, 10, 10, 10);
	pageArea = new QStackedWidget(this);
	pageArea->setMinimumSize(490, 290);
	pageArea->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(pageArea, 0, 0, 1, 4, Qt::AlignLeft);

	createGGAccountPage();
	createApplicationsPage();
	createSoundPage();

	backButton = new QPushButton(this);
	backButton->setText(tr("< Back"));
	connect(backButton, SIGNAL(clicked()), this, SLOT(backClicked()));
	layout->addWidget(backButton, 1, 0, 1, 1);

	nextButton = new QPushButton(this);
	nextButton->setText(tr("Next >"));
	connect(nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
	layout->addWidget(nextButton, 1, 1, 1, 1);

	cancelButton = new QPushButton(this);
	cancelButton->setText(tr("Cancel"));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	layout->addWidget(cancelButton, 1, 2, 1, 1);
	
	finishButton = new QPushButton(this);
	finishButton->setText(tr("Finish"));
	connect(finishButton, SIGNAL(clicked()), this, SLOT(finishClicked()));
	finishButton->setEnabled(false);
	layout->addWidget(finishButton, 1, 3, 1, 1);

	kdebugf2();
}

Wizard::~Wizard()
{
	kdebugf();
	kdebugf2();
}

void Wizard::addPage(QWidget *page, const QString &title, const QString &description, bool lastOne)
{
	QWidget *wholePage = new QWidget(this);
	QHBoxLayout *layout = new QHBoxLayout(wholePage);
	layout->setSpacing(5);

	QTextBrowser *descriptionPane = new QTextBrowser(wholePage);
	descriptionPane->setText(description);
	layout->addWidget(descriptionPane, 1);

	page->reparent(wholePage, QPoint(0, 0), true);
	layout->addWidget(page, 2);

	pageArea->addWidget(wholePage);
	pageArea->setWindowTitle(title);
}

void Wizard::nextClicked()
{
	int index = pageArea->currentIndex() + 1;
	if (index < pageArea->count())
		pageArea->setCurrentIndex(index);

	if (index == pageArea->count() - 1)
		finishButton->setEnabled(true);
}

void Wizard::backClicked()
{
	int index = pageArea->currentIndex() - 1;
	if (index >= 0)
		pageArea->setCurrentIndex(index);
	finishButton->setEnabled(false);
}

/**
	naci�ni�cie zako�cz i zapisanie konfiguracji (o ile nie nast�pi�o wcze�niej)
**/
void Wizard::finishClicked()
{
	saveGGAccountOptions();
	saveApplicationsOptions();
	saveSoundOptions();

	startWizardObj = 0;
	deleteLater();
}

void Wizard::cancelClicked()
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
	if (!gadu->doImportUserList())
	{
		MessageBox::msg(tr("User list couldn't be imported"));
		disconnect(gadu, SIGNAL(userListImported(bool, QValueList<UserListElement>)), this, SLOT(userListImported(bool, QValueList<UserListElement>)));
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

	foreach(QWidget *widget, dontHaveNumberWidgets)
		widget->setEnabled(false);

	haveNumber->setEnabled(false);
	nextButton->setEnabled(false);
	finishButton->setEnabled(false);
	cancelButton->setEnabled(false);

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
		config_file.writeEntry("General", "UIN", (int)uin);
		config_file.writeEntry("General", "Password", pwHash(ggNewPassword->text()));

		gadu->changeID(QString::number(uin));
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

	disconnect(gadu, SIGNAL(registered(bool, UinType)), this, SLOT(registeredGGAccount(bool, UinType)));

	haveNumber->setEnabled(true);
	nextButton->setEnabled(true);
	finishButton->setEnabled(true);
	cancelButton->setEnabled(true);

	kdebugf2();
}


/**
	sprawdzenie czy trzeba importowa� list� kontakt�w czy nie i ew. import
**/

void Wizard::tryImport()
{
	if (!ggImportContacts->isChecked())
		return;

	connect(gadu, SIGNAL(userListImported(bool, QValueList<UserListElement>)),
			wizardStarter, SLOT(userListImported(bool, QValueList<UserListElement>)));

	if (gadu->currentStatus().isOffline())
	{
		connect(gadu, SIGNAL(connected()), wizardStarter, SLOT(connected()));
		kadu->setOnline(); //kaze sie polaczyc i podpina sie pod sygnal polaczenia sie z siecia
	} //jak polaczony to bez cyrkow robi import
	else if (!gadu->doImportUserList())
	{
		MessageBox::msg(tr("User list wasn't imported because of some error"));
		disconnect(gadu, SIGNAL(userListImported(bool, QValueList<UserListElement>)), wizardStarter, SLOT(userListImported(bool, QValueList<UserListElement>)));
	}
}

void Wizard::createGGAccountPage()
{
	kdebugf();
	QWidget *ggPage = new QWidget(this);

	QVBoxLayout *layout = new QVBoxLayout(ggPage);
	layout->setSpacing(5);

	QGroupBox *account = new QGroupBox(tr("Account"), ggPage);

	QWidget *container = new QWidget(account);
	QGridLayout *gridLayout = new QGridLayout(container);
	gridLayout->setSpacing(5);

	haveNumber = new QRadioButton(tr("I have a number"), container);
	connect(haveNumber, SIGNAL(toggled(bool)), this, SLOT(haveNumberChanged(bool)));
	gridLayout->addMultiCellWidget(haveNumber, 0, 0, 0, 1);

	QLabel *ggNumberLabel = new QLabel(tr("Gadu-gadu number") + ":", container);
	gridLayout->addWidget(ggNumberLabel, 1, 0, Qt::AlignRight);
	ggNumber = new QLineEdit(container);
	gridLayout->addWidget(ggNumber, 1, 1);

	QLabel *ggPasswordLabel = new QLabel(tr("Gadu-gadu password") + ":", container);
	gridLayout->addWidget(ggPasswordLabel, 2, 0, Qt::AlignRight);
	ggPassword = new QLineEdit(container);
	ggPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggPassword, 2, 1);

	ggImportContacts = new QCheckBox(tr("Import contacts"), container);
	ggImportContacts->setChecked(true);
	gridLayout->addMultiCellWidget(ggImportContacts, 3, 3, 0, 1);

	dontHaveNumber = new QRadioButton(tr("I don't have a number"), container);
	gridLayout->addMultiCellWidget(dontHaveNumber, 4, 4, 0, 1);

	QLabel *ggNewPasswordLabel = new QLabel(tr("New password") + ":", container);
	gridLayout->addWidget(ggNewPasswordLabel, 5, 0, Qt::AlignRight);
	ggNewPassword = new QLineEdit(container);
	ggNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggNewPassword, 5, 1);

	QLabel *ggReNewPasswordLabel = new QLabel(tr("Retype password") + ":", container);
	gridLayout->addWidget(ggReNewPasswordLabel, 6, 0, Qt::AlignRight);
	ggReNewPassword = new QLineEdit(container);
	ggReNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggReNewPassword, 6, 1);

	QLabel *ggEMailLabel = new QLabel(tr("Your e-mail address") + ":", container);
	gridLayout->addWidget(ggEMailLabel, 7, 0, Qt::AlignRight);
	ggEMail = new QLineEdit(container);
	gridLayout->addWidget(ggEMail, 7, 1);

	ggRegisterAccount = new QPushButton(tr("Register"), container);
	connect(ggRegisterAccount, SIGNAL(clicked()), this, SLOT(registerGGAccount()));
	gridLayout->addMultiCellWidget(ggRegisterAccount, 8, 8, 0, 1);

	QButtonGroup *haveNumberGroup = new QButtonGroup();
	haveNumberGroup->insert(haveNumber);
	haveNumberGroup->insert(dontHaveNumber);

	container->setLayout(gridLayout);
	layout->addWidget(account);

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

	addPage(ggPage, tr("Gadu-gadu account"), tr("<h3>Welcome in Kadu</h3><h4>the Gadu-gadu network client for *nix and MacOS X.</h4>"
		"<p>This is first time you launch Kadu. "
		"This wizard will help you to configure the basic settings of Kadu. "
		"If you are experienced Kadu user you may omit the wizard by clicking Cancel.</p>"
		"<p>Please enter your account data. If you don't have one, you can create new here.</p>"
		"<p>E-mail address is needed when you want to recover lost password to account</p>"), false);

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
	QWidget *applicationsPage = new QWidget(this);

	QVBoxLayout *layout = new QVBoxLayout(applicationsPage);
	layout->setSpacing(5);

	QGroupBox *browserOptions = new QGroupBox(tr("WWW browser"), applicationsPage);

	QWidget *browserContainer = new QWidget(browserOptions);
	QGridLayout *browserGridLayout = new QGridLayout(browserContainer);
	browserGridLayout->setSpacing(5);

	browserGridLayout->addWidget(new QLabel(tr("Choose your browser") + ":", browserContainer), 0, 0, Qt::AlignRight);
	browserCombo = new QComboBox(browserContainer);
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
	connect(browserCombo, SIGNAL(activated(int)), this, SLOT(browserChanged(int)));
	browserGridLayout->addWidget(browserCombo, 0, 1);

	browserGridLayout->addWidget(new QLabel(tr("Custom browser") + ":", browserContainer), 1, 0, Qt::AlignRight);
	browserCommandLineEdit = new QLineEdit(browserContainer);
	browserGridLayout->addWidget(browserCommandLineEdit, 1, 1);

	browserContainer->setLayout(browserGridLayout);
	layout->addWidget(browserOptions);

	QGroupBox *emailOptions = new QGroupBox(tr("e-mail client"), applicationsPage);

	QWidget *emailContainer = new QWidget(emailOptions);
	QGridLayout *emailGridLayout = new QGridLayout(emailContainer);
	emailGridLayout->setSpacing(5);

	emailGridLayout->addWidget(new QLabel(tr("Choose your e-mail client") + ":", emailContainer), 0, 0, Qt::AlignRight);
	mailCombo = new QComboBox(emailContainer);
	mailCombo->insertItem(tr("Specify path"));
	mailCombo->insertItem(tr("KMail"));
	mailCombo->insertItem(tr("Thunderbird"));
	mailCombo->insertItem(tr("SeaMonkey"));
	mailCombo->insertItem(tr("Evolution"));
	connect(mailCombo, SIGNAL(activated(int)), this, SLOT(emailChanged(int)));
	emailGridLayout->addWidget(mailCombo, 0, 1);

	emailGridLayout->addWidget(new QLabel(tr("Custom e-mail client") + ":", emailContainer), 1, 0, Qt::AlignRight);
	mailCommandLineEdit = new QLineEdit(emailContainer);
	emailGridLayout->addWidget(mailCommandLineEdit, 1, 1);

	emailContainer->setLayout(emailGridLayout);
	layout->addWidget(emailOptions);

	applicationsPage->setLayout(layout);

	loadApplicationsOptions();

	addPage(applicationsPage, tr("Applications"), tr(
		"<p>Please setup Kadu for working with your favourite WWW browser and email program.</p>"
		"<p>Kadu will use these for opening various links from messages and user's descriptions</p>"
	), false);

	kdebugf2();
}

void Wizard::browserChanged(int index)
{
	QString browser = MainConfigurationWindow::instance()->getBrowserExecutable(index);
	browserCommandLineEdit->setEnabled(index == 0);
	browserCommandLineEdit->setText(browser);

	if (index != 0 && browser.isEmpty())
		if (!browserCombo->currentText().contains(tr("Not found")))
			browserCombo->changeItem(browserCombo->currentText() + " (" + tr("Not found") + ")", index);
}

void Wizard::emailChanged(int index)
{
	QString mail = MainConfigurationWindow::instance()->getEMailExecutable(index);

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
	QWidget *soundPage = new QWidget(this);

	QVBoxLayout *layout = new QVBoxLayout(soundPage);
	layout->setSpacing(5);

	QGroupBox *soundOptions = new QGroupBox(tr("Sound system"), soundPage);

	QWidget *container = new QWidget(soundOptions);
	QGridLayout *gridLayout = new QGridLayout(container);
	gridLayout->setSpacing(5);

	gridLayout->addWidget(new QLabel(tr("Sound system") + ":", container), 0, 0, Qt::AlignRight);
	soundModuleCombo = new QComboBox(container);
	gridLayout->addWidget(soundModuleCombo, 0, 1);

	soundTest = new QPushButton(tr("Test sound"), container);
	gridLayout->addMultiCellWidget(soundTest, 1, 1, 0, 1);
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
	soundModules.prepend("None");

	soundModuleCombo->insertStringList(soundModules);

	container->setLayout(gridLayout);
	layout->addWidget(soundOptions);
	soundPage->setLayout(layout);
	
	loadSoundOptions();

	addPage(soundPage, tr("Sound"), tr(
		"<p>Please select your sound driver for sound notifications. "
		"If you don't want sound notifications, use None driver.</p>"
		"<p>If you don't know which driver to use, just check every ony with Test sound button."
		"Don't forget to unmute your system before!</p>"
	), true);

	kdebugf2();
}

void Wizard::testSound()
{
	sound_manager->stop();
	changeSoundModule(soundModuleCombo->currentText());
	sound_manager->play(dataPath("kadu/themes/sounds/default/msg.wav"), true);
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
