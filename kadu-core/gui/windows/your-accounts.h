/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef YOUR_ACCOUNTS
#define YOUR_ACCOUNTS

#include <QtCore/QModelIndex>
#include <QtGui/QItemSelection>
#include <QtGui/QWidget>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "exports.h"

class QComboBox;
class QHBoxLayout;
class QListView;
class QStackedWidget;
class QVBoxLayout;

class AccountCreateWidget;
class AccountEditWidget;
class AccountsModel;
class ProtocolFactory;

KADUAPI class YourAccounts : public QWidget, AccountsAwareObject
{
	Q_OBJECT

	QListView *AccountsView;
	AccountsModel *MyAccountsModel;

	QStackedWidget *CreateEditStack;
	QStackedWidget *CreateStack;
	QStackedWidget *EditStack;

	QComboBox *Protocols;
	QWidget *NewAccountContainer;

	QMap<ProtocolFactory *, AccountCreateWidget *> CreateWidgets;
	QMap<Account, AccountEditWidget *> EditWidgets;

	void createGui();
	void createNewAccountWidget();
	void createEditAccountWidget();

private slots:
	void newAccountClicked();
	void protocolChanged(int protocolIndex);
	void accountCreated(Account account);
	void accountSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

	void okClicked();
	void applyClicked();

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void accountRegistered(Account account) { }
	virtual void accountUnregistered(Account account);

public:
	explicit YourAccounts(QWidget *parent = 0);
	virtual ~YourAccounts();

};

#endif // YOUR_ACCOUNT
