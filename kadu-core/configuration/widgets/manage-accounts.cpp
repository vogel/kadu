/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "manage-accounts.h"

ManageAccounts::ManageAccounts(QWidget *parent)
{
	AccountsListWidget = new QListWidget(this);
	AddAccountButton = new QPushButton(tr("Add account..."), this);
	RemoveAccountButton = new QPushButton(tr("Remove account..."), this);
	MoveUpAccountButton = new QPushButton(tr("Move up"), this);
	MoveDownAccountButton = new QPushButton(tr("Move down"), this);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(AddAccountButton);
	buttonLayout->addWidget(RemoveAccountButton);
	buttonLayout->addWidget(MoveUpAccountButton);
	buttonLayout->addWidget(MoveDownAccountButton);
	buttonLayout->addStretch(100);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(AccountsListWidget, 3);
	mainLayout->addItem(buttonLayout);
}

ManageAccounts::~ManageAccounts()
{
}
