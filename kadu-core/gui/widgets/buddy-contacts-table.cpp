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
#include <QHeaderView>

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
	View->setAlternatingRowColors(true);
	View->setDragEnabled(true);
	View->setEditTriggers(QAbstractItemView::AllEditTriggers);
	View->setItemDelegate(Delegate);
	View->setModel(Model);
	View->setVerticalHeader(0);

	View->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	View->horizontalHeader()->setStretchLastSection(true);

	layout->addWidget(View);
}
