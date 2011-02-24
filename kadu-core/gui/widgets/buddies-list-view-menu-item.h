/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef BUDDIES_LIST_VIEW_MENU_ITEM_H
#define BUDDIES_LIST_VIEW_MENU_ITEM_H

class ActionDescription;

class BuddiesListViewMenuItem
{
public:
	enum BuddiesListViewMenuCategory
	{
		MenuCategoryChat,
		MenuCategoryActions,
		MenuCategoryView,
		MenuCategoryManagement
	};

private:
	ActionDescription *Action;
	BuddiesListViewMenuCategory Category;
	int Priority;

public:
	BuddiesListViewMenuItem(ActionDescription *action, BuddiesListViewMenuCategory category, int priority);

	bool operator < (const BuddiesListViewMenuItem &compareTo) const;

	ActionDescription * actionDescription() { return Action; }
	BuddiesListViewMenuCategory category() const { return Category; }
	int priority() const { return Priority; }

};

#endif // BUDDIES_LIST_VIEW_MENU_ITEM_H
