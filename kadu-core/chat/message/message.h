/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtCore/QDateTime>

#include "chat/message/message-common.h"
#include "storage/shared-base.h"

#include "exports.h"

class Chat;
class Contact;
class MessageShared;

class KADUAPI Message : public SharedBase<MessageShared>
{
	KaduSharedBaseClass(Message)

public:
	static Message create();
	static Message loadStubFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint);
	static Message loadFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint);
	static Message null;

	Message();
	Message(MessageShared *data);
	Message(QObject *data);
	Message(const Message &copy);
	virtual ~Message();

	KaduSharedBase_PropertyCRW(Chat, messageChat, MessageChat)
	KaduSharedBase_PropertyCRW(Contact, messageSender, MessageSender)
	KaduSharedBase_PropertyCRW(QString, content, Content)
	KaduSharedBase_PropertyCRW(QDateTime, receiveDate, ReceiveDate)
	KaduSharedBase_PropertyCRW(QDateTime, sendDate, SendDate)
	KaduSharedBase_Property(MessageStatus, status, Status)
	KaduSharedBase_Property(MessageType, type, Type)
	KaduSharedBase_PropertyBool(Pending)
	KaduSharedBase_PropertyCRW(QString, id, Id)

};

Q_DECLARE_METATYPE(Message)

#endif // MESSAGE_H
