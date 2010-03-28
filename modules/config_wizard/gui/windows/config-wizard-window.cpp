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
#include <QtGui/QTextBrowser>

#include <stdlib.h>

#include "../sound/sound.h"

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "debug.h"
#include "html_document.h"
#include "icons-manager.h"
#include "modules.h"

#include "gui/widgets/config-wizard-applications-and-sound-page.h"
#include "gui/widgets/config-wizard-completed-page.h"
#include "gui/widgets/config-wizard-profile-page.h"

#include "config-wizard-window.h"

/**
 * @ingroup config_wizard
 * @{
 */

ConfigWizardWindow::ConfigWizardWindow(QWidget *parent) :
		QWizard(parent), testingSound(false)
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

	addPage(new ConfigWizardProfilePage(this));
	addPage(new ConfigWizardApplicationsAndSoundPage(this));
	addPage(new ConfigWizardCompletedPage(this));

	createApplicationsPage();
	createSoundPage();

	connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
	connect(this, SIGNAL(rejected()), this, SLOT(rejectedSlot()));

	kdebugf2();
}

ConfigWizardWindow::~ConfigWizardWindow()
{
	kdebugf();
	kdebugf2();
}

bool ConfigWizardWindow::validateCurrentPage()
{
	return !testingSound;
}

/**
	naci�ni�cie zako�cz i zapisanie konfiguracji (o ile nie nast�pi�o wcze�niej)
**/
void ConfigWizardWindow::acceptedSlot()
{
	saveApplicationsOptions();
	saveSoundOptions();

	deleteLater();
}

void ConfigWizardWindow::rejectedSlot()
{
	changeSoundModule(backupSoundModule);

	deleteLater();
}

void ConfigWizardWindow::closeEvent(QCloseEvent *e)
{
	QDialog::closeEvent(e);

	deleteLater();
}

/**
	wywo�anie wizarda z menu
**/
void ConfigWizardWindow::wizardStart()
{
	show();
}

/**
	opcje przegladarki www
**/
void ConfigWizardWindow::createApplicationsPage()
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

// 	gridLayout->addMultiCellWidget(descriptionPane, 0, 4, 0, 0);

	gridLayout->addWidget(new QLabel(tr("Choose your browser") + ":", applicationsPage), 0, 2, Qt::AlignRight);
	browserCombo = new QComboBox(applicationsPage);
	browserCombo->addItem(tr("Specify path"));
	browserCombo->addItem(tr("Konqueror"));
	browserCombo->addItem(tr("Opera"));
	browserCombo->addItem(tr("Opera (new tab)"));
	browserCombo->addItem(tr("SeaMonkey"));
	browserCombo->addItem(tr("Mozilla"));
	browserCombo->addItem(tr("Mozilla Firefox"));
	browserCombo->addItem(tr("Dillo"));
	browserCombo->addItem(tr("Galeon"));
	browserCombo->addItem(tr("Safari"));
	browserCombo->addItem(tr("Camino"));
	connect(browserCombo, SIGNAL(activated(int)), this, SLOT(browserChanged(int)));
	gridLayout->addWidget(browserCombo, 0, 3);

	gridLayout->addWidget(new QLabel(tr("Custom browser") + ":", applicationsPage), 1, 2, Qt::AlignRight);
	browserCommandLineEdit = new QLineEdit(applicationsPage);
	gridLayout->addWidget(browserCommandLineEdit, 1, 3);
 
	gridLayout->addWidget(new QLabel(tr("Choose your e-mail client") + ":", applicationsPage), 2, 2, Qt::AlignRight);
	mailCombo = new QComboBox(applicationsPage);
	mailCombo->addItem(tr("Specify path"));
	mailCombo->addItem(tr("KMail"));
	mailCombo->addItem(tr("Thunderbird"));
	mailCombo->addItem(tr("SeaMonkey"));
	mailCombo->addItem(tr("Evolution"));
	mailCombo->addItem("Mail");
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

void ConfigWizardWindow::browserChanged(int index)
{
	QString browser = MainConfigurationWindow::getBrowserExecutable(index);
	browserCommandLineEdit->setEnabled(index == 0);
	browserCommandLineEdit->setText(browser);
/*
	if (index != 0 && browser.isEmpty())
		if (!browserCombo->currentText().contains(tr("Not found")))
			browserCombo->changeItem(browserCombo->currentText() + " (" + tr("Not found") + ")", index);*/
}

void ConfigWizardWindow::emailChanged(int index)
{
	QString mail = MainConfigurationWindow::getEMailExecutable(index);

	mailCommandLineEdit->setEnabled(index == 0);
	mailCommandLineEdit->setText(mail);
/*
	if (index != 0 && mail.isEmpty())
		if (!mailCombo->currentText().contains(tr("Not found")))
			mailCombo->changeItem(mailCombo->currentText() + " (" + tr("Not found") + ")", index);*/
}

// don't care for performance here
void ConfigWizardWindow::loadApplicationsOptions()
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

// 	browserCombo->setCurrentItem(foundBrowserIndex);
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

// 	mailCombo->setCurrentItem(foundMailIndex);
	emailChanged(foundMailIndex);
}

void ConfigWizardWindow::saveApplicationsOptions()
{
	config_file.writeEntry("Chat", "WebBrowserNo", MainConfigurationWindow::browserIndexToString(browserCombo->currentIndex()));
	config_file.writeEntry("Chat", "WebBrowser", browserCommandLineEdit->text());
	config_file.writeEntry("Chat", "EmailClientNo", MainConfigurationWindow::emailIndexToString(mailCombo->currentIndex()));
	config_file.writeEntry("Chat", "MailClient", mailCommandLineEdit->text());
}

void ConfigWizardWindow::createSoundPage()
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

// 	gridLayout->addMultiCellWidget(descriptionPane, 0, 2, 0, 0);

	gridLayout->addWidget(new QLabel(tr("Sound system") + ":", soundPage), 0, 2, Qt::AlignRight);
	soundModuleCombo = new QComboBox(soundPage);
	gridLayout->addWidget(soundModuleCombo, 0, 3);

	soundTest = new QPushButton(tr("Test sound"), soundPage);
// 	gridLayout->addMultiCellWidget(soundTest, 1, 1, 2, 3);
	connect(soundTest, SIGNAL(clicked()), this, SLOT(testSound()));

	QStringList soundModules;
	ModuleInfo moduleInfo;

	QStringList moduleList = ModulesManager::instance()->staticModules();
	foreach(const QString &moduleName, moduleList)
		if (ModulesManager::instance()->moduleInfo(moduleName, moduleInfo))
			if (moduleInfo.provides.contains("sound_driver"))
			{
				soundModules.append(moduleName);
				break;
			}

	if (soundModules.size() == 0)
	{
		moduleList = ModulesManager::instance()->installedModules();
		foreach(const QString &moduleName, moduleList)
			if (ModulesManager::instance()->moduleInfo(moduleName, moduleInfo) && moduleInfo.provides.contains("sound_driver"))
				soundModules.append(moduleName);
	}

	// make alsa/ext/arts first in list

	if (soundModules.contains("arts_sound"))
	{
// 		soundModules.remove("arts_sound");
		soundModules.prepend("arts_sound");
	}
	if (soundModules.contains("ext_sound"))
	{
// 		soundModules.remove("ext_sound");
		soundModules.prepend("ext_sound");
	}
	if (soundModules.contains("alsa_sound"))
	{
// 		soundModules.remove("alsa_sound");
		soundModules.prepend("alsa_sound");
	}
	if (soundModules.contains("dshow_sound"))
	{
// 		soundModules.remove("dshow_sound");
		soundModules.prepend("dshow_sound");
	}
	soundModules.prepend("None");
// 	soundModuleCombo->insertStringList(soundModules);

	soundPage->setLayout(gridLayout);
	
	loadSoundOptions();

	addPage(soundPage);

	kdebugf2();
}

void ConfigWizardWindow::testSound()
{
#ifndef Q_OS_MAC
	sound_manager->stop();
#endif
	changeSoundModule(soundModuleCombo->currentText());

	testingSound = true;
	sound_manager->play(dataPath("kadu/themes/sounds/default/msg.wav"), true);
	testingSound = false;
}

void ConfigWizardWindow::loadSoundOptions()
{
	backupSoundModule = ModulesManager::instance()->moduleProvides("sound_driver");
/*
	if (!backupSoundModule.isEmpty())
		soundModuleCombo->setCurrentText(backupSoundModule);
	else
		soundModuleCombo->setCurrentItem(1); // just exclude "none"*/
}

void ConfigWizardWindow::saveSoundOptions()
{
	changeSoundModule(soundModuleCombo->currentText());
	ModulesManager::instance()->saveLoadedModules();
}

void ConfigWizardWindow::changeSoundModule(const QString &newModule)
{
	QString currentSoundModule = ModulesManager::instance()->moduleProvides("sound_driver");
	if (currentSoundModule != newModule)
	{
		if (ModulesManager::instance()->moduleIsLoaded(currentSoundModule))
			ModulesManager::instance()->deactivateModule(currentSoundModule);

		currentSoundModule = newModule;

		if (!currentSoundModule.isEmpty() && (currentSoundModule != "None"))
			ModulesManager::instance()->activateModule(currentSoundModule);
	}
}

/** @} */
