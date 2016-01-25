/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contact-data-extractor.h"

#include "accounts/account.h"
#include "avatars/avatar.h"
#include "contacts/contact.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"
#include "status/status-container.h"
#include "talkable/talkable.h"

#include <QtCore/QFileInfo>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

ContactDataExtractor::ContactDataExtractor(QObject *parent) :
		QObject{parent}
{
}

ContactDataExtractor::~ContactDataExtractor()
{
}

void ContactDataExtractor::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

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
		case StatusIconPath:
		{
			if (contact.ownerBuddy().isBlocked())
				return m_iconsManager->iconPath(KaduIcon("kadu_icons/blocked"));

			if (contact.isBlocking())
				return m_iconsManager->iconPath(KaduIcon("kadu_icons/blocking"));

			// TODO generic icon
			return contact.contactAccount().statusContainer()
					? m_iconsManager->iconPath(contact.contactAccount().statusContainer()->statusIcon(contact.currentStatus()))
					: QString();
		}
		case BuddyRole:
			return QVariant::fromValue(contact.ownerBuddy());
		case ContactRole:
			return QVariant::fromValue(contact);
		case DescriptionRole:
			if (contact.ownerBuddy() && contact.ownerBuddy().property("kadu:HideDescription", false).toBool())
				return QVariant();
			else
				return contact.currentStatus().description();
		case StatusRole:
			return QVariant::fromValue(contact.currentStatus());
		case AccountRole:
			return QVariant::fromValue(contact.contactAccount());
		case AvatarRole:
			return contact.avatar(useBuddyData).pixmap();
		case AvatarPathRole:
		{
			QFileInfo avatarInfo(contact.avatar(useBuddyData).filePath());

			if (avatarInfo.exists() && avatarInfo.isReadable() && avatarInfo.isFile())
				return avatarInfo.filePath();
			else
				return QString();
		}
		case ItemTypeRole:
			return ContactRole;
		case TalkableRole:
			return QVariant::fromValue(Talkable(contact));
		default:
			return QVariant();
	}
}

#include "moc_contact-data-extractor.cpp"
