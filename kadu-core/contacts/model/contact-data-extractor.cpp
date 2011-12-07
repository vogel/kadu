/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QVariant>

#include "accounts/account.h"
#include "avatars/avatar.h"
#include "buddies/buddy-kadu-data.h"
#include "contacts/contact.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"
#include "status/status-container.h"

#include "contact-data-extractor.h"

QVariant ContactDataExtractor::data(const Contact &contact, int role, bool useBuddyData)
{
	if (contact.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return contact.display(useBuddyData);
		case Qt::DecorationRole:
		{
			if (contact.ownerBuddy().isBlocked())
				return KaduIcon("kadu_icons/blocked").icon();

			if (contact.isBlocking())
				return KaduIcon("kadu_icons/blocking").icon();

			// TODO generic icon
			return contact.contactAccount().statusContainer()
					? contact.contactAccount().statusContainer()->statusIcon(contact.currentStatus()).icon()
					: QIcon();
		}
		case BuddyRole:
			return QVariant::fromValue(contact.ownerBuddy());
		case ContactRole:
			return QVariant::fromValue(contact);
		case DescriptionRole:
		{
			BuddyKaduData *bkd = contact.ownerBuddy().data()->moduleStorableData<BuddyKaduData>("kadu", 0, false);
			if (bkd && bkd->hideDescription())
				return QVariant();
			return contact.currentStatus().description();
		}
		case StatusRole:
			return QVariant::fromValue(contact.currentStatus());
		case AccountRole:
			return QVariant::fromValue(contact.contactAccount());
		case AvatarRole:
			return contact.avatar(useBuddyData).pixmap();
		case ItemTypeRole:
			return ContactRole;
		default:
			return QVariant();
	}
}
