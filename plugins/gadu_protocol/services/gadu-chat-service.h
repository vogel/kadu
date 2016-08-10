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

#pragma once

#include <libgadu.h>
#include <injeqt/injeqt.h>

#include "message/message-common.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-image.h"

#include "protocols/services/chat-service.h"

class QTimer;

class ChatManager;
class ChatStorage;
class Configuration;
class ContactManager;
class FormattedStringFactory;
class GaduChatImageService;
class GaduConnection;
class GaduFileTransferService;
class IconsManager;
class ImageStorageService;
class MessageStorage;
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

	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<Configuration> m_configuration;
	QPointer<ContactManager> m_contactManager;
	QPointer<GaduChatImageService> CurrentGaduChatImageService;
	QPointer<GaduFileTransferService> CurrentFileTransferService;
	QPointer<IconsManager> m_iconsManager;
	QPointer<ImageStorageService> CurrentImageStorageService;
	QPointer<FormattedStringFactory> m_formattedStringFactory;
	QPointer<MessageStorage> m_messageStorage;

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
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setImageStorageService(ImageStorageService *imageStorageService);
	INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);

	void removeTimeoutUndeliveredMessages();

public:
	explicit GaduChatService(Account account, QObject *parent = nullptr);
	virtual ~GaduChatService();

	/**
	 * @short Set gadu chat image service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param gaduChatImageService chat image service for this service
	 */
	void setGaduChatImageService(GaduChatImageService *gaduChatImageService);

	void setGaduFileTransferService(GaduFileTransferService *gaduFileTransferService);

	/**
	 * @short Set connection for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param connection connection for this service
	 */
	void setConnection(GaduConnection *connection);

	virtual int maxMessageLength() const override;

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
