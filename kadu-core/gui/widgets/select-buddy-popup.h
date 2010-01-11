/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SELECT_BUDDY_POPUP_H
#define SELECT_BUDDY_POPUP_H

#include "gui/widgets/buddies-list-widget.h"

class QModelIndex;

class Buddy;

class SelectBuddyPopup : public BuddiesListWidget
{
	Q_OBJECT

private slots:
	void itemClicked(const QModelIndex &index);

public:
	explicit SelectBuddyPopup(QWidget *parent = 0);
	virtual ~SelectBuddyPopup();

	void show(Buddy buddy);

signals:
	void buddySelected(Buddy buddy);

};

#include "buddies/buddy.h" // for MOC

#endif // SELECT_BUDDY_POPUP_H
