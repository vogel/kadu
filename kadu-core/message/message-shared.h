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

#ifndef MESSAGE_SHARED_H
#define MESSAGE_SHARED_H

#include <QtCore/QDateTime>
#include <QtCore/QPointer>
#include <QtCore/QSharedData>

#include "message/message-common.h"
#include "storage/shared.h"

class Chat;
class Contact;
class FormattedString;
class FormattedStringFactory;

class MessageShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageShared)

	QPointer<FormattedStringFactory> CurrentFormattedStringFactory;

	Chat *MessageChat;
	Contact *MessageSender;
	std::unique_ptr<FormattedString> Content;
	QString PlainTextContent;
	QString HtmlContent;
	QDateTime ReceiveDate;
	QDateTime SendDate;
	MessageStatus Status;
	MessageType Type;
	QString Id;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	static MessageShared * loadStubFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint);
	static MessageShared * loadFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint);

	explicit MessageShared(const QUuid &uuid = QUuid());
	virtual ~MessageShared();

	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void setStatus(MessageStatus status);

	void setContent(std::unique_ptr<FormattedString> &&content);
	FormattedString * content() const;

	KaduShared_PropertyDeclCRW(Chat, messageChat, MessageChat)
	KaduShared_PropertyDeclCRW(Contact, messageSender, MessageSender)
	KaduShared_PropertyRead(QString, plainTextContent, PlainTextContent)
	KaduShared_PropertyRead(QString, htmlContent, HtmlContent)
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

};

#endif // MESSAGE_SHARED_H
