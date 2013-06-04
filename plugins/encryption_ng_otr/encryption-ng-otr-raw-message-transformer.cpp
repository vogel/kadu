/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/userstate.h>
}

#include "accounts/account.h"
#include "chat/chat.h"
#include "chat/chat-details.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "formatted-string/formatted-string.h"
#include "message/message.h"

#include "encryption-ng-otr-app-ops-wrapper.h"
#include "encryption-ng-otr-notifier.h"
#include "encryption-ng-otr-op-data.h"
#include "encryption-ng-otr-private-key-service.h"
#include "encryption-ng-otr-user-state.h"

#include "encryption-ng-otr-raw-message-transformer.h"

EncryptionNgOtrRawMessageTransformer::EncryptionNgOtrRawMessageTransformer() :
		UserState(0)
{
}

EncryptionNgOtrRawMessageTransformer::~EncryptionNgOtrRawMessageTransformer()
{
}

void EncryptionNgOtrRawMessageTransformer::setUserState(EncryptionNgOtrUserState *userState)
{
	UserState = userState;
}

void EncryptionNgOtrRawMessageTransformer::setEncryptionNgOtrAppOpsWrapper(EncryptionNgOtrAppOpsWrapper *encryptionNgOtrAppOpsWrapper)
{
	OtrAppOpsWrapper = encryptionNgOtrAppOpsWrapper;
}

void EncryptionNgOtrRawMessageTransformer::setEncryptionNgOtrNotifier(EncryptionNgOtrNotifier *encryptionNgOtrNotifier)
{
	OtrNotifier = encryptionNgOtrNotifier;
}

void EncryptionNgOtrRawMessageTransformer::setEncryptionNgOtrPrivateKeyService(EncryptionNgOtrPrivateKeyService *encryptionNgOtrPrivateKeyService)
{
	OtrPrivateKeyService = encryptionNgOtrPrivateKeyService;
}

QByteArray EncryptionNgOtrRawMessageTransformer::transform(const QByteArray &messageContent, const Message &message)
{
	if (OtrAppOpsWrapper.isNull() || !UserState)
		return messageContent;

	switch (message.type())
	{
		case MessageTypeSent:
			return transformSent(messageContent, message);
		case MessageTypeReceived:
			return transformReceived(messageContent, message);
		default:
			return messageContent;
	}
}

QByteArray EncryptionNgOtrRawMessageTransformer::transformReceived(const QByteArray &messageContent, const Message &message)
{
	OtrlUserState userState = UserState->userState();
	if (!userState)
		return messageContent;

	EncryptionNgOtrOpData opData;
	opData.setAppOpsWrapper(OtrAppOpsWrapper.data());
	opData.setNotifier(OtrNotifier.data());
	opData.setPrivateKeyService(OtrPrivateKeyService.data());
	opData.setMessage(message);

	Account account = message.messageChat().chatAccount();
	char *newMessage = 0;

	bool ignoreMessage = otrl_message_receiving(userState, OtrAppOpsWrapper.data()->ops(), &opData,
			account.id().toUtf8().data(), account.protocolName().toUtf8().data(),
			message.messageSender().id().toUtf8().data(),
			messageContent.data(),
			&newMessage, 0, 0, 0, 0);

	if (ignoreMessage)
		return QByteArray();

	if (newMessage)
	{
		QByteArray result = newMessage;
		otrl_message_free(newMessage);
		return result;
	}
	else
		return messageContent;
}

QByteArray EncryptionNgOtrRawMessageTransformer::transformSent(const QByteArray &messageContent, const Message &message)
{
	Q_UNUSED(message);

	OtrlUserState userState = UserState->userState();
	if (!userState)
		return messageContent;

	Chat chat = message.messageChat();
	ChatDetails *chatDetails = chat.details();
	if (chatDetails->contacts().size() > 1)
		return messageContent;

	Contact receiver = (*chatDetails->contacts().begin());

	EncryptionNgOtrOpData opData;
	opData.setAppOpsWrapper(OtrAppOpsWrapper.data());
	opData.setPrivateKeyService(OtrPrivateKeyService.data());
	opData.setMessage(message);

	Account account = message.messageChat().chatAccount();
	char *newMessage = 0;

	gcry_error_t err = otrl_message_sending(userState, OtrAppOpsWrapper.data()->ops(), &opData,
			account.id().toUtf8().data(), account.protocolName().toUtf8().data(),
			receiver.id().toUtf8().data(), OTRL_INSTAG_BEST,
			messageContent.data(), 0,
			&newMessage, OTRL_FRAGMENT_SEND_SKIP,
			0, 0, 0);

	if (!err && newMessage)
	{
		QByteArray result = newMessage;
		otrl_message_free(newMessage);
		return result;
	}
	else
		return messageContent;
}
