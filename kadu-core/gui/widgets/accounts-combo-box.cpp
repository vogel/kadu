/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/filter/abstract-account-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "model/roles.h"

#include "accounts-combo-box.h"

AccountsComboBox::AccountsComboBox(bool includeSelectAccount, QWidget *parent) :
		KaduComboBox(parent)
{
	setDataRole(AccountRole);

	Model = new AccountsModel(this);
	ProxyModel = new AccountsProxyModel(this);
	setUpModel(Model, ProxyModel);

	if (includeSelectAccount)
		actionsModel()->addBeforeAction(new QAction(tr(" - Select account - "), this),
		                                ActionsProxyModel::NotVisibleWithOneRowSourceModel);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
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

void AccountsComboBox::currentIndexChangedSlot(int index)
{
	if (KaduComboBox::currentIndexChangedSlot(index))
		emit accountChanged(CurrentValue.value<Account>(), ValueBeforeChange.value<Account>());
}

bool AccountsComboBox::compare(QVariant value, QVariant previousValue) const
{
	return value.value<Account>() == previousValue.value<Account>();
}

void AccountsComboBox::addFilter(AbstractAccountFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void AccountsComboBox::removeFilter(AbstractAccountFilter *filter)
{
	ProxyModel->removeFilter(filter);
}
