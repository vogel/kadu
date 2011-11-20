/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_CONTACTS_TABLE_H
#define BUDDY_CONTACTS_TABLE_H

#include <QtCore/QModelIndex>
#include <QtGui/QWidget>

#include "buddies/buddy.h"

class QPushButton;
class QTableView;

class BuddyContactsTableDelegate;
class BuddyContactsTableModel;
class BuddyContactsTableModelProxy;

class BuddyContactsTable : public QWidget
{
	Q_OBJECT

	Buddy MyBuddy;

	QTableView *View;
	BuddyContactsTableDelegate *Delegate;
	BuddyContactsTableModel *Model;
	BuddyContactsTableModelProxy *Proxy;

	QPushButton *MoveUpButton;
	QPushButton *MoveDownButton;
	QPushButton *AddContactButton;
	QPushButton *DetachContactButton;
	QPushButton *RemoveContactButton;

	void createGui();

private slots:
	void viewSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

	void moveUpClicked();
	void moveDownClicked();
	void addClicked();
	void detachClicked();
	void removeClicked();

public:
	explicit BuddyContactsTable(Buddy buddy, QWidget *parent = 0);
	virtual ~BuddyContactsTable();

	bool isValid();
	void save();

signals:
	void validChanged();

};

#endif // BUDDY_CONTACTS_TABLE_H
