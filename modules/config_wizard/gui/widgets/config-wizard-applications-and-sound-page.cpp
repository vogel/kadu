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
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>

#include "modules.h"
#include "languages-manager.h"

#include "config-wizard-applications-and-sound-page.h"

ConfigWizardApplicationsAndSoundPage::ConfigWizardApplicationsAndSoundPage(QWidget *parent) :
		QWizardPage(parent)
{
	createGui();
}

ConfigWizardApplicationsAndSoundPage::~ConfigWizardApplicationsAndSoundPage()
{
}

void ConfigWizardApplicationsAndSoundPage::createGui()
{
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(5);

	QTextBrowser *descriptionPane = new QTextBrowser(this);
	descriptionPane->setText(tr("<p>Please specify the default browser and e-mail program to be used with Kadu.</p>"
		"<p>Kadu will use these programs to open links from chat messages and user descriptions.</p>"
		"<p>Also, please select your sound driver for sound notifications.</p>"));

	mainLayout->addWidget(descriptionPane, 2);

	QWidget *formWidget = new QWidget(this);
	QFormLayout *formLayout = new QFormLayout(formWidget);

	mainLayout->addWidget(formWidget, 5);

	formLayout->addRow(new QLabel(tr("<h3>Applications and sound</h3>"), this));

	formLayout->addRow(new QLabel(tr("<b>Web and E-Mail</b>"), this));

	BrowserCombo = new QComboBox(this);
	setBrowsers();

	formLayout->addRow(tr("Web Browser") + ":", BrowserCombo);

	EMailCombo = new QComboBox(this);
	setEMails();

	formLayout->addRow(tr("E-Mail Application") + ":", EMailCombo);

	formLayout->addRow(new QLabel(tr("<b>Sound</b>"), this));

	SoundModulesCombo = new QComboBox(this);
	setSoundDrivers();

	formLayout->addRow(tr("Sound driver") + ":", SoundModulesCombo);

	QPushButton *soundTestButton = new QPushButton(tr("Test ..."));

	formLayout->addRow("", soundTestButton);
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
