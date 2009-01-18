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
}

void ContactsModelProxy::setSourceModel(QAbstractItemModel *sourceModel)
{
	SourceContactModel = dynamic_cast<AbstractContactsModel *>(sourceModel);
	QSortFilterProxyModel::setSourceModel(sourceModel);

	setDynamicSortFilter(true);
	sort(0);
}

bool ContactsModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	if (!SourceContactModel)
		return QSortFilterProxyModel::lessThan(left, right);

	Contact leftContact = SourceContactModel->contact(left);
	Contact rightContact = SourceContactModel->contact(right);

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *leftContactAccountData = leftContact.accountData(account);
	ContactAccountData *rightContactAccountData = rightContact.accountData(account);

	if (!leftContactAccountData && !rightContactAccountData)
		return leftContact.display() < rightContact.display();

	if (!leftContactAccountData)
		return -1;

	if (!rightContactAccountData)
		return 1;

	Status leftStatus = leftContactAccountData->status();
	Status rightStatus = rightContactAccountData->status();

	return 0 == leftStatus.compareTo(rightStatus)
		? leftContact.display() < rightContact.display()
		: leftStatus.compareTo(rightStatus);
}

const QModelIndex ContactsModelProxy::contactIndex(Contact contact) const
{
	if (!SourceContactModel)
		return QModelIndex();

	return mapFromSource(SourceContactModel->contactIndex(contact));
}
