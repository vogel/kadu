 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account_manager.h"

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"

#include "protocols/status.h"

#include "contacts-model.h"

#include "contacts-model-proxy.h"

ContactsModelProxy::ContactsModelProxy(QObject *parent)
	: QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");
}

void ContactsModelProxy::setSourceModel(QAbstractItemModel *sourceModel)
{
	SourceContactModel = dynamic_cast<AbstractContactsModel *>(sourceModel);
	QSortFilterProxyModel::setSourceModel(sourceModel);

	setDynamicSortFilter(true);
	sort(0);
}

int ContactsModelProxy::compareNames(QString n1, QString n2) const
{
	return BrokenStringCompare
		? n1.toLower().localeAwareCompare(n2.toLower())
		: n1.localeAwareCompare(n2);
}

bool ContactsModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	if (!SourceContactModel)
		return QSortFilterProxyModel::lessThan(left, right);

	Contact leftContact = SourceContactModel->contact(left);
	Contact rightContact = SourceContactModel->contact(right);

	Account *leftAccount = leftContact.prefferedAccount();
	Account *rightAccount = rightContact.prefferedAccount();

	ContactAccountData *leftContactAccountData = leftContact.accountData(leftAccount);
	ContactAccountData *rightContactAccountData = rightContact.accountData(rightAccount);

	Status leftStatus = leftContactAccountData
		? leftContactAccountData->status()
		: Status();
	Status rightStatus = rightContactAccountData
		? rightContactAccountData->status()
		: Status();

	int statusCompare = leftStatus.compareTo(rightStatus);
	if (statusCompare < 0)
		return true;
	if (statusCompare > 0)
		return false;
	
	int displayCompare = compareNames(leftContact.display(), rightContact.display());
	return displayCompare < 0;
}

bool ContactsModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	Contact contact = SourceContactModel->contact(sourceModel()->index(sourceRow, 0));
	foreach (AbstractContactFilter *filter, Filters)
		if (!filter->acceptContact(contact))
			return false;

	return true;
}

void ContactsModelProxy::addFilter(AbstractContactFilter *filter)
{
	Filters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void ContactsModelProxy::removeFilter(AbstractContactFilter *filter)
{
	Filters.removeAll(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

Contact ContactsModelProxy::contact(const QModelIndex &index) const
{
	if (!SourceContactModel)
		return Contact::null;

	return SourceContactModel->contact(mapToSource(index));
}

const QModelIndex ContactsModelProxy::contactIndex(Contact contact) const
{
	if (!SourceContactModel)
		return QModelIndex();

	return mapFromSource(SourceContactModel->contactIndex(contact));
}
