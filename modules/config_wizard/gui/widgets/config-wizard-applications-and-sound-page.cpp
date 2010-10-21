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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "modules/sound/sound-manager.h"

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

	BrowserCheckBox = new QCheckBox(this);
	BrowserLineEdit = new QLineEdit(this);
	connect(BrowserCheckBox, SIGNAL(toggled(bool)), BrowserLineEdit, SLOT(setDisabled(bool)));

	formLayout()->addRow(tr("Use Default Web Browser") + ':', BrowserCheckBox);
	formLayout()->addRow(tr("Web Browser Executable") + ':', BrowserLineEdit);

	EMailCheckBox = new QCheckBox(this);
	EMailLineEdit = new QLineEdit(this);
	connect(EMailCheckBox, SIGNAL(toggled(bool)), EMailLineEdit, SLOT(setDisabled(bool)));

	formLayout()->addRow(tr("Use Default E-Mail Application") + ':', EMailCheckBox);
	formLayout()->addRow(tr("E-Mail Application Executable") + ':', EMailLineEdit);

	formLayout()->addRow(new QLabel(tr("<b>Sound</b>"), this));

	SoundModulesCombo = new QComboBox(this);
	setSoundDrivers();

	formLayout()->addRow(tr("Sound driver") + ':', SoundModulesCombo);

	QPushButton *soundTestButton = new QPushButton(tr("Test ..."));
	connect(soundTestButton, SIGNAL(clicked(bool)), this, SLOT(testSound()));

	formLayout()->addRow("", soundTestButton);
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

void ConfigWizardApplicationsAndSoundPage::changeSoundModule(const QString &newSoundModule)
{
	QString currentSoundModule = ModulesManager::instance()->moduleProvides("sound_driver");
	if (currentSoundModule == newSoundModule)
		return;

	if (ModulesManager::instance()->moduleIsLoaded(currentSoundModule))
		ModulesManager::instance()->deactivateModule(currentSoundModule);

	currentSoundModule = newSoundModule;

	if (!currentSoundModule.isEmpty() &&  ModulesManager::instance()->moduleIsInstalled(currentSoundModule))
		ModulesManager::instance()->activateModule(currentSoundModule);
}

void ConfigWizardApplicationsAndSoundPage::testSound()
{
	changeSoundModule(SoundModulesCombo->currentText());

	IsTestingSound = true;
	SoundManager::instance()->playFile(dataPath("kadu/themes/sounds/default/msg.wav"), true);
	IsTestingSound = false;
}

bool ConfigWizardApplicationsAndSoundPage::validatePage()
{
    return !IsTestingSound;
}

void ConfigWizardApplicationsAndSoundPage::initializeApplications()
{
	BrowserCheckBox->setChecked(config_file.readBoolEntry("Chat", "UseDefaultWebBrowser", true));
	EMailCheckBox->setChecked(config_file.readBoolEntry("Chat", "UseDefaultEMailClient", true));
}

void ConfigWizardApplicationsAndSoundPage::initializeSound()
{
	OldSoundModule = ModulesManager::instance()->moduleProvides("sound_driver");

	if (!OldSoundModule.isEmpty())
		SoundModulesCombo->setCurrentIndex(SoundModulesCombo->findText(OldSoundModule));
	else
		SoundModulesCombo->setCurrentIndex(1); // just exclude "none"*/
}

void ConfigWizardApplicationsAndSoundPage::initializePage()
{
	initializeApplications();
	initializeSound();
}

void ConfigWizardApplicationsAndSoundPage::acceptApplications()
{
	config_file.writeEntry("Chat", "UseDefaultWebBrowser", BrowserCheckBox->isChecked());
	config_file.writeEntry("Chat", "WebBrowser", BrowserLineEdit->text());
	config_file.writeEntry("Chat", "UseDefaultEMailClient", EMailCheckBox->isChecked());
	config_file.writeEntry("Chat", "MailClient", EMailLineEdit->text());
}

void ConfigWizardApplicationsAndSoundPage::acceptSound()
{
	changeSoundModule(SoundModulesCombo->currentText());
	ModulesManager::instance()->saveLoadedModules();
}

void ConfigWizardApplicationsAndSoundPage::acceptPage()
{
	acceptApplications();
	acceptSound();
}

void ConfigWizardApplicationsAndSoundPage::rejectPage()
{
    changeSoundModule(OldSoundModule);
}
