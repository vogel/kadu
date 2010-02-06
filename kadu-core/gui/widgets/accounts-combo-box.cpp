/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/filter/abstract-account-filter.h"
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

	connect(ProxyModel, SIGNAL(filterChanged()), this, SLOT(resetAccount()));

	ActionsModel = new ActionsProxyModel(this);
	if (includeSelectAccount)
		ActionsModel->addBeforeAction(new QAction(tr(" - Select account - "), this));
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
