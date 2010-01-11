/*
 * %kadu copyright begin%
 * Copyright 2009 Bartlomiej Zimon (uzi18@o2.pl)
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

#include <QtCore/QVariant>
#include <QtGui/QAbstractProxyModel>

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "model/roles.h"

#include "abstract-buddies-model.h"

Buddy AbstractBuddiesModel::buddyAt(const QModelIndex &index) const
{
	QVariant conVariant = index.data(BuddyRole);
	if (!conVariant.canConvert<Buddy>())
		return Buddy::null;

	return conVariant.value<Buddy>();
}

Contact AbstractBuddiesModel::contactAt(const QModelIndex &index) const
{
	QVariant conVariant = index.data(ContactRole);
	if (!conVariant.canConvert<Contact>())
		return Contact::null;

	return conVariant.value<Contact>();
}
