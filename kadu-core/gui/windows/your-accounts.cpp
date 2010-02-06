/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QAction>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
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
#include "gui/widgets/protocols-combo-box.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
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

	AccountsView = new QListView(this);
	contentLayout->addWidget(AccountsView);
// 	contentLayout->setStretchFactor(AccountsView, 1);
	MyAccountsModel = new AccountsModel(AccountsView);

	ActionsProxyModel::ModelActionList beforeActions;
	ActionsProxyModel::ModelActionList afterActions;
	AddExistingAccountAction = new QAction(tr("Add existing account"), this);
	CreateNewAccountAction = new QAction(tr("Create new account"), this);
	afterActions.append(AddExistingAccountAction);
	afterActions.append(CreateNewAccountAction);

	ActionsModel = new ActionsProxyModel(beforeActions, afterActions, this);
	ActionsModel->setSourceModel(MyAccountsModel);

	AccountsView->setModel(ActionsModel);
	AccountsView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	AccountsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	AccountsView->setIconSize(QSize(32, 32));
	connect(AccountsView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(accountSelectionChanged(const QItemSelection &, const QItemSelection &)));

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
	
	QLabel *createAccountLabel = new QLabel(tr("<font size='+2'><b>Create New Account</b></font>"));
	newAccountLayout->addWidget(createAccountLabel);
	
	QGroupBox *selectNetworkGroupbox = new QGroupBox(tr("Choose a network"), NewAccountContainer);
	selectNetworkGroupbox->setFlat(true);

	QFormLayout *selectNetworkLayout = new QFormLayout(selectNetworkGroupbox);

	QLabel *imNetworkLabel = new QLabel(tr("IM Network") + ":", NewAccountContainer);
	NewAccountProtocols = new ProtocolsComboBox(true, NewAccountContainer);
	selectNetworkLayout->addRow(imNetworkLabel, NewAccountProtocols);
	
	QLabel *protocolComboLabel = new QLabel(tr("<font size='-1'><i>The default network has been selected based on your language settings.</i></font>"));
	selectNetworkLayout->addRow(0, protocolComboLabel);
	
	newAccountLayout->addWidget(selectNetworkGroupbox);

	QGroupBox *createAccountGroupbox = new QGroupBox(tr("Create a New Account"), NewAccountContainer);
	createAccountGroupbox->setFlat(true);

	QGridLayout *createAccountLayout = new QGridLayout(createAccountGroupbox);
	
	CreateStack = new QStackedWidget(createAccountGroupbox);
	createAccountLayout->addWidget(CreateStack, 0, 1, 1, 1);
	
	newAccountLayout->addWidget(createAccountGroupbox, 100, Qt::AlignTop);

	connect(NewAccountProtocols, SIGNAL(protocolChanged(ProtocolFactory*)), this, SLOT(newAccountProtocolChanged(ProtocolFactory*)));
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

	AddAccountProtocols = new ProtocolsComboBox(true, AddAccountContainer);
	selectNetworkLayout->addWidget(AddAccountProtocols, 0, 2, 1, 1);
	
	QLabel *protocolComboLabel = new QLabel(tr("The default network has been selected based on your language settings.\nYou can change IM Network"));
	selectNetworkLayout->addWidget(protocolComboLabel, 1, 2, 1, 1);
	
	newAccountLayout->addWidget(selectNetworkGroupbox);
	
	QGroupBox *createAccountGroupbox = new QGroupBox(tr("Setup an Existing Account"), AddAccountContainer);
	QGridLayout *createAccountLayout = new QGridLayout(createAccountGroupbox);
	
	AddStack = new QStackedWidget(createAccountGroupbox);
	createAccountLayout->addWidget(AddStack, 0, 1, 1, 1);
	
	newAccountLayout->addWidget(createAccountGroupbox, 100, Qt::AlignTop);

	connect(AddAccountProtocols, SIGNAL(protocolChanged(ProtocolFactory*)), this, SLOT(addAccountProtocolChanged(ProtocolFactory*)));
	addAccountProtocolChanged(0);
}

void YourAccounts::createEditAccountWidget()
{
	EditStack = new QStackedWidget(this);
	EditStack->setContentsMargins(5, 5, 5, 5);
	CreateEditStack->addWidget(EditStack);
}

void YourAccounts::newAccountProtocolChanged(ProtocolFactory *protocolFactory)
{
	AccountCreateWidget *createWidget = 0;

	if (!CreateWidgets.contains(protocolFactory))
	{
		if (protocolFactory)
			createWidget = protocolFactory->newCreateAccountWidget(CreateStack);

		CreateWidgets[protocolFactory] = createWidget;
		if (createWidget)
		{
			connect(createWidget, SIGNAL(accountCreated(Account)), this, SLOT(accountCreated(Account)));
			CreateStack->addWidget(createWidget);
		}
	}
	else
		createWidget = CreateWidgets[protocolFactory];

	CreateStack->setVisible(0 != createWidget);
	if (createWidget)
		CreateStack->setCurrentWidget(createWidget);
}

void YourAccounts::addAccountProtocolChanged(ProtocolFactory *protocolFactory)
{
	AccountAddWidget *addWidget = 0;

	if (!AddWidgets.contains(protocolFactory))
	{
		if (protocolFactory)
			addWidget = protocolFactory->newAddAccountWidget(AddStack);

		AddWidgets[protocolFactory] = addWidget;
		if (addWidget)
		{
			connect(addWidget, SIGNAL(accountCreated(Account)), this, SLOT(accountCreated(Account)));
			AddStack->addWidget(addWidget);
		}
	}
	else
		addWidget = AddWidgets[protocolFactory];

	AddStack->setVisible(0 != addWidget);
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

	QAction *action = qvariant_cast<QAction *>(current.data(ActionRole));
	if (action == AddExistingAccountAction)
	{
		CreateEditStack->setCurrentWidget(AddAccountContainer);
		return;
	}

	if (action == CreateNewAccountAction)
	{
		CreateEditStack->setCurrentWidget(NewAccountContainer);
		return;
	}

	Account account = qvariant_cast<Account>(current.data(AccountRole));
	if (!account)
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
	if (CreateEditStack->currentWidget() == NewAccountContainer)
	{
		AccountCreateWidget *widget = dynamic_cast<AccountCreateWidget *>(CreateStack->currentWidget());
		if (widget)
			widget->apply();
	}
	else
	{
		AccountAddWidget *widgetAdd = dynamic_cast<AccountAddWidget *>(AddStack->currentWidget());
		if (widgetAdd)
			widgetAdd->apply();
	}
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
