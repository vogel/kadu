/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QAction>

#include "accounts/filter/abstract-account-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "model/model-chain.h"
#include "model/roles.h"

#include "accounts-combo-box.h"

AccountsComboBox::AccountsComboBox(bool includeSelectAccount, ActionVisibility visibility, QWidget *parent) :
		ActionsComboBox(parent)
{
	if (includeSelectAccount)
		addBeforeAction(new QAction(tr(" - Select account - "), this), visibility);

	ModelChain *chain = new ModelChain(this);
	Model = new AccountsModel(chain);
	ProxyModel = new AccountsProxyModel(chain);
	chain->setBaseModel(Model);
	chain->addProxyModel(ProxyModel);
	setUpModel(AccountRole, chain);
}

AccountsComboBox::~AccountsComboBox()
{
}

void AccountsComboBox::setCurrentAccount(Account account)
{
	setCurrentValue(account);
}

Account AccountsComboBox::currentAccount()
{
	return currentValue().value<Account>();
}

void AccountsComboBox::setIncludeIdInDisplay(bool includeIdInDisplay)
{
	Model->setIncludeIdInDisplay(includeIdInDisplay);
}

void AccountsComboBox::addFilter(AbstractAccountFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void AccountsComboBox::removeFilter(AbstractAccountFilter *filter)
{
	ProxyModel->removeFilter(filter);
}

#include "moc_accounts-combo-box.cpp"
