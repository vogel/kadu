/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/model/accounts-model.h"
#include "contacts/model/contacts-model-base.h"
#include "gui/widgets/account-create-widget.h"
#include "gui/widgets/account-edit-widget.h"
#include "misc/misc.h"
#include "protocols/model/protocols-model.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

#include "your-accounts.h"
#include <protocols/protocol.h>

YourAccounts::YourAccounts(QWidget *parent) :
		QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Your accounts"));

	createGui();

	loadWindowGeometry(this, "General", "YourAccountsWindowGeometry", 0, 50, 425, 500);
}

YourAccounts::~YourAccounts()
{
	saveWindowGeometry(this, "General", "YourAccountsWindowGeometry");
}

void YourAccounts::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QHBoxLayout *contentLayout = new QHBoxLayout;
	mainLayout->addItem(contentLayout);

	QVBoxLayout *sideLayout = new QVBoxLayout;
	contentLayout->addItem(sideLayout);
	contentLayout->setStretchFactor(sideLayout, 1);

	QPushButton *newAccount = new QPushButton(tr("New account"), this);
	sideLayout->addWidget(newAccount);
	connect(newAccount, SIGNAL(clicked()), this, SLOT(newAccountClicked()));

	AccountsView = new QListView(this);
	sideLayout->addWidget(AccountsView);
	MyAccountsModel = new AccountsModel(AccountsView);
	AccountsView->setModel(MyAccountsModel);
	AccountsView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	AccountsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	AccountsView->setIconSize(QSize(32, 32));
	connect(AccountsView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(accountSelectionChanged(const QItemSelection &, const QItemSelection &)));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Ok"), this);
	QPushButton *applyButton = new QPushButton(IconsManager::instance()->loadIcon("ApplyWindowButton"), tr("Apply"), this);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(okClicked()));
	connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(applyClicked()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(applyButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	CreateEditStack = new QStackedWidget(this);
	contentLayout->addWidget(CreateEditStack, 100);

	createNewAccountWidget();
	createEditAccountWidget();

	CreateEditStack->setCurrentWidget(NewAccountContainer);
}

void YourAccounts::createNewAccountWidget()
{
	NewAccountContainer = new QWidget(this);
	CreateEditStack->addWidget(NewAccountContainer);

	QVBoxLayout *newAccountLayout = new QVBoxLayout(NewAccountContainer);

	QHBoxLayout *selectProtocolLayout = new QHBoxLayout;
	newAccountLayout->addItem(selectProtocolLayout);
	newAccountLayout->setStretchFactor(selectProtocolLayout, 1);

	selectProtocolLayout->addWidget(new QLabel(tr("Select network") + ":", NewAccountContainer));
	selectProtocolLayout->addSpacing(20);

	Protocols = new QComboBox(NewAccountContainer);
	ProtocolsModel *Model = new ProtocolsModel(tr("Choose a network to add"), Protocols);
	Protocols->setModel(Model);
	selectProtocolLayout->addWidget(Protocols, 10);

	CreateStack = new QStackedWidget(this);
	newAccountLayout->addWidget(CreateStack, 100, Qt::AlignTop);

	connect(Protocols, SIGNAL(activated(int)), this, SLOT(protocolChanged(int)));
	protocolChanged(0);
}

void YourAccounts::createEditAccountWidget()
{
	EditStack = new QStackedWidget(this);
	EditStack->setContentsMargins(5, 5, 5, 5);
	CreateEditStack->addWidget(EditStack);
}

void YourAccounts::newAccountClicked()
{
	AccountsView->selectionModel()->clearSelection();
	CreateEditStack->setCurrentWidget(NewAccountContainer);
}

void YourAccounts::protocolChanged(int protocolIndex)
{
	if (protocolIndex < 0 || protocolIndex >= Protocols->count())
		return;

	ProtocolFactory *factory = ProtocolsManager::instance()->byName(Protocols->itemData(protocolIndex, ProtocolRole).toString());
	AccountCreateWidget *createWidget;

	if (!CreateWidgets.contains(factory))
	{
		if (factory)
			createWidget = factory->newCreateAccountWidget(NewAccountContainer);
		else
			createWidget = new AccountCreateWidget(this);

		CreateWidgets[factory] = createWidget;
		if (createWidget)
		{
			connect(createWidget, SIGNAL(accountCreated(Account *)), this, SLOT(accountCreated(Account *)));
			CreateStack->addWidget(createWidget);
		}
	}
	else
		createWidget = CreateWidgets[factory];

	if (createWidget)
		CreateStack->setCurrentWidget(createWidget);
}

void YourAccounts::accountCreated(Account *account)
{
	account->importProxySettings();
	AccountManager::instance()->registerAccount(account);
	AccountsView->selectionModel()->clearSelection();
	AccountsView->selectionModel()->select(MyAccountsModel->accountModelIndex(account), QItemSelectionModel::Select);
}

void YourAccounts::accountSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	if (1 != selected.indexes().count())
		return;

	QModelIndex current = selected.indexes().first();

	CreateEditStack->setCurrentWidget(EditStack);
	const AccountsModel *accountModel = dynamic_cast<const AccountsModel *>(current.model());
	if (!accountModel)
		return;

	Account *account = accountModel->account(current);
	if (!account)
		return;

	AccountEditWidget *editWidget;
	if (!EditWidgets.contains(account))
	{
		editWidget = account->protocol()->protocolFactory()->newEditAccountWidget(account, this);
		EditWidgets[account] = editWidget;
		EditStack->addWidget(editWidget);
	}
	else
		editWidget = EditWidgets[account];

	if (editWidget)
		EditStack->setCurrentWidget(editWidget);
}

void YourAccounts::accountUnregistered(Account *account)
{
	if (EditWidgets.contains(account))
	{
		EditStack->removeWidget(EditWidgets[account]);
		EditWidgets.remove(account);
	}
}

void YourAccounts::okClicked()
{
	applyClicked();
	close();
}

void YourAccounts::applyClicked()
{
	foreach (AccountEditWidget *editWidget, EditWidgets)
		editWidget->apply();
}
