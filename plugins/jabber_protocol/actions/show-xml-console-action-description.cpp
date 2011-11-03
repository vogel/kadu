/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/xml-console.h"
#include "identities/identity.h"

#include "show-xml-console-action-description.h"

ShowXmlConsoleActionDescription::ShowXmlConsoleActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeMainMenu);
	setName("showXmlConsole");
	setText(tr("Show XML Console"));

	ShowXmlConsoleMenu = new QMenu();
	updateShowXmlConsoleMenu();
	connect(ShowXmlConsoleMenu, SIGNAL(triggered(QAction*)),
			this, SLOT(menuActionTriggered(QAction*)));

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
	Core::instance()->kaduWindow()->removeMenuActionDescription(this);

	// action is owner of this object
	ShowXmlConsoleMenu = 0;
}

void ShowXmlConsoleActionDescription::insertMenuActionDescription()
{
	Core::instance()->kaduWindow()->insertMenuActionDescription(this, KaduWindow::MenuTools);
}

void ShowXmlConsoleActionDescription::actionInstanceCreated(Action *action)
{
	action->setMenu(ShowXmlConsoleMenu);
	action->setVisible(!ShowXmlConsoleMenu->actions().isEmpty());
}

void ShowXmlConsoleActionDescription::updateShowXmlConsoleMenu()
{
	ShowXmlConsoleMenu->clear();

	foreach (const Account &account, AccountManager::instance()->items())
		if (account.protocolName() == QLatin1String("jabber"))
		{
			QAction *action = new QAction(QString("%1 (%2)").arg(account.accountIdentity().name(), account.id()), ShowXmlConsoleMenu);
			action->setData(QVariant::fromValue(account));
			ShowXmlConsoleMenu->addAction(action);
		}

	bool enable = !ShowXmlConsoleMenu->actions().isEmpty();
	foreach (Action *action, actions())
		action->setVisible(enable);
}

void ShowXmlConsoleActionDescription::menuActionTriggered(QAction *action)
{
	Account account = action->data().value<Account>();
	if (!account)
		return;

	(new XmlConsole(account))->show();
}
