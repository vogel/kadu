/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QFile>
#include <QtCore/QTimer>

#include "accounts/account.h"
#include "avatars/avatar.h"
#include "avatars/avatar-job-manager.h"
#include "avatars/avatar-shared.h"
#include "configuration/configuration-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"
#include "debug.h"

#include "avatar-manager.h"

AvatarManager * AvatarManager::Instance = 0;

AvatarManager * AvatarManager::instance()
{
	if (!Instance)
		Instance = new AvatarManager();

	return Instance;
}

AvatarManager::AvatarManager()
{
	triggerAllAccountsRegistered();

	UpdateTimer = new QTimer(this);
	UpdateTimer->setInterval(5 * 60 * 1000); // 5 minutes
	connect(UpdateTimer, SIGNAL(timeout()), this, SLOT(updateAvatars()));
	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact)), this, SLOT(contactAdded(Contact)));

	UpdateTimer->start();
}

AvatarManager::~AvatarManager()
{
	triggerAllAccountsUnregistered();
	disconnect(ContactManager::instance(), SIGNAL(contactAdded(Contact)), this, SLOT(contactAdded(Contact)));
}

void AvatarManager::itemAboutToBeAdded(Avatar item)
{
	emit avatarAboutToBeAdded(item);
}

void AvatarManager::itemAdded(Avatar item)
{
	connect(item, SIGNAL(updated()), this, SLOT(avatarDataUpdated()));
	connect(item, SIGNAL(pixmapUpdated()), this, SLOT(avatarPixmapUpdated()));
	emit avatarAdded(item);
}

void AvatarManager::itemAboutToBeRemoved(Avatar item)
{
	emit avatarAboutToBeRemoved(item);
	disconnect(item, SIGNAL(updated()), this, SLOT(avatarDataUpdated()));
	disconnect(item, SIGNAL(pixmapUpdated()), this, SLOT(avatarPixmapUpdated()));
}

void AvatarManager::itemRemoved(Avatar item)
{
	emit avatarRemoved(item);
}

void AvatarManager::accountRegistered(Account account)
{
	QMutexLocker(&mutex());

	connect(account, SIGNAL(connected()), this, SLOT(updateAccountAvatars()));
}

void AvatarManager::accountUnregistered(Account account)
{
	QMutexLocker(&mutex());

	disconnect(account, SIGNAL(connected()), this, SLOT(updateAccountAvatars()));
}

void AvatarManager::contactAdded(Contact contact)
{
	QMutexLocker(&mutex());

	Protocol *protocol = contact.contactAccount().protocolHandler();
	if (protocol && protocol->isConnected())
		updateAvatar(contact, true);
}

bool AvatarManager::needUpdate(const Contact &contact)
{
	QMutexLocker(&mutex());

	if (!contact.contactAvatar())
		return true;

	QDateTime lastUpdated = contact.contactAvatar().lastUpdated();
	if (!lastUpdated.isValid())
		return true;
	// one hour passed
	if (lastUpdated.secsTo(QDateTime::currentDateTime()) > 60 * 60)
		return true;

	QDateTime nextUpdate = contact.contactAvatar().nextUpdate();
	if (nextUpdate > QDateTime::currentDateTime())
		return true;

	return false;
}

void AvatarManager::updateAvatar(const Contact &contact, bool force)
{
	QMutexLocker(&mutex());

	if (!force && !needUpdate(contact))
		return;

	AvatarJobManager::instance()->addJob(contact);
}

void AvatarManager::updateAvatars()
{
	QMutexLocker(&mutex());

	foreach (const Contact &contact, ContactManager::instance()->items())
		if (!contact.ownerBuddy().isAnonymous())
			updateAvatar(contact);
}

void AvatarManager::updateAccountAvatars()
{
	QMutexLocker(&mutex());

	Account account(sender());
	if (!account)
		return;

	foreach (const Contact &contact, ContactManager::instance()->contacts(account))
		if (!contact.ownerBuddy().isAnonymous())
			updateAvatar(contact, true);
}

void AvatarManager::avatarDataUpdated()
{
	QMutexLocker(&mutex());

	Avatar avatar(sender());
	if (avatar)
		emit avatarUpdated(avatar);
}

void AvatarManager::avatarPixmapUpdated()
{
	QMutexLocker(&mutex());

	Avatar avatar(sender());
	if (avatar)
		avatar.store(); // store file now so webkit can see it
}

Avatar AvatarManager::byBuddy(Buddy buddy, NotFoundAction action)
{
	if (buddy.buddyAvatar())
		return buddy.buddyAvatar();

	if (ActionReturnNull == action)
		return Avatar::null;

	Avatar avatar = Avatar::create();
	buddy.setBuddyAvatar(avatar);

	if (ActionCreateAndAdd == action)
		addItem(avatar);

	return avatar;
}

Avatar AvatarManager::byContact(Contact contact, NotFoundAction action)
{
	if (contact.contactAvatar())
		return contact.contactAvatar();

	if (ActionReturnNull == action)
		return Avatar::null;

	Avatar avatar = Avatar::create();
	contact.setContactAvatar(avatar);

	if (ActionCreateAndAdd == action)
		addItem(avatar);

	return avatar;
}
