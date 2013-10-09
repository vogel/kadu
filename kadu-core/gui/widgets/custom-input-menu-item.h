/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Slawomir Stepien (s.stepien@interia.pl)
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

#ifndef CUSTOM_INPUT_MENU_ITEM_H
#define CUSTOM_INPUT_MENU_ITEM_H

class ActionDescription;

class CustomInputMenuItem
{
public:
	enum CustomInputMenuCategory
	{
		MenuCategoryTextEdit,
		MenuCategoryModule,
		MenuCategorySuggestion,
	};

private:
	ActionDescription *Action;
	CustomInputMenuCategory Category;
	int Priority;

public:
	CustomInputMenuItem(ActionDescription *action, CustomInputMenuCategory category, int priority);

	bool operator < (const CustomInputMenuItem &compareTo) const;

	ActionDescription * actionDescription() const { return Action; }
	CustomInputMenuCategory category() const { return Category; }
	int priority() const { return Priority; }
};

#endif // CUSTOM_INPUT_MENU_ITEM_H
