/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MESSAGE_SHARED_H
#define MESSAGE_SHARED_H

#include <QtCore/QSharedData>

#include "buddies/buddy.h"
#include "chat/message/message.h"
#include "storage/shared.h"

class Chat;

class MessageShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageShared)

	Chat MessageChat;
	Contact MessageSender;
	QString Content;
	QDateTime ReceiveDate;
	QDateTime SendDate;
	Message::Status Status;
	Message::Type Type;
	bool Pending;
	int Id;

protected:
	virtual void load();
	virtual void emitUpdated();

public:
	static MessageShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint);
	static MessageShared * loadFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint);

	explicit MessageShared(QUuid uuid = QUuid());
	virtual ~MessageShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();

	void setStatus(Message::Status status);

	KaduShared_Property(Chat, messageChat, MessageChat)
	KaduShared_Property(Contact, messageSender, MessageSender)
	KaduShared_Property(QString, content, Content)
	KaduShared_Property(QDateTime, receiveDate, ReceiveDate)
	KaduShared_Property(QDateTime, sendDate, SendDate)
	KaduShared_PropertyRead(Message::Status, status, Status)
	KaduShared_Property(Message::Type, type, Type)
	KaduShared_PropertyBool(Pending)
	KaduShared_Property(int, id, Id)

signals:
	void statusChanged(Message::Status);

	void updated();

};

#endif // MESSAGE_SHARED_H
