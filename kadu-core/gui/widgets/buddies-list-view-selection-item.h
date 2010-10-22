/*
 * %kadu copyright begin%
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

#ifndef BUDDIES_LIST_VIEW_SELECTION_ITEM_H
#define BUDDIES_LIST_VIEW_SELECTION_ITEM_H

#include "buddies/buddy.h"
#include "contacts/contact.h"

class BuddiesListViewSelectionItem
{
public:
	enum SelectedItemType
	{
		SelectedItemNone,
		SelectedItemBuddy,
		SelectedItemContact
	};

private:
	SelectedItemType SelectedItem;
	Buddy SelectedBuddy;
	Contact SelectedContact;

public:
	BuddiesListViewSelectionItem();
	BuddiesListViewSelectionItem(SelectedItemType selectedItem, Buddy selectedBuddy, Contact selectedContact);
	BuddiesListViewSelectionItem(const BuddiesListViewSelectionItem& copyMe);

	BuddiesListViewSelectionItem & operator = (const BuddiesListViewSelectionItem &copyMe);

	SelectedItemType selectedItem() { return SelectedItem; }
	Buddy selectedBuddy() { return SelectedBuddy; }
	Contact selectedContact() { return SelectedContact; }

};

#endif // BUDDIES_LIST_VIEW_SELECTION_ITEM_H

