/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "menu-inventory.h"

#include "accounts/account.h"
#include "contacts/contact-set.h"
#include "core/injected-factory.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "protocols/protocol-menu-manager.h"

#include <QtWidgets/QMenu>

MenuInventory::MenuInventory(QObject *parent) :
		QObject{parent}
{
}

MenuInventory::~MenuInventory()
{
}

void MenuInventory::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

KaduMenu * MenuInventory::menu(const QString &category)
{
	if (!Menus.contains(category))
		Menus.insert(category, m_injectedFactory->makeInjected<KaduMenu>(category));

	return Menus.value(category);
}

void MenuInventory::registerProtocolMenuManager(ProtocolMenuManager *manager)
{
	ProtocolMenuManagers.append(manager);
}

void MenuInventory::unregisterProtocolMenuManager(ProtocolMenuManager *manager)
{
	ProtocolMenuManagers.removeAll(manager);
}

#include "moc_menu-inventory.cpp"
