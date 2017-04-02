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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "avatars/aggregated-contact-avatar-service.h"
#include "avatars/avatar-job-manager.h"
#include "avatars/avatar-storage.h"
#include "avatars/avatar.h"
#include "configuration/configuration-manager.h"
#include "contacts/contact-global-id.h"
#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"

#include "avatar-manager.h"

AvatarManager::AvatarManager(QObject *parent) : Manager<Avatar>{parent}
{
}

AvatarManager::~AvatarManager() = default;

void AvatarManager::setAggregatedContactAvatarService(AggregatedContactAvatarService *aggregatedContactAvatarService)
{
    m_aggregatedContactAvatarService = aggregatedContactAvatarService;
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
    connect(
        m_aggregatedContactAvatarService, &AggregatedContactAvatarService::avatarAvailable, this,
        &AvatarManager::avatarAvailable);
}

void AvatarManager::done()
{
    disconnect(
        m_aggregatedContactAvatarService, &AggregatedContactAvatarService::avatarAvailable, this,
        &AvatarManager::avatarAvailable);
}

void AvatarManager::avatarAvailable(const ContactGlobalId &contactId, const QByteArray &id)
{
    (void)id;

    auto contact = m_contactManager->byId(contactId.account, contactId.id.value, ActionReturnNull);
    if (contact)
        updateAvatar(contact);
}

void AvatarManager::itemAboutToBeAdded(Avatar)
{
}

void AvatarManager::itemAdded(Avatar item)
{
    connect(item, SIGNAL(pixmapUpdated()), this, SLOT(avatarPixmapUpdated()));
}

void AvatarManager::itemAboutToBeRemoved(Avatar item)
{
    disconnect(item, 0, this, 0);
}

void AvatarManager::itemRemoved(Avatar)
{
}

Avatar AvatarManager::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    return m_avatarStorage->loadStubFromStorage(storagePoint);
}

void AvatarManager::updateAvatar(const Contact &contact)
{
    QMutexLocker locker(&mutex());

    m_avatarJobManager->addJob(contact);
}

void AvatarManager::removeAvatar(const Contact &contact)
{
    auto avatar = byContact(contact, ActionReturnNull);
    if (!avatar)
        return;

    avatar.setPixmap(QPixmap{});
}

void AvatarManager::avatarPixmapUpdated()
{
    QMutexLocker locker(&mutex());

    Avatar avatar(sender());
    if (avatar)
        avatar.storeAvatar();   // store file now so webkit can see it
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
