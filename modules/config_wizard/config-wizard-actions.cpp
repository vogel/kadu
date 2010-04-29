/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAction>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "misc/path-conversion.h"
#include "debug.h"

#include "gui/windows/config-wizard-window.h"

#include "config-wizard-actions.h"

ConfigWizardActions *ConfigWizardActions::Instance = 0;

void ConfigWizardActions::registerActions(bool firstLoad)
{
	Q_UNUSED(firstLoad)
	
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/config-wizard.ui"));

	Instance = new ConfigWizardActions();
}

void ConfigWizardActions::unregisterActions()
{
  	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/config-wizard.ui"));
	delete Instance;
	Instance = 0;
}

ConfigWizardActions * ConfigWizardActions::instance()
{
	return Instance;
}

ConfigWizardActions::ConfigWizardActions() :
		ConfigurationWindow(0)
{
	/*ShowConfigWizardActionDescription = new ActionDescription(this,
			ActionDescription::TypeMainMenu, "configWizardAction",
			this, SLOT(showConfigWizardActionActivated(QAction *, bool)),
			"kadu/kadu-wizard.png",
			"kadu/kadu-wizard.png",
			tr("Configuration Wizard"), false, ""
	);
	Core::instance()->kaduWindow()->insertMenuActionDescription(ShowConfigWizardActionDescription, KaduWindow::MenuKadu, 0);*/
}

ConfigWizardActions::~ConfigWizardActions()
{
	//Core::instance()->kaduWindow()->removeMenuActionDescription(ShowConfigWizardActionDescription);
}

void ConfigWizardActions::showConfigWizard()
{
	kdebugf();

	ConfigWizardWindow *wizard = new ConfigWizardWindow();
	wizard->show();
}

void ConfigWizardActions::showConfigWizardActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	showConfigWizard();
}

void ConfigWizardActions::showConfigWizardSlot()
{
	showConfigWizard();
}

void ConfigWizardActions::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigurationWindow = mainConfigurationWindow;
	connect(ConfigurationWindow, SIGNAL(destroyed()), this, SLOT(configurationWindowDestroyed()));

	QWidget *startButton = mainConfigurationWindow->widget()->widgetById("config-wizard/start");

	connect(startButton, SIGNAL(clicked()), this, SLOT(showConfigWizardSlot()));
}

void ConfigWizardActions::configurationWindowDestroyed()
{
	ConfigurationWindow = 0;
}
