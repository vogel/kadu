/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef TALKABLE_MENU_ITEM_H
#define TALKABLE_MENU_ITEM_H

class ActionDescription;

class TalkableMenuItem
{
public:
	enum MenuCategory
	{
		CategoryChat,
		CategoryActions,
		CategoryView,
		CategoryManagement
	};

private:
	ActionDescription *Action;
	MenuCategory Category;
	int Priority;

public:
	TalkableMenuItem(ActionDescription *action, MenuCategory category, int priority);

	bool operator < (const TalkableMenuItem &compareTo) const;

	ActionDescription * actionDescription() { return Action; }
	MenuCategory category() const { return Category; }
	int priority() const { return Priority; }

};

#endif // TALKABLE_MENU_ITEM_H
