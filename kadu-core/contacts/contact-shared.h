/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "status/status.h"
#include "storage/shared.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtNetwork/QHostAddress>
#include <injeqt/injeqt.h>

class AccountManager;
class Account;
class AvatarManager;
class Avatar;
class BuddyManager;
class Buddy;
class Configuration;
class ContactManager;
class ProtocolFactory;
class ProtocolsManager;
class RosterEntry;

class KADUAPI ContactShared : public Shared
{
    Q_OBJECT
    Q_DISABLE_COPY(ContactShared)

    QPointer<AccountManager> m_accountManager;
    QPointer<AvatarManager> m_avatarManager;
    QPointer<BuddyManager> m_buddyManager;
    QPointer<Configuration> m_configuration;
    QPointer<ContactManager> m_contactManager;
    QPointer<ProtocolsManager> m_protocolsManager;

    RosterEntry *Entry;

    Account *ContactAccount;
    Avatar *ContactAvatar;
    Buddy *OwnerBuddy;
    QString Id;
    int Priority;
    short int MaximumImageSize;
    quint16 UnreadMessagesCount;

    Status CurrentStatus;
    bool Blocking;
    bool IgnoreNextStatusChange;

    void addToBuddy();
    void removeFromBuddy();

    void doSetOwnerBuddy(const Buddy &buddy);
    void doSetContactAvatar(const Avatar &contactAvatar);

private slots:
    INJEQT_SET void setAccountManager(AccountManager *accountManager);
    INJEQT_SET void setAvatarManager(AvatarManager *avatarManager);
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setContactManager(ContactManager *contactManager);
    INJEQT_SET void setProtocolsManager(ProtocolsManager *protocolsManager);
    INJEQT_INIT void init();

    void protocolFactoryRegistered(ProtocolFactory *protocolFactory);
    void protocolFactoryUnregistered(ProtocolFactory *protocolFactory);

    void avatarUpdated();
    void changeNotifierChanged();

protected:
    virtual void load();
    virtual void store();
    virtual bool shouldStore();

public:
    explicit ContactShared(const QUuid &uuid = QUuid());
    virtual ~ContactShared();

    virtual StorableObject *storageParent();
    virtual QString storageNodeName();

    virtual void aboutToBeRemoved();

    KaduShared_PropertyRead(const QString &, id, Id) void setId(const QString &id);

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Return RosterEntry for this Contact.
     * @return RosterEntry for this Contact
     *
     * This method never returns null entry.
     */
    RosterEntry *rosterEntry();

    KaduShared_PropertyDeclCRW(Account, contactAccount, ContactAccount)

        KaduShared_PropertyReadDecl(Avatar, contactAvatar) void setContactAvatar(const Avatar &contactAvatar);

    KaduShared_PropertyDeclCRW(Buddy, ownerBuddy, OwnerBuddy)

        KaduShared_PropertyRead(int, priority, Priority) void setPriority(int priority);

    KaduShared_Property(const Status &, currentStatus, CurrentStatus) KaduShared_PropertyBool(Blocking)
        KaduShared_Property(bool, ignoreNextStatusChange, IgnoreNextStatusChange)
            KaduShared_Property(short int, maximumImageSize, MaximumImageSize)
                KaduShared_Property(quint16, unreadMessagesCount, UnreadMessagesCount)

                    bool isAnonymous();
    QString display(bool useBuddyData);
    Avatar avatar(bool useBuddyData);

signals:
    void updated();
    void buddyUpdated();
    void priorityUpdated();
    void updatedLocally();
};
