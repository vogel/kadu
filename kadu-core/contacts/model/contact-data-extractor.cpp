/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "contacts/contact.h"
#include "buddies/buddy-kadu-data.h"
#include "buddies/buddy-shared.h"
#include "model/roles.h"
#include "icons-manager.h"

#include "contact-data-extractor.h"

QVariant ContactDataExtractor::data(const Contact &contact, int role, bool useBuddyData)
{
	if (contact.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return useBuddyData
					? contact.ownerBuddy().display()
					: contact.id();
		case Qt::DecorationRole:
		{
			if (contact.ownerBuddy().isBlocked())
				return IconsManager::instance()->iconByPath("kadu_icons/kadu-blocked");

			if (contact.isBlocking())
				return IconsManager::instance()->iconByPath("kadu_icons/kadu-blocking");

			// TODO generic icon
			return !contact.contactAccount().isNull()
					? contact.contactAccount().data()->statusIcon(contact.currentStatus())
					: QIcon();
		}
		case BuddyRole:
			return QVariant::fromValue(contact.ownerBuddy());
		case ContactRole:
			return QVariant::fromValue(contact);
		case DescriptionRole:
		{
			BuddyKaduData *bkd = contact.ownerBuddy().data()->moduleStorableData<BuddyKaduData>("kadu", true);
			if (bkd && bkd->hideDescription())
				return QVariant();
			return contact.currentStatus().description();
		}
		case StatusRole:
			return QVariant::fromValue(contact.currentStatus());
		case AccountRole:
			return QVariant::fromValue(contact.contactAccount());
		case AvatarRole:
			if (useBuddyData && !contact.ownerBuddy().buddyAvatar().isEmpty())
				return QVariant::fromValue(contact.ownerBuddy().buddyAvatar().pixmap());
			else
				return QVariant::fromValue(contact.contactAvatar().pixmap());
		case ItemTypeRole:
			return ContactRole;
		default:
			return QVariant();
	}
}
