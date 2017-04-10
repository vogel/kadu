/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;
class Avatars;
class BuddyChatManager;
class BuddyManager;
class BuddyPreferredManager;
class Buddy;
class ChatDataExtractor;
class ChatManager;
class ChatStorage;
class Chat;
class Contact;
class Status;
class Talkable;

class KADUAPI TalkableConverter : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit TalkableConverter(QObject *parent = nullptr);
    virtual ~TalkableConverter();

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Returns account assigned to this Talkable instance.
     * @return account assigned to this Talkable instance
     *
     * For Talkables of type ItemChat or ItemContact account assigned to contained Chat or Contact object
     * is returned. For Talkables of type ItemBuddy account assigned to preferred contact of contained Buddy is
     * returned. For computation of preffered contact @see BuddyPreferredManager.
     */
    Account toAccount(const Talkable &talkable) const;

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Returns avatar path assigned to this Talkable.
     * @return avatar assigned to this Talkable
     *
     * If current Talkable instance is of type ItemBuddy and Buddy instance contains non-empty avatar, then it is used.
     * If not, avatar of contact returned by toContact() is used and returned.
     */
    QString toAvatarPath(const Talkable &talkable) const;

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Returns Buddy instance from this Talkable instance.
     * @return Buddy instance from this Talkable instance
     *
     * If current Talkable instance is of type ItemBuddy, then Buddy used to create this instance is returned.
     * If current Talkable instance is of type ItemContact, then ownerBuddy of Contact used to create this instance is
     * returned.
     * If current Talkable instance is of type ItemChat, then if this Chat is composed only of one Contact, this
     * Contact's
     * ownerBuddy is returned. In other case null Buddy is returned.
     */
    Buddy toBuddy(const Talkable &talkable) const;

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Returns Contact instance from this Talkable instance.
     * @return Contact instance from this Talkable instance
     *
     * If current Talkable instance is of type ItemContact, then Contact used to create this instance is returned.
     * If current Talkable instance is of type ItemBuddy, then preffered contact of Buddy used to create this
     * instance is returned. For computation of preffered contact @see BuddyPreferredManager.
     * If current Talkable instance is of type ItemChat, then if this Chat is composed only of one Contact, this Contact
     * is returned. In other case null Contact is returned.
     */
    Contact toContact(const Talkable &talkable) const;

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Returns Contact instance from this Talkable instance.
     * @return Contact instance from this Talkable instance
     *
     * If current Talkable instance is of type ItemChat, then Chat used to create this instance is returned.
     * When current Talkable is of type ItemBuddy then AggreagetChat is returned. In other case, Chat build
     * from one Contact is returned.
     */
    Chat toChat(const Talkable &talkable) const;

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Returns display value of this Talkable instance.
     * @return display value of this Talkable instance
     */
    QString toDisplay(const Talkable &talkable) const;

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Returns current status of Contact returnd by toContact().
     * @return current status of Contact returnd by toContact()
     */
    Status toStatus(const Talkable &talkable) const;

private:
    QPointer<Avatars> m_avatars;
    QPointer<BuddyChatManager> m_buddyChatManager;
    QPointer<BuddyManager> m_buddyManager;
    QPointer<BuddyPreferredManager> m_buddyPreferredManager;
    QPointer<ChatDataExtractor> m_chatDataExtractor;
    QPointer<ChatManager> m_chatManager;
    QPointer<ChatStorage> m_chatStorage;

private slots:
    INJEQT_SET void setAvatars(Avatars *avatars);
    INJEQT_SET void setBuddyChatManager(BuddyChatManager *buddyChatManager);
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
    INJEQT_SET void setChatDataExtractor(ChatDataExtractor *chatDataExtractor);
    INJEQT_SET void setChatManager(ChatManager *chatManager);
    INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
};
