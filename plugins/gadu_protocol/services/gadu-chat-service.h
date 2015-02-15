/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_CHAT_SERVICE_H
#define GADU_CHAT_SERVICE_H

#include <libgadu.h>

#include "message/message-common.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-image.h"

#include "protocols/services/chat-service.h"

class QTimer;

class FormattedStringFactory;
class GaduChatImageService;
class GaduConnection;
class GaduFileTransferService;
class ImageStorageService;
class RawMessage;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduChatService
 * @todo Refactor
 * @short Service for sending and receiving messages in Gadu-Gadu protocol.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service implements sending and receiving messages in Gadu-Gadu protocol.
 */
class GaduChatService : public ChatService
{
	Q_OBJECT

	QPointer<GaduChatImageService> CurrentGaduChatImageService;
	QPointer<GaduFileTransferService> CurrentFileTransferService;
	QPointer<ImageStorageService> CurrentImageStorageService;
	QPointer<FormattedStringFactory> CurrentFormattedStringFactory;

	QPointer<GaduConnection> Connection;

	QHash<int, Message> UndeliveredMessages;

	bool isSystemMessage(struct gg_event *e);
	Contact getSender(struct gg_event *e);
	bool ignoreSender(gg_event *e, Buddy sender);
	ContactSet getRecipients(struct gg_event *e);
	RawMessage getRawMessage(struct gg_event *e);
	bool ignoreRichText(Contact sender);

	void handleMsg(Contact sender, ContactSet recipients, MessageType type, struct gg_event *e);
	int sendRawMessage(const QVector<Contact> &contacts, const RawMessage &rawMessage, bool isPlainText);
	UinType * contactsToUins(const QVector<Contact> &contacts) const;

	QTimer *RemoveTimer;

private slots:
	void removeTimeoutUndeliveredMessages();

public:
	explicit GaduChatService(Account account, QObject *parent = 0);
	virtual ~GaduChatService();

	/**
	 * @short Set gadu chat image service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param gaduChatImageService chat image service for this service
	 */
	void setGaduChatImageService(GaduChatImageService *gaduChatImageService);

	void setGaduFileTransferService(GaduFileTransferService *gaduFileTransferService);

	/**
	 * @short Set image storage service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageStorageService image storage service for this service
	 *
	 * This service is used to get full file paths of chat images.
	 */
	void setImageStorageService(ImageStorageService *imageStorageService);

	/**
	 * @short Set formatted string factory for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param formattedStringFactory formatted string factory for this service
	 */
	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	/**
	 * @short Set connection for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param connection connection for this service
	 */
	void setConnection(GaduConnection *connection);

	virtual int maxMessageLength() const;

public slots:
	virtual bool sendMessage(const Message &message) override;
	virtual bool sendRawMessage(const Chat &chat, const QByteArray &rawMessage) override;

	/**
	 * @short Leave @p chat.
	 *
	 * Use to leave chat. Sets IgnoreAllMessages property to true.
	 */
	virtual void leaveChat(const Chat &chat) override;

	void handleEventMsg(struct gg_event *e);
	void handleEventMultilogonMsg(struct gg_event *e);
	void handleEventAck(struct gg_event *e);

signals:
	void chatImageKeyReceived(const QString &id, const ChatImage &chatImage);

};

/**
 * @}
 */

#endif // GADU_CHAT_SERVICE_H
