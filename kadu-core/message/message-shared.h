/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QSharedData>

#include "message/message-common.h"
#include "storage/shared.h"

class Chat;
class Contact;

class MessageShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageShared)

	Chat *MessageChat;
	Contact *MessageSender;
	QString Content;
	QDateTime ReceiveDate;
	QDateTime SendDate;
	MessageStatus Status;
	MessageType Type;
	bool Pending;
	QString Id;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();
	virtual void emitUpdated();

public:
	static MessageShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint);
	static MessageShared * loadFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint);

	explicit MessageShared(const QUuid &uuid = QUuid());
	virtual ~MessageShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void setStatus(MessageStatus status);

	KaduShared_PropertyDeclCRW(Chat, messageChat, MessageChat)
	KaduShared_PropertyDeclCRW(Contact, messageSender, MessageSender)
	KaduShared_Property(const QString &, content, Content)
	KaduShared_Property(const QDateTime &, receiveDate, ReceiveDate)
	KaduShared_Property(const QDateTime &, sendDate, SendDate)
	KaduShared_PropertyRead(MessageStatus, status, Status)
	KaduShared_Property(MessageType, type, Type)
	KaduShared_PropertyBool(Pending)
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

};

#endif // MESSAGE_SHARED_H
