/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "config-wizard-actions.h"

#include "gui/windows/config-wizard-window.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "gui/actions/action-description.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/paths-provider.h"
#include "activate.h"
#include "debug.h"

#include <QtWidgets/QAction>

ConfigWizardActions::ConfigWizardActions(QObject *parent) :
		QObject{parent}
{
	m_showConfigWizardActionDescription = new ActionDescription(this, ActionDescription::TypeMainMenu,
			"showConfigWizard", this, SLOT(showConfigWizardSlot()), KaduIcon(),
			tr("Start Configuration Wizard"));
}

ConfigWizardActions::~ConfigWizardActions()
{
	delete m_wizard.data();
}

void ConfigWizardActions::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void ConfigWizardActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void ConfigWizardActions::registerActions()
{
	m_menuInventory
		->menu("tools")
		->addAction(m_showConfigWizardActionDescription, KaduMenu::SectionTools)
		->update();
}

void ConfigWizardActions::unregisterActions()
{
	m_menuInventory
		->menu("tools")
		->removeAction(m_showConfigWizardActionDescription)
		->update();
}

void ConfigWizardActions::showConfigWizard()
{
	kdebugf();

	if (m_wizard)
		_activateWindow(m_wizard.data());
	else
	{
		m_wizard = m_injectedFactory->makeInjected<ConfigWizardWindow>();
		// we have to delay it a bit to show after main window to have focus on startup
		QMetaObject::invokeMethod(m_wizard.data(), "show", Qt::QueuedConnection);
	}
}

void ConfigWizardActions::showConfigWizardSlot()
{
	showConfigWizard();
}

#include "moc_config-wizard-actions.cpp"
