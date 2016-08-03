/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message-common.h"
#include "storage/shared.h"
#include "exports.h"

#include <QtCore/QDateTime>
#include <QtCore/QPointer>
#include <QtCore/QSharedData>

class ChatManager;
class Chat;
class ContactManager;
class Contact;
class UnreadMessageRepository;

class KADUAPI MessageShared : public Shared
{
	Q_OBJECT

public:
	explicit MessageShared(const QUuid &uuid = QUuid());
	virtual ~MessageShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void setStatus(MessageStatus status);

	KaduShared_PropertyDeclCRW(Chat, messageChat, MessageChat)
	KaduShared_PropertyDeclCRW(Contact, messageSender, MessageSender)
	KaduShared_Property(const QString &, htmlContent, HtmlContent)
	KaduShared_Property(const QDateTime &, receiveDate, ReceiveDate)
	KaduShared_Property(const QDateTime &, sendDate, SendDate)
	KaduShared_PropertyRead(MessageStatus, status, Status)
	KaduShared_Property(MessageType, type, Type)
	KaduShared_Property(const QString &, id, Id)

signals:
	/**
	 * @short Signal emited when message status was changed.
	 * @param previousStatus status before change
	 *
	 * This signal is emited when message status changes.
	 */
	void statusChanged(MessageStatus previousStatus);

	void updated();

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<ContactManager> m_contactManager;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

	Chat *MessageChat;
	Contact *MessageSender;
	QString HtmlContent;
	QDateTime ReceiveDate;
	QDateTime SendDate;
	MessageStatus Status;
	MessageType Type;
	QString Id;

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);

};
