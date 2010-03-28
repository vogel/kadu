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
		QWizard(parent)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Kadu Wizard"));

	setOption(IndependentPages, true);

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
/*
	createApplicationsPage();
	createSoundPage();*/

	connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
	connect(this, SIGNAL(rejected()), this, SLOT(rejectedSlot()));

	kdebugf2();
}

ConfigWizardWindow::~ConfigWizardWindow()
{
	kdebugf();
	kdebugf2();
}

void ConfigWizardWindow::addPage(ConfigWizardPage *page)
{
	ConfigWizardPages.append(page);

	QWizard::addPage(page);
}

void ConfigWizardWindow::acceptedSlot()
{
	foreach (ConfigWizardPage *page, ConfigWizardPages)
		page->acceptPage();
}

void ConfigWizardWindow::rejectedSlot()
{
	foreach (ConfigWizardPage *page, ConfigWizardPages)
		page->rejectPage();
}

/**
	wywo�anie wizarda z menu
**/
void ConfigWizardWindow::wizardStart()
{
	show();
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

	kdebugf2();
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
// 	changeSoundModule(soundModuleCombo->currentText());
	ModulesManager::instance()->saveLoadedModules();
}

/** @} */
