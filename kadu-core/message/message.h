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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtCore/QDateTime>

#include "message/message-common.h"
#include "message/message-shared.h"
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
	static Message loadStubFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint);
	static Message loadFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint);
	static Message null;

	Message();
	Message(MessageShared *data);
	explicit Message(QObject *data);
	Message(const Message &copy);
	virtual ~Message();

	KaduSharedBase_PropertyCRW(Chat, messageChat, MessageChat)
	KaduSharedBase_PropertyCRW(Contact, messageSender, MessageSender)
	void setContent(std::unique_ptr<FormattedString> &&content) const;
	KaduSharedBase_PropertyRead(FormattedString *, content, Content)
	KaduSharedBase_PropertyRead(QString, plainTextContent, PlainTextContent)
	KaduSharedBase_PropertyRead(QString, htmlContent, HtmlContent)
	KaduSharedBase_PropertyCRW(QDateTime, receiveDate, ReceiveDate)
	KaduSharedBase_PropertyCRW(QDateTime, sendDate, SendDate)
	KaduSharedBase_Property(MessageStatus, status, Status)
	KaduSharedBase_Property(MessageType, type, Type)
	KaduSharedBase_PropertyCRW(QString, id, Id)

};

Q_DECLARE_METATYPE(Message)

#endif // MESSAGE_H
