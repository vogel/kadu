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
#include <QtGui/QListWidgetItem>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"
#include "icons_manager.h"

#include "manage-accounts.h"

ManageAccounts::ManageAccounts(QWidget *parent)
		: QWidget(parent)
{
	createGui();
	loadAccounts();
}

ManageAccounts::~ManageAccounts()
{
}

void ManageAccounts::createGui()
{
	AccountsListWidget = new QListWidget(this);
	AddAccountButton = new QPushButton(tr("Add account..."), this);
	AddAccountButton->setMenu(createGuiAddAccountMenu());

	EditAccountButton = new QPushButton(tr("Edit account..."), this);
	RemoveAccountButton = new QPushButton(tr("Remove account..."), this);
	MoveUpAccountButton = new QPushButton(tr("Move up"), this);
	MoveDownAccountButton = new QPushButton(tr("Move down"), this);

	QVBoxLayout *buttonLayout = new QVBoxLayout();
	buttonLayout->addWidget(AddAccountButton);
	buttonLayout->addWidget(EditAccountButton);
	buttonLayout->addWidget(RemoveAccountButton);
	buttonLayout->addWidget(MoveUpAccountButton);
	buttonLayout->addWidget(MoveDownAccountButton);
	buttonLayout->addStretch(100);

	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->addWidget(AccountsListWidget, 3);
	mainLayout->addItem(buttonLayout);

	setLayout(mainLayout);
}

QMenu * ManageAccounts::createGuiAddAccountMenu()
{
	QMenu *addAccountMenu = new QMenu(this);

	foreach (ProtocolFactory *protocolFactory, ProtocolsManager::instance()->protocolFactories())
	{
		QIcon icon = icons_manager->loadIcon(protocolFactory->iconName());
		QString name = protocolFactory->name();

		QAction *protocolAction = addAccountMenu->addAction(icon, name);
		protocolAction->setData(name);
	}

	return addAccountMenu;
}

void ManageAccounts::loadAccounts()
{
	AccountsListWidget->clear();

	foreach (Account *account, AccountManager::instance()->accounts())
	{
		QListWidgetItem *accountListWidgetItem = new QListWidgetItem();

		accountListWidgetItem->setText(account->name());
		accountListWidgetItem->setIcon(account->protocol()->icon());

		AccountsListWidget->addItem(accountListWidgetItem);
	}
}
