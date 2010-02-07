/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef YOUR_ACCOUNTS
#define YOUR_ACCOUNTS

#include <QtCore/QModelIndex>
#include <QtGui/QItemSelection>
#include <QtGui/QWidget>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "exports.h"

class QHBoxLayout;
class QGroupBox;
class QLabel;
class QListView;
class QStackedWidget;
class QVBoxLayout;

class AccountAddWidget;
class AccountCreateWidget;
class AccountEditWidget;
class AccountsModel;
class ActionsProxyModel;
class ProtocolFactory;
class ProtocolsComboBox;

KADUAPI class YourAccounts : public QWidget, AccountsAwareObject
{
	Q_OBJECT

	QListView *AccountsView;
	AccountsModel *MyAccountsModel;
	ActionsProxyModel *ActionsModel;
	QAction *AddExistingAccountAction;
	QAction *CreateNewAccountAction;

	QLabel *MainAccountLabel;
	QGroupBox *MainAccountGroupBox;

	QStackedWidget *MainStack;
	QStackedWidget *CreateAddStack;
	QStackedWidget *EditStack;

	ProtocolsComboBox *Protocols;
	QWidget *CreateAddAccountContainer;

	QMap<ProtocolFactory *, AccountCreateWidget *> CreateWidgets;
	QMap<ProtocolFactory *, AccountAddWidget *> AddWidgets;
	QMap<Account, AccountEditWidget *> EditWidgets;

	void createGui();
	void createAccountWidget();
	void createEditAccountWidget();

	AccountCreateWidget * getAccountCreateWidget(ProtocolFactory *protocol);
	AccountAddWidget * getAccountAddWidget(ProtocolFactory *protocol);
	AccountEditWidget * getAccountEditWidget(Account account);

	void switchToCreateMode();
	void switchToAddMode();

	void selectAccount(Account account);

	void updateCurrentWidget();

private slots:
	void protocolChanged(ProtocolFactory *protocolFactory);

	void accountCreated(Account account);
	void accountSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

	void okClicked();

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void accountRegistered(Account account) { }
	virtual void accountUnregistered(Account account);

public:
	explicit YourAccounts(QWidget *parent = 0);
	virtual ~YourAccounts();

};

#endif // YOUR_ACCOUNT
