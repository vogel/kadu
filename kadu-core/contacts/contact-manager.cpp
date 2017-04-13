/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contact-manager.h"

#include "buddies/buddy-manager.h"
#include "buddies/buddy-storage.h"
#include "buddies/buddy.h"
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-storage.h"
#include "core/myself.h"
#include "message/unread-message-repository.h"
#include "misc/change-notifier-lock.h"
#include "parser/parser.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-entry.h"

#include <QtCore/QPair>
#include <QtCore/QTimer>

ContactManager::ContactManager(QObject *parent) : Manager<Contact>{parent}
{
}

ContactManager::~ContactManager()
{
}

void ContactManager::setBuddyStorage(BuddyStorage *buddyStorage)
{
    m_buddyStorage = buddyStorage;
}

void ContactManager::setConfigurationManager(ConfigurationManager *configurationManager)
{
    m_configurationManager = configurationManager;
}

void ContactManager::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ContactManager::setContactStorage(ContactStorage *contactStorage)
{
    m_contactStorage = contactStorage;
}

void ContactManager::setMyself(Myself *myself)
{
    m_myself = myself;
}

void ContactManager::setParser(Parser *parser)
{
    m_parser = parser;
}

void ContactManager::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
    m_unreadMessageRepository = unreadMessageRepository;
}

void ContactManager::init()
{
    // needed for QueuedConnection
    qRegisterMetaType<Contact>("Contact");

    for (auto &message : m_unreadMessageRepository->allUnreadMessages())
        unreadMessageAdded(message);

    connect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)), this, SLOT(unreadMessageAdded(Message)));
    connect(
        m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)), this, SLOT(unreadMessageRemoved(Message)));

    m_configurationManager->registerStorableObject(this);

    ensureLoaded();
}

void ContactManager::done()
{
    disconnect(m_unreadMessageRepository, 0, this, 0);

    for (auto const &message : m_unreadMessageRepository->allUnreadMessages())
        unreadMessageRemoved(message);

    m_configurationManager->unregisterStorableObject(this);
}

void ContactManager::unreadMessageAdded(const Message &message)
{
    const Contact &contact = message.messageSender();
    contact.setUnreadMessagesCount(contact.unreadMessagesCount() + 1);
}

void ContactManager::unreadMessageRemoved(const Message &message)
{
    const Contact &contact = message.messageSender();
    quint16 unreadMessagesCount = contact.unreadMessagesCount();
    if (unreadMessagesCount > 0)
        contact.setUnreadMessagesCount(unreadMessagesCount - 1);
}

void ContactManager::itemAboutToBeAdded(Contact item)
{
    QMutexLocker locker(&mutex());

    connect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
    emit contactAboutToBeAdded(item);
}

void ContactManager::itemAdded(Contact item)
{
    if (!item)
        return;

    QMutexLocker locker(&mutex());

    emit contactAdded(item);

    if (m_myself->buddy() == item.ownerBuddy())
        item.rosterEntry()->setSynchronized();
}

void ContactManager::itemAboutToBeRemoved(Contact item)
{
    QMutexLocker locker(&mutex());

    disconnect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
    emit contactAboutToBeRemoved(item);
}

void ContactManager::itemRemoved(Contact item)
{
    emit contactRemoved(item);
}

Contact ContactManager::byId(Account account, const QString &id, NotFoundAction action)
{
    QMutexLocker locker(&mutex());

    ensureLoaded();

    if (id.isEmpty() || account.isNull())
        return Contact::null;

    for (auto &contact : items())
        if (account == contact.contactAccount() && id == contact.id())
            return contact;

    if (action == ActionReturnNull)
        return Contact::null;

    Contact contact = m_contactStorage->create();

    ChangeNotifierLock lock(
        contact.rosterEntry()->hasLocalChangesNotifier(), ChangeNotifierLock::ModeForget);   // don't emit dirty signals
    contact.setId(id);
    contact.setContactAccount(account);

    if (action == ActionCreateAndAdd)
        addItem(contact);

    auto buddy = m_buddyStorage->create();
    contact.setOwnerBuddy(buddy);

    return contact;
}

QVector<Contact> ContactManager::contacts(Account account, AnonymousInclusion inclusion)
{
    QMutexLocker locker(&mutex());

    ensureLoaded();

    QVector<Contact> contacts;

    if (account.isNull())
        return contacts;

    for (auto const &contact : items())
        if (account == contact.contactAccount() && ((IncludeAnonymous == inclusion) || !contact.isAnonymous()))
            contacts.append(contact);

    return contacts;
}

void ContactManager::contactDataUpdated()
{
    QMutexLocker locker(&mutex());

    Contact contact(sender());
    if (!contact.isNull())
        emit contactUpdated(contact);
}

Contact ContactManager::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    return m_contactStorage->loadStubFromStorage(storagePoint);
}

#include "moc_contact-manager.cpp"
