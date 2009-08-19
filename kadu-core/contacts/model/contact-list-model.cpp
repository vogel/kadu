 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-list-mime-data-helper.h"

#include "protocols/protocol.h"

#include "contact-list-model.h"

ContactListModel::ContactListModel(ContactList list, QObject *parent)
	: ContactsModelBase(parent), List(list)
{
	triggerAllAccountsRegistered();
}

int ContactListModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid()
		? ContactsModelBase::rowCount(parent)
		: List.count();
}

Contact ContactListModel::contact(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	return parent.isValid()
		? List.at(parent.row())
		: index.isValid()
			? List.at(index.row())
			: Contact::null;
}

const QModelIndex ContactListModel::contactIndex(Contact contact) const
{
	return index(List.indexOf(contact), 0);
}
