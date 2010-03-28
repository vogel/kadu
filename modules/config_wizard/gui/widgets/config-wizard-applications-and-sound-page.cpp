/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "modules/sound/sound.h"

#include "modules.h"
#include "languages-manager.h"

#include "config-wizard-applications-and-sound-page.h"

ConfigWizardApplicationsAndSoundPage::ConfigWizardApplicationsAndSoundPage(QWidget *parent) :
		ConfigWizardPage(parent), IsTestingSound(false)
{
	setDescription(tr("<p>Please specify the default browser and e-mail program to be used with Kadu.</p>"
		"<p>Kadu will use these programs to open links from chat messages and user descriptions.</p>"
		"<p>Also, please select your sound driver for sound notifications.</p>"));

	createGui();
}

ConfigWizardApplicationsAndSoundPage::~ConfigWizardApplicationsAndSoundPage()
{
}

void ConfigWizardApplicationsAndSoundPage::createGui()
{
	formLayout()->addRow(new QLabel(tr("<h3>Applications and sound</h3>"), this));

	formLayout()->addRow(new QLabel(tr("<b>Web and E-Mail</b>"), this));

	BrowserCombo = new QComboBox(this);
	connect(BrowserCombo, SIGNAL(activated(int)), this, SLOT(browserChanged(int)));
	setBrowsers();

	BrowserLineEdit = new QLineEdit(this);

	formLayout()->addRow(tr("Web Browser") + ":", BrowserCombo);
	formLayout()->addRow(tr("Web Browser Executable") + ":", BrowserLineEdit);

	EMailCombo = new QComboBox(this);
	connect(EMailCombo, SIGNAL(activated(int)), this, SLOT(emailChanged(int)));
	setEMails();

	EMailLineEdit = new QLineEdit(this);

	formLayout()->addRow(tr("E-Mail Application") + ":", EMailCombo);
	formLayout()->addRow(tr("E-Mail Application Executable") + ":", EMailLineEdit);

	formLayout()->addRow(new QLabel(tr("<b>Sound</b>"), this));

	SoundModulesCombo = new QComboBox(this);
	setSoundDrivers();

	formLayout()->addRow(tr("Sound driver") + ":", SoundModulesCombo);

	QPushButton *soundTestButton = new QPushButton(tr("Test ..."));
	connect(soundTestButton, SIGNAL(clicked(bool)), this, SLOT(testSound()));

	formLayout()->addRow("", soundTestButton);
}

void ConfigWizardApplicationsAndSoundPage::setBrowsers()
{
	BrowserCombo->addItem(tr("- Select Browser -"));
	BrowserCombo->addItem(tr("Konqueror"));
	BrowserCombo->addItem(tr("Opera"));
	BrowserCombo->addItem(tr("Opera (new tab)"));
	BrowserCombo->addItem(tr("SeaMonkey"));
	BrowserCombo->addItem(tr("Mozilla"));
	BrowserCombo->addItem(tr("Mozilla Firefox"));
	BrowserCombo->addItem(tr("Dillo"));
	BrowserCombo->addItem(tr("Galeon"));
	BrowserCombo->addItem(tr("Safari"));
	BrowserCombo->addItem(tr("Camino"));
}

void ConfigWizardApplicationsAndSoundPage::setEMails()
{
	EMailCombo->addItem(tr("- Select E-Mail Application -"));
	EMailCombo->addItem(tr("KMail"));
	EMailCombo->addItem(tr("Thunderbird"));
	EMailCombo->addItem(tr("SeaMonkey"));
	EMailCombo->addItem(tr("Evolution"));
}

void ConfigWizardApplicationsAndSoundPage::setSoundDrivers()
{
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
		soundModules.removeAll("arts_sound");
		soundModules.prepend("arts_sound");
	}
	if (soundModules.contains("ext_sound"))
	{
		soundModules.removeAll("ext_sound");
		soundModules.prepend("ext_sound");
	}
	if (soundModules.contains("alsa_sound"))
	{
		soundModules.removeAll("alsa_sound");
		soundModules.prepend("alsa_sound");
	}
	if (soundModules.contains("dshow_sound"))
	{
		soundModules.removeAll("dshow_sound");
		soundModules.prepend("dshow_sound");
	}
	soundModules.prepend("- Select Sound Configuration -");

	SoundModulesCombo->addItems(soundModules);
}

void ConfigWizardApplicationsAndSoundPage::browserChanged(int index)
{
	QString browser = MainConfigurationWindow::getBrowserExecutable(index);

	BrowserLineEdit->setEnabled(index == 0);
	BrowserLineEdit->setText(browser);

	if (index != 0 && browser.isEmpty())
		if (!BrowserCombo->currentText().contains(tr("Not found")))
			BrowserCombo->setItemText(index, BrowserCombo->currentText() + " (" + tr("Not found") + ")");
}

void ConfigWizardApplicationsAndSoundPage::emailChanged(int index)
{
	QString mail = MainConfigurationWindow::getEMailExecutable(index);

	EMailLineEdit->setEnabled(index == 0);
	EMailLineEdit->setText(mail);

	if (index != 0 && mail.isEmpty())
		if (!EMailCombo->currentText().contains(tr("Not found")))
			EMailCombo->setItemText(index, EMailCombo->currentText() + " (" + tr("Not found") + ")");
}

void ConfigWizardApplicationsAndSoundPage::changeSoundModule(const QString &newSoundModule)
{
	QString currentSoundModule = ModulesManager::instance()->moduleProvides("sound_driver");
	if (currentSoundModule != newSoundModule)
	{
		if (ModulesManager::instance()->moduleIsLoaded(currentSoundModule))
			ModulesManager::instance()->deactivateModule(currentSoundModule);

		currentSoundModule = newSoundModule;

		if (!currentSoundModule.isEmpty() &&  ModulesManager::instance()->moduleIsInstalled(currentSoundModule))
			ModulesManager::instance()->activateModule(currentSoundModule);
	}
}

void ConfigWizardApplicationsAndSoundPage::testSound()
{
#ifndef Q_OS_MAC
	sound_manager->stop();
#endif

	changeSoundModule(SoundModulesCombo->currentText());

	IsTestingSound = true;
	sound_manager->play(dataPath("kadu/themes/sounds/default/msg.wav"), true);
	IsTestingSound = false;
}

bool ConfigWizardApplicationsAndSoundPage::validatePage()
{
    return !IsTestingSound;
}

void ConfigWizardApplicationsAndSoundPage::initializePage()
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

	BrowserCombo->setCurrentIndex(foundBrowserIndex);
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

	EMailCombo->setCurrentIndex(foundMailIndex);
	emailChanged(foundMailIndex);
}

void ConfigWizardApplicationsAndSoundPage::acceptPage()
{
	config_file.writeEntry("Chat", "WebBrowserNo", MainConfigurationWindow::browserIndexToString(BrowserCombo->currentIndex()));
	config_file.writeEntry("Chat", "WebBrowser", BrowserLineEdit->text());
	config_file.writeEntry("Chat", "EmailClientNo", MainConfigurationWindow::emailIndexToString(EMailCombo->currentIndex()));
	config_file.writeEntry("Chat", "MailClient", EMailLineEdit->text());
}
