 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

	int displayCompare = compareNames(leftAccount.name(), rightAccount.name());
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
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void AccountsProxyModel::removeFilter(AbstractAccountFilter *filter)
{
	Filters.removeAll(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}
