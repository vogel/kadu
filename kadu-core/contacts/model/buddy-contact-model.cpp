/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddy-contact-model.h"

BuddyContactModel::BuddyContactModel(Buddy buddy, QObject *parent) :
		QAbstractListModel(parent), SourceBuddy(buddy), IncludeIdentityInDisplay(false)
{
	connect(SourceBuddy, SIGNAL(contactAboutToBeAdded(Contact)),
			this, SLOT(contactAboutToBeAdded(Contact)));
	connect(SourceBuddy, SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	connect(SourceBuddy, SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	connect(SourceBuddy, SIGNAL(contactRemoved(Contact)),
			this, SLOT(contactRemoved(Contact)));
}

BuddyContactModel::~BuddyContactModel()
{
	disconnect(SourceBuddy, SIGNAL(contactAboutToBeAdded(Contact)),
			this, SLOT(contactAboutToBeAdded(Contact)));
	disconnect(SourceBuddy, SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	disconnect(SourceBuddy, SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	disconnect(SourceBuddy, SIGNAL(contactRemoved(Contact)),
			this, SLOT(contactRemoved(Contact)));
}

int BuddyContactModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 1;
}

int BuddyContactModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : SourceBuddy.contacts().count();
}

QVariant BuddyContactModel::data(const QModelIndex &index, int role) const
{
	Contact data = contact(index);
	if (data.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			if (IncludeIdentityInDisplay)
				return QString("%1 (%2)").arg(data.id()).arg(data.contactAccount().accountIdentity().name());
			else
				return data.id();

		case Qt::DecorationRole:
			return data.contactAccount().protocolHandler()
					? data.contactAccount().protocolHandler()->icon().icon()
					: QIcon();

		case ContactRole:
			return QVariant::fromValue<Contact>(data);

		default:
			return QVariant();
	}
}

Contact BuddyContactModel::contact(const QModelIndex &index) const
{
	if (!index.isValid())
		return Contact::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return Contact::null;

	return SourceBuddy.contacts().at(index.row());
}

int BuddyContactModel::contactIndex(Contact data)
{
	return SourceBuddy.contacts().indexOf(data);
}

QModelIndex BuddyContactModel::contactModelIndex(Contact data)
{
	return createIndex(contactIndex(data), 0, 0);
}

void BuddyContactModel::contactAboutToBeAdded(Contact data)
{
	Q_UNUSED(data)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void BuddyContactModel::contactAdded(Contact data)
{
	Q_UNUSED(data)

	endInsertRows();
}

void BuddyContactModel::contactAboutToBeRemoved(Contact data)
{
	int index = contactIndex(data);
	beginRemoveRows(QModelIndex(), index, index);
}

void BuddyContactModel::contactRemoved(Contact data)
{
	Q_UNUSED(data)

	endRemoveRows();
}

void BuddyContactModel::setIncludeIdentityInDisplay(bool includeIdentityInDisplay)
{
	if (IncludeIdentityInDisplay == includeIdentityInDisplay)
		return;

	IncludeIdentityInDisplay = includeIdentityInDisplay;
	emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}
