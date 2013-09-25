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

#include "otr-app-ops-wrapper.h"
#include "otr-op-data.h"
#include "otr-private-key-service.h"
#include "otr-user-state-service.h"

#include "otr-raw-message-transformer.h"

OtrRawMessageTransformer::OtrRawMessageTransformer() :
		EnableFragments(false)
{
}

OtrRawMessageTransformer::~OtrRawMessageTransformer()
{
}

void OtrRawMessageTransformer::setEnableFragments(bool enableFragments)
{
	EnableFragments = enableFragments;
}

void OtrRawMessageTransformer::setOtrAppOpsWrapper(OtrAppOpsWrapper *encryptionNgOtrAppOpsWrapper)
{
	AppOpsWrapper = encryptionNgOtrAppOpsWrapper;
}

void OtrRawMessageTransformer::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

void OtrRawMessageTransformer::setOtrPrivateKeyService(OtrPrivateKeyService *encryptionNgOtrPrivateKeyService)
{
	PrivateKeyService = encryptionNgOtrPrivateKeyService;
}

QByteArray OtrRawMessageTransformer::transform(const QByteArray &messageContent, const Message &message)
{
	if (AppOpsWrapper.isNull())
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

QByteArray OtrRawMessageTransformer::transformReceived(const QByteArray &messageContent, const Message &message)
{
	if (!UserStateService)
		return messageContent;

	OtrlUserState userState = UserStateService.data()->userState();
	if (!userState)
		return messageContent;

	OtrOpData opData(otrOpDataFromMessage(message));
	Account account = message.messageChat().chatAccount();
	char *newMessage = 0;
    OtrlTLV *tlvs = 0;

	bool ignoreMessage = otrl_message_receiving(userState, AppOpsWrapper.data()->ops(), &opData,
			account.id().toUtf8().data(), account.protocolName().toUtf8().data(),
			message.messageSender().id().toUtf8().data(),
			messageContent.data(),
			&newMessage, &tlvs, 0, 0, 0);

    OtrlTLV *tlv = otrl_tlv_find(tlvs, OTRL_TLV_DISCONNECTED);
    if (tlv)
		AppOpsWrapper.data()->peerClosedSession(message.messageSender());
    otrl_tlv_free(tlvs);

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

QByteArray OtrRawMessageTransformer::transformSent(const QByteArray &messageContent, const Message &message)
{
	if (!UserStateService)
		return messageContent;

	OtrlUserState userState = UserStateService.data()->userState();
	if (!userState)
		return messageContent;

	Chat chat = message.messageChat();
	ChatDetails *chatDetails = chat.details();
	if (chatDetails->contacts().size() > 1)
		return messageContent;

	Contact receiver = (*chatDetails->contacts().begin());
	OtrOpData opData(otrOpDataFromMessage(message));
	Account account = message.messageChat().chatAccount();
	char *newMessage = 0;

	gcry_error_t err = otrl_message_sending(userState, AppOpsWrapper.data()->ops(), &opData,
			account.id().toUtf8().data(), account.protocolName().toUtf8().data(),
			receiver.id().toUtf8().data(), OTRL_INSTAG_BEST,
			messageContent.data(), 0,
			&newMessage, EnableFragments ? OTRL_FRAGMENT_SEND_ALL : OTRL_FRAGMENT_SEND_SKIP,
			0, 0, 0);

	if (!err && newMessage)
	{
		QByteArray result = newMessage;
		otrl_message_free(newMessage);
		return result;
	}

	if (newMessage)
		otrl_message_free(newMessage);

	return messageContent;
}

OtrOpData OtrRawMessageTransformer::otrOpDataFromMessage(const Message &message) const
{
	OtrOpData opData;
	opData.setAppOpsWrapper(AppOpsWrapper.data());
	opData.setPrivateKeyService(PrivateKeyService.data());
	opData.setChat(message.messageChat());
	opData.setPeerDisplay(message.messageChat().contacts().toContact().display(true));

	return opData;
}
