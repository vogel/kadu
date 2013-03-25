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
#	include <libotr/userstate.h>
}

#include "accounts/account.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "formatted-string/formatted-string.h"
#include "message/message.h"

#include "encryption-ng-otr-app-ops-wrapper.h"
#include "encryption-ng-otr-op-data.h"
#include "encryption-ng-otr-private-key-service.h"
#include "encryption-ng-otr-user-state-service.h"

#include "encryption-ng-otr-raw-message-transformer.h"

EncryptionNgOtrRawMessageTransformer::EncryptionNgOtrRawMessageTransformer()
{
}

EncryptionNgOtrRawMessageTransformer::~EncryptionNgOtrRawMessageTransformer()
{
}

void EncryptionNgOtrRawMessageTransformer::setEncryptionNgOtrAppOpsWrapper(EncryptionNgOtrAppOpsWrapper *encryptionNgOtrAppOpsWrapper)
{
	OtrAppOpsWrapper = encryptionNgOtrAppOpsWrapper;
}

void EncryptionNgOtrRawMessageTransformer::setEncryptionNgOtrPrivateKeyService(EncryptionNgOtrPrivateKeyService *encryptionNgOtrPrivateKeyService)
{
	OtrPrivateKeyService = encryptionNgOtrPrivateKeyService;
}

void EncryptionNgOtrRawMessageTransformer::setEncryptionNgOtrUserStateService(EncryptionNgOtrUserStateService *encryptionNgOtrUserStateService)
{
	OtrUserStateService = encryptionNgOtrUserStateService;
}

QByteArray EncryptionNgOtrRawMessageTransformer::transform(const QByteArray &messageContent, const Message &message)
{
	if (MessageTypeSent == message.type() || OtrAppOpsWrapper.isNull() || OtrUserStateService.isNull())
		return messageContent;

	printf("received message: %s\n", messageContent.data());

	Account account = message.messageChat().chatAccount();
	OtrlUserState userState = OtrUserStateService.data()->forAccount(account);
	if (!userState)
		return messageContent;

	EncryptionNgOtrOpData opData;
	opData.setAppOpsWrapper(OtrAppOpsWrapper.data());
	opData.setAPrivateKeyService(OtrPrivateKeyService.data());
	opData.setMessage(message);

	char *newMessage = 0;
	bool ignoreMessage = otrl_message_receiving(userState, OtrAppOpsWrapper.data()->ops(), &opData,
			account.id().toUtf8().data(), account.protocolName().toUtf8().data(),
			message.messageSender().id().toUtf8().data(),
			messageContent.data(),
			&newMessage, 0, 0, 0);

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
