/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QTableView>

#include "gui/widgets/buddy-contacts-table-delegate.h"
#include "gui/widgets/buddy-contacts-table-model.h"

#include "buddy-contacts-table.h"

BuddyContactsTable::BuddyContactsTable(Buddy buddy, QWidget *parent) :
		QWidget(parent)
{
	Delegate = new BuddyContactsTableDelegate(this);
	Model = new BuddyContactsTableModel(buddy, this);

	createGui();
}

BuddyContactsTable::~BuddyContactsTable()
{
}

void BuddyContactsTable::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	View = new QTableView(this);
	View->setItemDelegate(Delegate);
	View->setVerticalHeader(0);
	View->setModel(Model);

	layout->addWidget(View);
}
