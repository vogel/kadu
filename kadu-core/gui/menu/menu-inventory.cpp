/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "contacts/contact-set.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "protocols/protocol-menu-manager.h"

#include "menu-inventory.h"

MenuInventory * MenuInventory::Instance = 0;

MenuInventory * MenuInventory::instance()
{
	if (!Instance)
		Instance = new MenuInventory();

	return Instance;
}

MenuInventory::MenuInventory()
{
}

void MenuInventory::bindMenu(const QString &category, QMenu *menu)
{
	KaduMenu *kaduMenu = new KaduMenu(category);
	kaduMenu->attachToMenu(menu);
	Menus.insert(category, kaduMenu);
}

KaduMenu * MenuInventory::menu(const QString &category)
{
	KaduMenu *menu = Menus.value(category);

	if (!menu)
		bindMenu(category, new QMenu());

	menu = Menus.value(category);

	return menu;
}

void MenuInventory::registerProtocolMenuManager(ProtocolMenuManager *manager)
{
	ProtocolMenuManagers.append(manager);
}

void MenuInventory::unregisterProtocolMenuManager(ProtocolMenuManager *manager)
{
	ProtocolMenuManagers.removeAll(manager);
}
