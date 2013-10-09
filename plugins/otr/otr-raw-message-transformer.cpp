/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "chat/chat-details.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "formatted-string/formatted-string.h"
#include "message/message.h"

#include "otr-app-ops-service.h"
#include "otr-op-data-factory.h"
#include "otr-op-data.h"
#include "otr-session-service.h"
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

void OtrRawMessageTransformer::setAppOpsService(OtrAppOpsService *appOpsService)
{
	AppOpsService = appOpsService;
}

void OtrRawMessageTransformer::setOpDataFactory(OtrOpDataFactory *opDataFactory)
{
	OpDataFactory = opDataFactory;
}

void OtrRawMessageTransformer::setSessionService(OtrSessionService *sessionService)
{
	SessionService = sessionService;
}

void OtrRawMessageTransformer::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

QByteArray OtrRawMessageTransformer::transform(const QByteArray &messageContent, const Message &message)
{
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
	if (!AppOpsService || !OpDataFactory || !UserStateService)
		return messageContent;

	OtrlUserState userState = UserStateService.data()->userState();
	if (!userState)
		return messageContent;

	OtrOpData opData = OpDataFactory.data()->opDataForContact(message.messageChat().contacts().toContact());
	Account account = message.messageChat().chatAccount();
	char *newMessage = 0;
	OtrlTLV *tlvs = 0;

	bool ignoreMessage = otrl_message_receiving(userState, AppOpsService.data()->appOps(), &opData,
			account.id().toUtf8().data(), account.protocolName().toUtf8().data(),
			message.messageSender().id().toUtf8().data(),
			messageContent.data(),
			&newMessage, &tlvs, 0, 0, 0);

	OtrlTLV *tlv = otrl_tlv_find(tlvs, OTRL_TLV_DISCONNECTED);
	if (tlv)
		emit peerEndedSession(message.messageSender());
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
	if (!AppOpsService || !OpDataFactory || !UserStateService)
		return messageContent;

	OtrlUserState userState = UserStateService.data()->userState();
	if (!userState)
		return messageContent;

	Chat chat = message.messageChat();
	ChatDetails *chatDetails = chat.details();
	if (chatDetails->contacts().size() > 1)
		return messageContent;

	Contact receiver = (*chatDetails->contacts().begin());
	OtrOpData opData = OpDataFactory.data()->opDataForContact(message.messageChat().contacts().toContact());
	Account account = message.messageChat().chatAccount();
	char *newMessage = 0;

	gcry_error_t err = otrl_message_sending(userState, AppOpsService.data()->appOps(), &opData,
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
