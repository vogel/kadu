/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/filter/abstract-account-filter.h"
#include "model/roles.h"

#include "accounts-proxy-model.h"

AccountsProxyModel::AccountsProxyModel(QObject *parent) :
		QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");
}

AccountsProxyModel::~AccountsProxyModel()
{
}

void AccountsProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
	QSortFilterProxyModel::setSourceModel(sourceModel);

	setDynamicSortFilter(true);
	sort(0);
}

int AccountsProxyModel::compareNames(QString n1, QString n2) const
{
	return BrokenStringCompare
			? n1.toLower().localeAwareCompare(n2.toLower())
			: n1.localeAwareCompare(n2);
}

bool AccountsProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	Account leftAccount = left.data(AccountRole).value<Account>();
	Account rightAccount = right.data(AccountRole).value<Account>();

	if (leftAccount.isNull())
		return false;
	if (rightAccount.isNull())
		return true;

	int displayCompare = compareNames(leftAccount.accountIdentity().name(), rightAccount.accountIdentity().name());
	return displayCompare < 0;
}

bool AccountsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if (sourceParent.isValid())
		return true;

	Account account = sourceModel()->index(sourceRow, 0).data(AccountRole).value<Account>();;
	foreach (AbstractAccountFilter *filter, Filters)
		if (!filter->acceptAccount(account))
			return false;

		return true;
}

void AccountsProxyModel::addFilter(AbstractAccountFilter *filter)
{
	Filters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(filterChangedSlot()));
}

void AccountsProxyModel::removeFilter(AbstractAccountFilter *filter)
{
	Filters.removeAll(filter);
	invalidateFilter();
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(filterChangedSlot()));
}

void AccountsProxyModel::filterChangedSlot()
{
	invalidateFilter();
	emit filterChanged();
}
