/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gui/actions/action-description.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/kadu-paths.h"
#include "activate.h"
#include "debug.h"

#include "gui/windows/config-wizard-window.h"

#include "config-wizard-configuration-ui-handler.h"

ConfigWizardConfigurationUiHandler *ConfigWizardConfigurationUiHandler::Instance = 0;

void ConfigWizardConfigurationUiHandler::registerActions(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (Instance)
		return;

	Instance = new ConfigWizardConfigurationUiHandler();
}

void ConfigWizardConfigurationUiHandler::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

ConfigWizardConfigurationUiHandler * ConfigWizardConfigurationUiHandler::instance()
{
	return Instance;
}

ConfigWizardConfigurationUiHandler::ConfigWizardConfigurationUiHandler()
{
	ShowConfigWizardActionDescription = new ActionDescription(this, ActionDescription::TypeMainMenu,
			"showConfigWizard", this, SLOT(showConfigWizardSlot()), KaduIcon(),
			tr("Start Configuration Wizard"));

	MenuInventory::instance()
		->menu("tools")
		->addAction(ShowConfigWizardActionDescription, KaduMenu::SectionTools)
		->update();
}

ConfigWizardConfigurationUiHandler::~ConfigWizardConfigurationUiHandler()
{
	MenuInventory::instance()
		->menu("tools")
		->removeAction(ShowConfigWizardActionDescription)
		->update();

	delete Wizard.data();
}

void ConfigWizardConfigurationUiHandler::showConfigWizard()
{
	kdebugf();

	if (Wizard)
		_activateWindow(Wizard.data());
	else
	{
		Wizard = new ConfigWizardWindow();
		// we have to delay it a bit to show after main window to have focus on startup
		QMetaObject::invokeMethod(Wizard.data(), "show", Qt::QueuedConnection);
	}
}

void ConfigWizardConfigurationUiHandler::showConfigWizardSlot()
{
	showConfigWizard();
}

#include "moc_config-wizard-configuration-ui-handler.cpp"
