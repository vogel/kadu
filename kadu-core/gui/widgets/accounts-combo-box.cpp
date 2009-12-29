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
#include <accounts/filter/abstract-account-filter.h>

AccountsComboBox::AccountsComboBox(bool includeSelectAccount, QWidget *parent) :
		QComboBox(parent)
{
	Model = new AccountsModel(this);
	ProxyModel = new AccountsProxyModel(this);
	ProxyModel->setSourceModel(Model);

	connect(ProxyModel, SIGNAL(filterChanged()), this, SLOT(resetAccount()));

	ActionsProxyModel::ModelActionList accountsModelBeforeActions;
	if (includeSelectAccount)
		accountsModelBeforeActions.append(qMakePair<QString, QString>(tr(" - Select account - "), ""));
	ActionsModel = new ActionsProxyModel(accountsModelBeforeActions,
			ActionsProxyModel::ModelActionList(), this);
	ActionsModel->setSourceModel(ProxyModel);

	setModel(ActionsModel);

	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot(int)));
}

AccountsComboBox::~AccountsComboBox()
{
}

void AccountsComboBox::setCurrentAccount(Account account)
{
	QModelIndex index = Model->accountModelIndex(account);
	index = ProxyModel->mapFromSource(index);
	index = ActionsModel->mapFromSource(index);

	if (index.row() < 0 || index.row() >= count())
		setCurrentIndex(0);
	else
		setCurrentIndex(index.row());

	CurrentAccount = account;
}

Account AccountsComboBox::currentAccount()
{
	CurrentAccount = qvariant_cast<Account>(ActionsModel->index(currentIndex(), 0).data(AccountRole));
	return CurrentAccount;
}

void AccountsComboBox::addFilter(AbstractAccountFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void AccountsComboBox::removeFilter(AbstractAccountFilter *filter)
{
	ProxyModel->removeFilter(filter);
}

void AccountsComboBox::activatedSlot(int index)
{
	currentAccount(); // sets CurrentAccount variable
}

void AccountsComboBox::resetAccount()
{
	setCurrentAccount(CurrentAccount);
}
