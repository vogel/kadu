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
#include "accounts/account-manager.h"

#include "accounts/model/accounts-model.h"

#include "gui/windows/configuration-window.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

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
	AccountsListWidget = new QListView(this);
	AccountsListWidget->setModel(new AccountsModel(AccountsListWidget));

	AddAccountButton = new QPushButton(tr("Add account..."), this);
	AddAccountButton->setMenu(createGuiAddAccountMenu());

	EditAccountButton = new QPushButton(tr("Edit account..."), this);
	RemoveAccountButton = new QPushButton(tr("Remove account..."), this);
	MoveUpAccountButton = new QPushButton(tr("Move up"), this);
	MoveDownAccountButton = new QPushButton(tr("Move down"), this);

	connect(EditAccountButton, SIGNAL(clicked()), this, SLOT(editAccount()));
	connect(RemoveAccountButton, SIGNAL(clicked()), this, SLOT(removeAccount()));

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
		QString displayName = protocolFactory->displayName();

		QAction *protocolAction = addAccountMenu->addAction(icon, displayName,
				this, SLOT(addAccount()));
		protocolAction->setData(name);
	}

	return addAccountMenu;
}

void ManageAccounts::loadAccounts()
{/*
	AccountsListWidget->clear();

	foreach (Account *account, AccountManager::instance()->accounts())
	{
		QListWidgetItem *accountListWidgetItem = new QListWidgetItem();

		accountListWidgetItem->setText(account->name());
		accountListWidgetItem->setToolTip(account->uuid());
		accountListWidgetItem->setIcon(account->protocol()->icon());

		AccountsListWidget->addItem(accountListWidgetItem);
	}*/
}

Account * ManageAccounts::currentAccount()
{
	AccountsModel *model = dynamic_cast<AccountsModel *>(AccountsListWidget->model());
	if (0 == model)
		return 0;

	return model->account(AccountsListWidget->currentIndex());
}

void ManageAccounts::addAccount()
{
	QAction *senderAction = dynamic_cast<QAction *>(sender());
	if (0 == senderAction)
		return;

	QString protocolName = senderAction->data().toString();
	if (protocolName.isEmpty())
		return;

	ProtocolFactory *protocolFactory = ProtocolsManager::instance()->byName(protocolName);
	if (0 == protocolFactory)
		return;

	Account *newAccount = protocolFactory->newAccount();
	if (!newAccount)
		return;

	ConfigurationWindow *configurationDialog = protocolFactory->newConfigurationDialog(newAccount, this);
	if (0 == configurationDialog)
	{
		delete newAccount;
		return;
	}

	configurationDialog->setWindowModality(Qt::WindowModal);
	if (configurationDialog->exec() == QDialog::Accepted)
	{
		AccountManager::instance()->registerAccount(newAccount);
		loadAccounts();
		return;
	}
	delete newAccount;
}

void ManageAccounts::removeAccount()
{
	Account *account = currentAccount();
	if (0 == account)
		return;

	AccountManager::instance()->deleteAccount(account);
	loadAccounts();
}

void ManageAccounts::editAccount()
{
	Account *account = currentAccount();
	if (0 == account)
		return;

	Protocol *protocol = account->protocol();
	if (0 == protocol)
		return;

	ProtocolFactory *protocolFactory = protocol->protocolFactory();
	if (0 == protocolFactory)
		return;

	ConfigurationWindow *configurationDialog = protocolFactory->newConfigurationDialog(account, this);
	if (0 == configurationDialog)
		return;

	configurationDialog->setWindowModality(Qt::WindowModal);
	if (QDialog::Accepted == configurationDialog->exec())
		loadAccounts();
}
