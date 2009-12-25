/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"

#include "accounts-combo-box.h"

AccountsComboBox::AccountsComboBox(bool includeSelectAccount, QWidget *parent) :
		QComboBox(parent)
{
	Model = new AccountsModel(this);
	ProxyModel = new AccountsProxyModel(this);
	ProxyModel->setSourceModel(Model);
	
	ActionsProxyModel::ModelActionList accountsModelBeforeActions;
	if (includeSelectAccount)
		accountsModelBeforeActions.append(qMakePair<QString, QString>(tr(" - Select account - "), ""));
	ActionsModel = new ActionsProxyModel(accountsModelBeforeActions,
			ActionsProxyModel::ModelActionList(), this);
	ActionsModel->setSourceModel(ProxyModel);

	setModel(ActionsModel);
}

AccountsComboBox::~AccountsComboBox()
{
}

void AccountsComboBox::setCurrentAccount(Account account)
{
	setCurrentIndex(Model->accountIndex(account));
}

Account AccountsComboBox::currentAccount()
{
	return qvariant_cast<Account>(ActionsModel->index(currentIndex(), 0).data(AccountRole));
}

void AccountsComboBox::addFilter(AbstractAccountFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void AccountsComboBox::removeFilter(AbstractAccountFilter *filter)
{
	ProxyModel->removeFilter(filter);
}
