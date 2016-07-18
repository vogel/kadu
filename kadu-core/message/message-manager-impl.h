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

#include "accounts/accounts-aware-object.h"
#include "message/message-manager.h"
#include "message/message.h"
#include "exports.h"

#include <QtCore/QObject>
#include <injeqt/injeqt.h>

class AccountManager;
class FormattedStringFactory;
class MessageFilterService;
class MessageStorage;
class MessageTransformerService;

class KADUAPI MessageManagerImpl : public MessageManager, public AccountsAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MessageManagerImpl(QObject *parent = nullptr);
	virtual ~MessageManagerImpl();

	virtual bool sendMessage(const Chat &chat, const QString &content, bool silent = false) override;
	virtual bool sendMessage(const Chat &chat, std::unique_ptr<FormattedString> &&content, bool silent = false) override;
	virtual bool sendRawMessage(const Chat &chat, const QByteArray &content) override;

protected:
	virtual void accountRegistered(Account account) override;
	virtual void accountUnregistered(Account account) override;

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<MessageFilterService> m_messageFilterService;
	QPointer<MessageStorage> m_messageStorage;
	QPointer<MessageTransformerService> m_messageTransformerService;
	QPointer<FormattedStringFactory> m_formattedStringFactory;

	/**
	 * @short Create outoing message for given chat and given content.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of outgoing message
	 * @param content content of outgoing message
	 */
	Message createOutgoingMessage(const Chat &chat, std::unique_ptr<FormattedString> &&content);

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);

	/**
	 * @short Set message filter service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param messageFilterService message filter service for this service
	 */
	INJEQT_SET void setMessageFilterService(MessageFilterService *messageFilterService);
	INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);

	/**
	 * @short Set message transformer service for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param messageTransformerService message transformer service for this service
	 */
	INJEQT_SET void setMessageTransformerService(MessageTransformerService *messageTransformerService);

	/**
	 * @short Set formatted string factory for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param formattedStringFactory formatted string factory for this service
	 */
	INJEQT_SET void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	INJEQT_INIT void init();
	INJEQT_DONE void done();

	/**
	 * @short Slot called every time a new message was received from any of registered accounts.
	 * @author Rafał 'Vogel' Malinowski
	 * @param message received message
	 *
	 * This slot is called every time a new message was received from any of registered accounts.
	 * Each receied message is automatically recorded as unread message and can be set as pending message
	 * if no applicable chat widget is available.
	 *
	 * This slot emits messageReceived and unreadMessageAdded signals.
	 */
	void messageReceivedSlot(const Message &message);

};

/**
 * @}
 */
