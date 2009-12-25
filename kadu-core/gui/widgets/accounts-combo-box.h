/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNTS_COMBO_BOX_H
#define ACCOUNTS_COMBO_BOX_H

#include <QtGui/QComboBox>

class AbstractAccountFilter;
class Account;
class AccountsModel;
class AccountsProxyModel;
class ActionsProxyModel;

class AccountsComboBox : public QComboBox
{
	Q_OBJECT

	AccountsModel *Model;
	AccountsProxyModel *ProxyModel;
	ActionsProxyModel *ActionsModel;

public:
	explicit AccountsComboBox(bool includeSelectAccount = false, QWidget *parent = 0);
	virtual ~AccountsComboBox();

	void setCurrentAccount(Account account);
	Account currentAccount();

	void addFilter(AbstractAccountFilter *filter);
	void removeFilter(AbstractAccountFilter *filter);

};

#endif // ACCOUNTS_COMBO_BOX_H
