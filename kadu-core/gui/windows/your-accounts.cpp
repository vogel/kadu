/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#ifdef Q_WS_MAEMO_5
#include <QtGui/QScrollArea>
#endif

#include "accounts/account-manager.h"
#include "accounts/model/accounts-model.h"
#include "gui/widgets/account-add-widget.h"
#include "gui/widgets/account-create-widget.h"
#include "gui/widgets/account-edit-widget.h"
#include "gui/widgets/modal-configuration-widget.h"
#include "gui/widgets/protocols-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "protocols/filter/can-register-protocol-filter.h"

#include "activate.h"
#include "icons-manager.h"

#include "your-accounts.h"

YourAccounts *YourAccounts::Instance = 0;

YourAccounts::YourAccounts(QWidget *parent) :
		QWidget(parent), CurrentWidget(0), IsCurrentWidgetEditAccount(false), ForceWidgetChange(false), CanRegisterFilter(new CanRegisterProtocolFilter())
{
	setWindowRole("kadu-your-accounts");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Your accounts"));

	createGui();
	AccountsView->selectionModel()->select(AccountsView->model()->index(0, 0), QItemSelectionModel::ClearAndSelect);

	loadWindowGeometry(this, "General", "YourAccountsWindowGeometry", 0, 50, 700, 500);

	CanRegisterFilter->setEnabled(true);
}

YourAccounts::~YourAccounts()
{
	saveWindowGeometry(this, "General", "YourAccountsWindowGeometry");

	Instance = 0;
}

YourAccounts * YourAccounts::instance()
{
	if (!Instance)
		Instance = new YourAccounts();

	return Instance;
}

void YourAccounts::show()
{
	QWidget::show();

	_activateWindow(this);
}

void YourAccounts::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QHBoxLayout *contentLayout = new QHBoxLayout();
	mainLayout->addItem(contentLayout);

	AccountsView = new QListView(this);
	AccountsView->setMinimumWidth(150);
	contentLayout->addWidget(AccountsView);
	MyAccountsModel = new AccountsModel(AccountsView);

	QAction *separator = new QAction(this);
	separator->setSeparator(true);

	AddExistingAccountAction = new QAction(IconsManager::instance()->iconByPath("contact-new"), tr("Add existing account"), this);
	CreateNewAccountAction = new QAction(IconsManager::instance()->iconByPath("system-users"), tr("Create new account"), this);

	ActionsProxyModel* actionsModel = new ActionsProxyModel(this);
	actionsModel->addAfterAction(separator, ActionsProxyModel::NotVisibleWithEmptySourceModel);
	actionsModel->addAfterAction(AddExistingAccountAction);
	actionsModel->addAfterAction(CreateNewAccountAction);
	actionsModel->setSourceModel(MyAccountsModel);

	AccountsView->setModel(actionsModel);
	AccountsView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	AccountsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	AccountsView->setIconSize(QSize(32, 32));
	connect(AccountsView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(accountSelectionChanged(const QItemSelection &, const QItemSelection &)));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Close"), this);

	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	MainStack = new QStackedWidget(this);

#ifdef Q_WS_MAEMO_5
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setFrameStyle(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setWidget(MainStack);
	scrollArea->setWidgetResizable(true);
	contentLayout->addWidget(scrollArea, 100);
#else
	contentLayout->addWidget(MainStack, 100);
#endif

	createAccountWidget();
	createEditAccountWidget();
}

void YourAccounts::switchToCreateMode()
{
#ifndef Q_WS_MAEMO_5
	MainAccountLabel->setText(tr("<font size='+2'><b>Create New Account</b></font>"));
	MainAccountGroupBox->setTitle(tr("Create New Account"));
#endif
	Protocols->addFilter(CanRegisterFilter);
}

void YourAccounts::switchToAddMode()
{
#ifndef Q_WS_MAEMO_5
	MainAccountLabel->setText(tr("<font size='+2'><b>Add Existing Account</b></font>"));
	MainAccountGroupBox->setTitle(tr("Setup an Existing Account"));
#endif
	Protocols->removeFilter(CanRegisterFilter);
}

void YourAccounts::createAccountWidget()
{
	CreateAddAccountContainer = new QWidget(this);
	CreateAddAccountContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	MainStack->addWidget(CreateAddAccountContainer);
	MainStack->setCurrentWidget(CreateAddAccountContainer);

	QVBoxLayout *newAccountLayout = new QVBoxLayout(CreateAddAccountContainer);

	MainAccountLabel = new QLabel();
	newAccountLayout->addWidget(MainAccountLabel);

	QGroupBox *selectNetworkGroupbox = new QGroupBox(tr("Choose a network"), CreateAddAccountContainer);
	selectNetworkGroupbox->setFlat(true);
#ifdef Q_WS_MAEMO_5
	selectNetworkGroupbox->setTitle("");
#endif
	QFormLayout *selectNetworkLayout = new QFormLayout(selectNetworkGroupbox);

	QLabel *imNetworkLabel = new QLabel(tr("IM Network") + ':', CreateAddAccountContainer);
	Protocols = new ProtocolsComboBox(CreateAddAccountContainer);
	selectNetworkLayout->addRow(imNetworkLabel, Protocols);

//#ifndef Q_WS_MAEMO_5
//	QLabel *protocolComboLabel = new QLabel(tr("<font size='-1'><i>The default network has been selected based on your language settings.</i></font>"));
//	selectNetworkLayout->addRow(0, protocolComboLabel);
//#endif

	newAccountLayout->addWidget(selectNetworkGroupbox);

	MainAccountGroupBox = new QGroupBox(CreateAddAccountContainer);
	MainAccountGroupBox->setFlat(true);

	QGridLayout *createAccountLayout = new QGridLayout(MainAccountGroupBox);

	CreateAddStack = new QStackedWidget(MainAccountGroupBox);
	createAccountLayout->addWidget(CreateAddStack, 0, 1, 1, 1);

	newAccountLayout->addWidget(MainAccountGroupBox, Qt::AlignTop);

	connect(Protocols, SIGNAL(protocolChanged(ProtocolFactory*, ProtocolFactory*)),
			this, SLOT(protocolChanged(ProtocolFactory*, ProtocolFactory*)));

	switchToCreateMode();
}

void YourAccounts::createEditAccountWidget()
{
	EditStack = new QStackedWidget(this);
	EditStack->setContentsMargins(5, 5, 5, 5);
	MainStack->addWidget(EditStack);
}

AccountCreateWidget * YourAccounts::getAccountCreateWidget(ProtocolFactory *protocol)
{
	AccountCreateWidget *widget = 0;

	if (!CreateWidgets.contains(protocol))
	{
		if (protocol)
			widget = protocol->newCreateAccountWidget(CreateAddStack);

		CreateWidgets[protocol] = widget;
		if (widget)
		{
			connect(widget, SIGNAL(accountCreated(Account)), this, SLOT(accountCreated(Account)));
			connect(widget, SIGNAL(cancelled()), this, SLOT(resetProtocol()));
			CreateAddStack->addWidget(widget);
		}
	}
	else
		widget = CreateWidgets[protocol];

	return widget;
}

AccountAddWidget * YourAccounts::getAccountAddWidget(ProtocolFactory *protocol)
{
	AccountAddWidget *widget = 0;

	if (!AddWidgets.contains(protocol))
	{
		if (protocol)
			widget = protocol->newAddAccountWidget(CreateAddStack);

		AddWidgets[protocol] = widget;
		if (widget)
		{
			connect(widget, SIGNAL(accountCreated(Account)), this, SLOT(accountCreated(Account)));
			CreateAddStack->addWidget(widget);
		}
	}
	else
		widget = AddWidgets[protocol];

	return widget;
}

AccountEditWidget * YourAccounts::getAccountEditWidget(Account account)
{
	AccountEditWidget *editWidget;
	if (!EditWidgets.contains(account))
	{
		editWidget = account.protocolHandler()->protocolFactory()->newEditAccountWidget(account, this);
		EditWidgets[account] = editWidget;
		EditStack->addWidget(editWidget);
	}
	else
		editWidget = EditWidgets[account];

	return editWidget;
}

void YourAccounts::protocolChanged(ProtocolFactory *protocolFactory, ProtocolFactory *lastProtocolFactory)
{
	Q_UNUSED(protocolFactory)

	if (canChangeWidget())
	{
		updateCurrentWidget();
		return;
	}

	ForceWidgetChange = true;
	Protocols->setCurrentProtocol(lastProtocolFactory);
	ForceWidgetChange = false;
}

void YourAccounts::resetProtocol()
{
	ForceWidgetChange = true;
	Protocols->setCurrentProtocol(0);
	ForceWidgetChange = false;
}

void YourAccounts::updateCurrentWidget()
{
	QWidget *widget = 0;

	QModelIndexList selection = AccountsView->selectionModel()->selectedIndexes();
	if (1 != selection.size())
		return;

	QAction *action = selection[0].data(ActionRole).value<QAction *>();
	if (!action)
	{
		MainStack->setCurrentWidget(EditStack);
		Account account = selection[0].data(AccountRole).value<Account>();
		if (account)
		{
			EditStack->setCurrentWidget(getAccountEditWidget(account));
			CurrentWidget = getAccountEditWidget(account);
			IsCurrentWidgetEditAccount = true;
		}

		return;
	}

	IsCurrentWidgetEditAccount = false;

	MainStack->setCurrentWidget(CreateAddAccountContainer);
	if (action == CreateNewAccountAction)
	{
		widget = getAccountCreateWidget(Protocols->currentProtocol());
		switchToCreateMode();
	}
	else if (action == AddExistingAccountAction)
	{
		widget = getAccountAddWidget(Protocols->currentProtocol());
		switchToAddMode();
	}

	CreateAddStack->setVisible(0 != widget);
	if (widget)
		CreateAddStack->setCurrentWidget(widget);

	CurrentWidget = dynamic_cast<ModalConfigurationWidget *>(widget);
}

bool YourAccounts::canChangeWidget()
{
	if (ForceWidgetChange)
		return true;

	if (!CurrentWidget)
		return true;

	if (StateNotChanged == CurrentWidget->state())
		return true;

	if (!IsCurrentWidgetEditAccount)
	{
		QMessageBox::StandardButton result = QMessageBox::question(this, tr("Account"),
				tr("You have unsaved changes in current account.<br />Do you want to return to editing?"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		switch (result)
		{
			case QMessageBox::Yes:
				return false;

			case QMessageBox::No:
				CurrentWidget->cancel();
				return true;

			default:
				return false;
		}
	}

	if (StateChangedDataValid == CurrentWidget->state())
	{
		QMessageBox::StandardButton result = QMessageBox::question(this, tr("Account"),
				tr("You have unsaved changes in current account.<br />Do you want to save them?"),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

		switch (result)
		{
			case QMessageBox::Yes:
				CurrentWidget->apply();
				return true;

			case QMessageBox::No:
				CurrentWidget->cancel();
				return true;

			default:
				return false;
		}
	}

	if (StateChangedDataInvalid == CurrentWidget->state())
	{
		QMessageBox::StandardButton result = QMessageBox::question(this, tr("Account"),
				tr("You have unsaved changes in current account.<br />This data is invalid, so you will loose all changes.<br />Do you want to go back to edit them?"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		switch (result)
		{
			case QMessageBox::Yes:
				return false;

			case QMessageBox::No:
				CurrentWidget->cancel();
				return true;

			default:
				return false;
		}
	}

	return true;
}

void YourAccounts::accountCreated(Account account)
{
	account.importProxySettings();
	AccountManager::instance()->addItem(account);

	ConfigurationManager::instance()->flush();
	selectAccount(account);
}

void YourAccounts::selectAccount(Account account)
{
	AccountsView->selectionModel()->clearSelection();
	AccountsView->selectionModel()->select(MyAccountsModel->indexForValue(account), QItemSelectionModel::Select);
}

void YourAccounts::accountSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	Q_UNUSED(selected)
	Q_UNUSED(deselected)

	if (canChangeWidget())
	{
		updateCurrentWidget();
		return;
	}

	ForceWidgetChange = true;
	AccountsView->selectionModel()->select(deselected, QItemSelectionModel::ClearAndSelect);
	ForceWidgetChange = false;
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

void YourAccounts::closeEvent(QCloseEvent *e)
{
	if (canChangeWidget())
		e->accept();
	else
		e->ignore();
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
