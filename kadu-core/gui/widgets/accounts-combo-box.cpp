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

#include "accounts/filter/abstract-account-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "model/roles.h"

#include "accounts-combo-box.h"

AccountsComboBox::AccountsComboBox(bool includeSelectAccount, QWidget *parent) :
		KaduComboBox<Account>(parent), IncludeSelectAccount(includeSelectAccount)
{
	Model = new AccountsModel(this);

	setUpModel(Model, new AccountsProxyModel(this));

	connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
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
	return currentValue();
}

void AccountsComboBox::setIncludeIdInDisplay(bool includeIdInDisplay)
{
	Model->setIncludeIdInDisplay(includeIdInDisplay);
}

void AccountsComboBox::currentIndexChangedSlot(int index)
{
	if (KaduComboBox<Account>::currentIndexChangedSlot(index))
		emit accountChanged(CurrentValue, ValueBeforeChange);
}

void AccountsComboBox::updateValueBeforeChange()
{
	KaduComboBox<Account>::updateValueBeforeChange();
}

void AccountsComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	KaduComboBox<Account>::rowsRemoved(parent, start, end);
}

int AccountsComboBox::preferredDataRole() const
{
	return AccountRole;
}

QString AccountsComboBox::selectString() const
{
	return IncludeSelectAccount ? tr(" - Select account - ") : QString();
}

ActionsProxyModel::ActionVisibility AccountsComboBox::selectVisibility() const
{
	return ActionsProxyModel::NotVisibleWithOneRowSourceModel;
}

void AccountsComboBox::addFilter(AbstractAccountFilter *filter)
{
	static_cast<AccountsProxyModel *>(SourceProxyModel)->addFilter(filter);
}

void AccountsComboBox::removeFilter(AbstractAccountFilter *filter)
{
	static_cast<AccountsProxyModel *>(SourceProxyModel)->removeFilter(filter);
}
