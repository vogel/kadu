/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "model/roles.h"

#include <QtCore/QModelIndexList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyPreferredManager;
class ChatManager;
class ChatStorage;
class UnreadMessageRepository;

class ModelIndexListConverter : public QObject
{
	Q_OBJECT

public:
	explicit ModelIndexListConverter(const QModelIndexList &modelIndexList, QObject *parent = nullptr);
	virtual ~ModelIndexListConverter();

	RoleSet roles() const;
	BuddySet buddies() const;
	ContactSet contacts() const;
	Chat chat() const;

private:
	QPointer<BuddyPreferredManager> m_buddyPreferredManager;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

	const QModelIndexList &ModelIndexList;

	RoleSet Roles;
	BuddySet Buddies;
	ContactSet Contacts;
	Chat ComputedChat;

	void buildRoles();
	void buildBuddies();
	void buildContacts();
	void buildChat();

	Chat chatFromIndex(const QModelIndex &index) const;
	Chat chatFromBuddies() const;
	Chat chatFromContacts(const Account &account) const;

	Account commonAccount() const;
	Contact contactForAccount(const QModelIndex &inde, const Account &account) const;

private slots:
	INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);
	INJEQT_INIT void init();

};
