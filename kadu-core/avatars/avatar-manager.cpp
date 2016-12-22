/*
 * %kadu copyright begin%
 * Copyright 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "avatars/avatar-job-manager.h"
#include "avatars/avatar-storage.h"
#include "avatars/avatar.h"
#include "configuration/configuration-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"
#include "debug.h"

#include "avatar-manager.h"

AvatarManager::AvatarManager(QObject *parent) :
		SimpleManager<Avatar>{parent},
		UpdateTimer{nullptr}
{
}

AvatarManager::~AvatarManager()
{
}

void AvatarManager::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void AvatarManager::setAvatarJobManager(AvatarJobManager *avatarJobManager)
{
	m_avatarJobManager = avatarJobManager;
}

void AvatarManager::setAvatarStorage(AvatarStorage *avatarStorage)
{
	m_avatarStorage = avatarStorage;
}

void AvatarManager::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void AvatarManager::init()
{
	triggerAllAccountsAdded(m_accountManager);

	UpdateTimer = new QTimer(this);
	UpdateTimer->setInterval(30 * 60 * 1000); // half an hour
	connect(UpdateTimer, SIGNAL(timeout()), this, SLOT(updateAvatars()));
	connect(m_contactManager, SIGNAL(contactAdded(Contact)), this, SLOT(contactAdded(Contact)));

	UpdateTimer->start();
}

void AvatarManager::done()
{
	triggerAllAccountsRemoved(m_accountManager);
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
	disconnect(item, 0, this, 0);
}

void AvatarManager::itemRemoved(Avatar item)
{
	emit avatarRemoved(item);
}

Avatar AvatarManager::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	return m_avatarStorage->loadStubFromStorage(storagePoint);
}

void AvatarManager::accountAdded(Account account)
{
	QMutexLocker locker(&mutex());

	connect(account, SIGNAL(connected()), this, SLOT(updateAccountAvatars()));
}

void AvatarManager::accountRemoved(Account account)
{
	QMutexLocker locker(&mutex());

	disconnect(account, 0, this, 0);
}

void AvatarManager::contactAdded(Contact contact)
{
	QMutexLocker locker(&mutex());

	auto protocol = contact.contactAccount().protocolHandler();
	if (!protocol || !protocol->isConnected() || !protocol->avatarService())
		return;

	if (protocol->avatarService()->eventBasedUpdates())
		return;

	updateAvatar(contact, true);
}

bool AvatarManager::needUpdate(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	Protocol *protocol = contact.contactAccount().protocolHandler();
	if (!protocol || !protocol->isConnected())
		return false;

	auto avatar = byContact(contact, ActionCreateAndAdd);

	QDateTime lastUpdated = avatar.lastUpdated();
	if (!lastUpdated.isValid())
		return true;
	// one hour passed
	if (lastUpdated.secsTo(QDateTime::currentDateTime()) > 60 * 60)
		return true;

	QDateTime nextUpdate = avatar.nextUpdate();
	if (nextUpdate > QDateTime::currentDateTime())
		return true;

	return false;
}

void AvatarManager::updateAvatar(const Contact &contact, bool force)
{
	QMutexLocker locker(&mutex());

	if (!force && !needUpdate(contact))
		return;

	m_avatarJobManager->addJob(contact);
}

void AvatarManager::removeAvatar(const Contact &contact)
{
	auto avatar = byContact(contact, ActionReturnNull);
	if (!avatar)
		return;

	avatar.setLastUpdated(QDateTime::currentDateTime());
	avatar.setNextUpdate(QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() + 7200));
	avatar.setPixmap(QPixmap{});
}

void AvatarManager::updateAvatars()
{
	QMutexLocker locker(&mutex());

	foreach (const Contact &contact, m_contactManager->items())
		if (!contact.isAnonymous())
		{
			auto account = contact.contactAccount();
			if (!account || !account.protocolHandler() || !account.protocolHandler()->avatarService())
				continue;

			if (account.protocolHandler()->avatarService()->eventBasedUpdates())
				continue;

			updateAvatar(contact);
		}
}

void AvatarManager::updateAccountAvatars()
{
	QMutexLocker locker(&mutex());

	Account account(sender());
	if (!account || !account.protocolHandler() || !account.protocolHandler()->avatarService())
		return;

	if (account.protocolHandler()->avatarService()->eventBasedUpdates())
		return;

	foreach (const Contact &contact, m_contactManager->contacts(account))
		if (!contact.isAnonymous())
			updateAvatar(contact, true);
}

void AvatarManager::avatarDataUpdated()
{
	QMutexLocker locker(&mutex());

	Avatar avatar(sender());
	if (avatar)
		emit avatarUpdated(avatar);
}

void AvatarManager::avatarPixmapUpdated()
{
	QMutexLocker locker(&mutex());

	Avatar avatar(sender());
	if (avatar)
		avatar.storeAvatar(); // store file now so webkit can see it
}

Avatar AvatarManager::byBuddy(Buddy buddy, NotFoundAction action)
{
	if (buddy.buddyAvatar())
		return buddy.buddyAvatar();

	if (ActionReturnNull == action)
		return Avatar::null;

	auto avatar = m_avatarStorage->create();
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

	auto avatar = m_avatarStorage->create();
	contact.setContactAvatar(avatar);

	if (ActionCreateAndAdd == action)
		addItem(avatar);

	return avatar;
}

#include "moc_avatar-manager.cpp"
