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

#include "buddies/buddy.h"

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

	void createGui();

private slots:
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
