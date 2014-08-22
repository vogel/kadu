/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/type/chat-type-contact.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"

#include "otr-op-data.h"

#include "otr-message-event-service.h"

void OtrMessageEventService::wrapperOtrHandleMessageEvent(void *data, OtrlMessageEvent event, ConnContext *context, const char *message, gcry_error_t error)
{
	Q_UNUSED(context);

	OtrOpData *opData = static_cast<OtrOpData *>(data);
	if (opData->messageEventService())
		opData->messageEventService()->handleMessageEvent(opData->contact(), event, message, error);
}

OtrMessageEventService::OtrMessageEventService(QObject *parent) :
		QObject(parent)
{
}

OtrMessageEventService::~OtrMessageEventService()
{
}

void OtrMessageEventService::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	CurrentChatWidgetManager = chatWidgetManager;
}

void OtrMessageEventService::handleMessageEvent(const Contact &contact, OtrlMessageEvent event, const QString &message, gcry_error_t errorCode) const
{
	auto errorMessage = messageString(event, message, errorCode, contact.display(true));

	if (errorMessage.isEmpty())
		return;

	auto chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
	auto chatWidget = Core::instance()->chatWidgetRepository()->widgetForChat(chat);
	if (chatWidget)
		chatWidget->appendSystemMessage(errorMessage);
}

QString OtrMessageEventService::messageString(OtrlMessageEvent event, const QString &message, gcry_error_t errorCode, const QString &peerDisplay) const
{
	switch (event)
	{
		case OTRL_MSGEVENT_ENCRYPTION_REQUIRED:
			return tr("Unencrypted messages to %1 are not allowed. Attmpting to start a private conversation...").arg(peerDisplay);
		case OTRL_MSGEVENT_ENCRYPTION_ERROR:
			return tr("Encryption error. Message was not sent.");
		case OTRL_MSGEVENT_CONNECTION_ENDED:
			return tr("Message was not sent. Private conversation was closed by %1. Either restart or close your private conversation.").arg(peerDisplay);
		case OTRL_MSGEVENT_SETUP_ERROR:
			return tr("Error during setting up private conversation with %1: %2").arg(peerDisplay).arg(gpgErrorString(errorCode));
		case OTRL_MSGEVENT_MSG_REFLECTED:
			return tr("We are receiving our own OTR messages from %1. Either restart or close your private conversation.").arg(peerDisplay);
		case OTRL_MSGEVENT_MSG_RESENT:
			return tr("Last message was resent: %1").arg(message);
		case OTRL_MSGEVENT_RCVDMSG_NOT_IN_PRIVATE:
			return tr("Message from %1 was unreadable, as you are not currently communicating privately.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_UNREADABLE:
			return tr("Message from %1 was unreadable.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_MALFORMED:
			return tr("Message from %1 was malformed.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_GENERAL_ERR:
			return tr("Encryption error: %1").arg(message);
		case OTRL_MSGEVENT_RCVDMSG_UNENCRYPTED:
			return tr("Message from %1 was unencrypted.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_UNRECOGNIZED:
			return tr("Message from %1 was unrecognized.").arg(peerDisplay);
		case OTRL_MSGEVENT_RCVDMSG_FOR_OTHER_INSTANCE:
			return tr("%1 has sent a message intended for different session. "
					  "If you are logged in multiple times another session may have received the message.").arg(peerDisplay);
		default:
			return QString();
	}
}

QString OtrMessageEventService::gpgErrorString(gcry_error_t errorCode) const
{
	switch (errorCode)
	{
		case 0:
		case GPG_ERR_INV_VALUE:
			return (tr("Malformed message received"));
		default:
			return gcry_strerror(errorCode);
	}
}
