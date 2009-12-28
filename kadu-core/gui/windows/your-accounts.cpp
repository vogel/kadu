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
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/model/accounts-model.h"
#include "gui/widgets/account-add-widget.h"
#include "gui/widgets/account-create-widget.h"
#include "gui/widgets/account-edit-widget.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/model/protocols-model.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

#include "your-accounts.h"

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

	AccountsView = new QListView(this);
	sideLayout->addWidget(AccountsView);
	MyAccountsModel = new AccountsModel(AccountsView);
	AccountsView->setModel(MyAccountsModel);
	AccountsView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	AccountsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	AccountsView->setIconSize(QSize(32, 32));
	connect(AccountsView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(accountSelectionChanged(const QItemSelection &, const QItemSelection &)));
			
	QPushButton *addAccount = new QPushButton(tr("Add Existing Account"), this);
	sideLayout->addWidget(addAccount);
	connect(addAccount, SIGNAL(clicked()), this, SLOT(addAccountClicked()));
	
	QPushButton *createAccount = new QPushButton(tr("Create New Account"), this);
	sideLayout->addWidget(createAccount);
	connect(createAccount, SIGNAL(clicked()), this, SLOT(newAccountClicked()));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Ok"), this);
	QPushButton *addAccountButton = new QPushButton(IconsManager::instance()->loadIcon("ApplyWindowButton"), tr("Add Account"), this);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(okClicked()));
	connect(addAccountButton, SIGNAL(clicked(bool)), this, SLOT(addAccountButtonClicked()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(addAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	CreateEditStack = new QStackedWidget(this);
	contentLayout->addWidget(CreateEditStack, 100);

	createNewAccountWidget();
	createNewAccountCreatedWidget();
	createAddAccountWidget();
	createEditAccountWidget();

	CreateEditStack->setCurrentWidget(NewAccountContainer);
}

void YourAccounts::createNewAccountWidget()
{
	NewAccountContainer = new QWidget(this);
	CreateEditStack->addWidget(NewAccountContainer);

	QVBoxLayout *newAccountLayout = new QVBoxLayout(NewAccountContainer);
	
	QLabel *createAccountLabel = new QLabel(tr("Create New Account"));
	newAccountLayout->addWidget(createAccountLabel);
	
	QGroupBox *selectNetworkGroupbox = new QGroupBox(tr("Choose a network"), NewAccountContainer);
	QGridLayout *selectNetworkLayout = new QGridLayout(selectNetworkGroupbox);

	QLabel *imNetworkLabel = new QLabel(tr("IM Network") + ":", NewAccountContainer);
	selectNetworkLayout->addWidget(imNetworkLabel, 0, 1, 1, 1);

	NewAccountProtocols = new QComboBox(NewAccountContainer);
	ProtocolsModel *model = new ProtocolsModel(NewAccountProtocols);

	ActionsProxyModel::ModelActionList beforeActions;
	beforeActions.append(qMakePair<QString, QString>("", ""));
	ActionsProxyModel *actionsModel = new ActionsProxyModel(beforeActions, ActionsProxyModel::ModelActionList(), NewAccountProtocols);
	actionsModel->setSourceModel(model);

	NewAccountProtocols->setModel(actionsModel);
	selectNetworkLayout->addWidget(NewAccountProtocols, 0, 2, 1, 1);
	
	QLabel *protocolComboLabel = new QLabel(tr("The default network has been selected based on your language settings."));
	selectNetworkLayout->addWidget(protocolComboLabel, 1, 2, 1, 1);
	
	newAccountLayout->addWidget(selectNetworkGroupbox);
	
	QGroupBox *createAccountGroupbox = new QGroupBox(tr("Create a New Account"), NewAccountContainer);
	QGridLayout *createAccountLayout = new QGridLayout(createAccountGroupbox);
	
	CreateStack = new QStackedWidget(createAccountGroupbox);
	createAccountLayout->addWidget(CreateStack, 0, 1, 1, 1);
	
	newAccountLayout->addWidget(createAccountGroupbox, 100, Qt::AlignTop);

	connect(NewAccountProtocols, SIGNAL(activated(int)), this, SLOT(newAccountProtocolChanged(int)));
	newAccountProtocolChanged(0);
}

void YourAccounts::createNewAccountCreatedWidget()
{
	NewAccountCreatedContainer = new QWidget(this);
	CreateEditStack->addWidget(NewAccountCreatedContainer);
	//TODO 0.6.6 some text formatting
	QGridLayout *newAccountCreatedLayout = new QGridLayout(NewAccountCreatedContainer);
	int row = 0;
	
	QLabel *createAccountLabel = new QLabel(tr("Create New Account"));
	newAccountCreatedLayout->addWidget(createAccountLabel, row++, 0, 1, 1);
	
	QLabel *successLabel = new QLabel(tr("Account Added"));
	newAccountCreatedLayout->addWidget(successLabel, row++, 0, 1, 1);
	
	QLabel *successDetailsLabel = new QLabel(tr("Your new account was added successfully and has been added to your account list on the left."));
	newAccountCreatedLayout->addWidget(successDetailsLabel, row++, 0, 1, 1);
	
	newAccountCreatedLayout->setRowStretch(row, 100);
}

void YourAccounts::createAddAccountWidget()
{
	AddAccountContainer = new QWidget(this);
	CreateEditStack->addWidget(AddAccountContainer);

	QVBoxLayout *newAccountLayout = new QVBoxLayout(AddAccountContainer);
	
	QLabel *createAccountLabel = new QLabel(tr("Add Existing Account"));
	newAccountLayout->addWidget(createAccountLabel);
	
	QGroupBox *selectNetworkGroupbox = new QGroupBox(tr("Choose a network"), AddAccountContainer);
	QGridLayout *selectNetworkLayout = new QGridLayout(selectNetworkGroupbox);

	QLabel *imNetworkLabel = new QLabel(tr("IM Network") + ":", AddAccountContainer);
	selectNetworkLayout->addWidget(imNetworkLabel, 0, 1, 1, 1);

	AddAccountProtocols = new QComboBox(AddAccountContainer);
	ProtocolsModel *model = new ProtocolsModel(AddAccountProtocols);

	ActionsProxyModel::ModelActionList beforeActions;
	beforeActions.append(qMakePair<QString, QString>("", ""));
	ActionsProxyModel *actionsModel = new ActionsProxyModel(beforeActions, ActionsProxyModel::ModelActionList(), AddAccountProtocols);
	actionsModel->setSourceModel(model);

	AddAccountProtocols->setModel(actionsModel);
	selectNetworkLayout->addWidget(AddAccountProtocols, 0, 2, 1, 1);
	
	QLabel *protocolComboLabel = new QLabel(tr("The default network has been selected based on your language settings.\nYou can change IM Network"));
	selectNetworkLayout->addWidget(protocolComboLabel, 1, 2, 1, 1);
	
	newAccountLayout->addWidget(selectNetworkGroupbox);
	
	QGroupBox *createAccountGroupbox = new QGroupBox(tr("Setup an Existing Account"), AddAccountContainer);
	QGridLayout *createAccountLayout = new QGridLayout(createAccountGroupbox);
	
	AddStack = new QStackedWidget(createAccountGroupbox);
	createAccountLayout->addWidget(AddStack, 0, 1, 1, 1);
	
	newAccountLayout->addWidget(createAccountGroupbox, 100, Qt::AlignTop);

	connect(AddAccountProtocols, SIGNAL(activated(int)), this, SLOT(addAccountProtocolChanged(int)));
	addAccountProtocolChanged(0);
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

void YourAccounts::addAccountClicked()
{
	AccountsView->selectionModel()->clearSelection();
	CreateEditStack->setCurrentWidget(AddAccountContainer);
}

void YourAccounts::newAccountProtocolChanged(int protocolIndex)
{
	if (protocolIndex < 0 || protocolIndex >= NewAccountProtocols->count())
		return;

	ProtocolFactory *factory = ProtocolsManager::instance()->byName(NewAccountProtocols->itemData(protocolIndex, ProtocolRole).toString());
	AccountCreateWidget *createWidget;

	if (!CreateWidgets.contains(factory))
	{
		if (factory)
			createWidget = factory->newCreateAccountWidget(NewAccountContainer);
		else
			createWidget = 0;

		CreateWidgets[factory] = createWidget;
		if (createWidget)
		{
			connect(createWidget, SIGNAL(accountCreated(Account)), this, SLOT(accountCreated(Account)));
			CreateStack->addWidget(createWidget);
		}
	}
	else
		createWidget = CreateWidgets[factory];

	if (createWidget)
		CreateStack->setCurrentWidget(createWidget);
}

void YourAccounts::addAccountProtocolChanged(int protocolIndex)
{
	if (protocolIndex < 0 || protocolIndex >= AddAccountProtocols->count())
		return;

	ProtocolFactory *factory = ProtocolsManager::instance()->byName(AddAccountProtocols->itemData(protocolIndex, ProtocolRole).toString());
	AccountAddWidget *addWidget;

	if (!AddWidgets.contains(factory))
	{
		if (factory)
			addWidget = factory->newAddAccountWidget(AddAccountContainer);
		else
			addWidget = 0;

		AddWidgets[factory] = addWidget;
		if (addWidget)
		{
			connect(addWidget, SIGNAL(accountCreated(Account)), this, SLOT(accountCreated(Account)));
			AddStack->addWidget(addWidget);
		}
	}
	else
		addWidget = AddWidgets[factory];

	if (addWidget)
		AddStack->setCurrentWidget(addWidget);
}

void YourAccounts::accountCreated(Account account)
{
	account.importProxySettings();
	AccountManager::instance()->addItem(account);
	AccountsView->selectionModel()->clearSelection();
	AccountsView->selectionModel()->select(MyAccountsModel->accountModelIndex(account), QItemSelectionModel::Select);
	CreateEditStack->setCurrentWidget(NewAccountCreatedContainer);
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

	Account account = accountModel->account(current);
	if (account.isNull())
		return;

	AccountEditWidget *editWidget;
	if (!EditWidgets.contains(account))
	{
		editWidget = account.protocolHandler()->protocolFactory()->newEditAccountWidget(account, this);
		EditWidgets[account] = editWidget;
		EditStack->addWidget(editWidget);
	}
	else
		editWidget = EditWidgets[account];

	if (editWidget)
		EditStack->setCurrentWidget(editWidget);
}

void YourAccounts::accountUnregistered(Account account)
{
	if (EditWidgets.contains(account))
	{
		EditStack->removeWidget(EditWidgets[account]);
		EditWidgets[account]->deleteLater();
		EditWidgets.remove(account);
	}
}

void YourAccounts::okClicked()
{
	foreach (AccountEditWidget *editWidget, EditWidgets)
		editWidget->apply();
	close();
}

void YourAccounts::addAccountButtonClicked()
{
  	AccountCreateWidget *widget = dynamic_cast<AccountCreateWidget *>(CreateStack->currentWidget());
	if (!widget)
	{
		AccountAddWidget *widgetAdd = dynamic_cast<AccountAddWidget *>(AddStack->currentWidget());
		if (widgetAdd)
			widgetAdd->apply();
		return;
	}
	widget->apply();
}

void YourAccounts::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
