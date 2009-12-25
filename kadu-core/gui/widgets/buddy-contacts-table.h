/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_CONTACTS_TABLE_H
#define BUDDY_CONTACTS_TABLE_H

#include <QtGui/QWidget>

class QTableView;

class Buddy;
class BuddyContactsTableModel;

class BuddyContactsTable : public QWidget
{
	Q_OBJECT

	QTableView *View;
	BuddyContactsTableModel *Model;

	void createGui();

public:
	explicit BuddyContactsTable(Buddy buddy, QWidget *parent = 0);
	virtual ~BuddyContactsTable();

};

#endif // BUDDY_CONTACTS_TABLE_H
