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

#include <QtGui/QWidget>

#include "gui/widgets/account-create-widget.h"
#include "exports.h"

class QComboBox;
class QHBoxLayout;
class QListView;
class QVBoxLayout;

class AccountsModel;

KADUAPI class YourAccounts : public QWidget
{
	Q_OBJECT

	QHBoxLayout *ContentLayout;
	QListView *AccountsView;
	AccountsModel *MyAccountsModel;
	QComboBox *Protocols;
	QWidget *NewAccountContainer;
	QVBoxLayout *MainNewAccountLayout;
	AccountCreateWidget *CurrentNewAccountWidget;

	void createGui();
	void createNewAccountWidget();

private slots:
	void protocolChanged(int protocolIndex);
	void accountCreated(Account *account);

public:
	explicit YourAccounts(QWidget *parent = 0);
	virtual ~YourAccounts();

};

#endif // YOUR_ACCOUNT
