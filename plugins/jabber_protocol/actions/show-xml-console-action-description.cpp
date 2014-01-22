/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QTimer>
#include <QtGui/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/windows/xml-console.h"
#include "identities/identity.h"

#include "show-xml-console-action-description.h"

ShowXmlConsoleActionDescription::ShowXmlConsoleActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeMainMenu);
	setName("showXmlConsole");
	setText(tr("Show XML Console"));

	registerAction();

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
			this, SLOT(updateShowXmlConsoleMenu()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
			this, SLOT(updateShowXmlConsoleMenu()));

	// It is needed bacause of loading protocol plugins before creating GUI.
	// TODO: Fix somehow. Maybe creating all action descriptions could be delayed.
	QTimer::singleShot(0, this, SLOT(insertMenuActionDescription()));
}

ShowXmlConsoleActionDescription::~ShowXmlConsoleActionDescription()
{
	// actions will delete their menus
	MenuInventory::instance()
		->menu("tools")
		->removeAction(this)
		->update();
}

void ShowXmlConsoleActionDescription::insertMenuActionDescription()
{
	MenuInventory::instance()
		->menu("tools")
		->addAction(this, KaduMenu::SectionTools)
		->update();
}

void ShowXmlConsoleActionDescription::actionInstanceCreated(Action *action)
{
	Q_UNUSED(action)

	// It may look like it was suboptimal but in reality there will be
	// only one action instance.
	updateShowXmlConsoleMenu();
}

void ShowXmlConsoleActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	menuActionTriggered(sender);
}

void ShowXmlConsoleActionDescription::updateShowXmlConsoleMenu()
{
	QVector<Account> jabberAccounts = AccountManager::instance()->byProtocolName("jabber");

	foreach (Action *action, actions())
	{
		QMenu *menu = action->menu();
		if (jabberAccounts.isEmpty() || 1 == AccountManager::instance()->items().count())
		{
			delete menu;
			action->setMenu(0);

			if (jabberAccounts.isEmpty())
			{
				action->setData(QVariant());
				action->setVisible(false);
			}
			else
			{
				action->setData(QVariant::fromValue(jabberAccounts.at(0)));
				action->setVisible(true);
			}
		}
		else
		{
			if (menu)
				menu->clear();
			else
			{
				menu = new QMenu();
				action->setMenu(menu);
				connect(menu, SIGNAL(triggered(QAction*)),
						this, SLOT(menuActionTriggered(QAction*)));
			}

			foreach (const Account &account, jabberAccounts)
			{
				QAction *menuAction = menu->addAction(QString("%1 (%2)").arg(account.accountIdentity().name(), account.id()));
				menuAction->setData(QVariant::fromValue(account));
			}

			action->setData(QVariant());
			action->setVisible(true);
		}
	}
}

void ShowXmlConsoleActionDescription::menuActionTriggered(QAction *action)
{
	Account account = action->data().value<Account>();
	if (!account)
		return;

	(new XmlConsole(account))->show();
}

#include "moc_show-xml-console-action-description.cpp"
