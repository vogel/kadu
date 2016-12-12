/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#include "kadu-menu.h"

class QMenu;
class ActionDescription;

class MenuItem
{
	ActionDescription *Action;
	KaduMenu::MenuSection Section;
	int Priority;

public:
	MenuItem(ActionDescription *action, KaduMenu::MenuSection section, int priority = 0);

	bool operator < (const MenuItem &compareTo) const;

	ActionDescription * actionDescription() { return Action; }
	KaduMenu::MenuSection section() const { return Section; }
	int priority() const { return Priority; }

};

#endif // MENU_ITEM_H
