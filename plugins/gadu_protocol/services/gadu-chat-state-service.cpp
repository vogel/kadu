/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"

#include "helpers/gadu-protocol-helper.h"
#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "gadu-chat-state-service.h"

GaduChatStateService::GaduChatStateService(GaduProtocol *parent) :
		ChatStateService(parent), CurrentChatService(0), Protocol(parent)
{
}

GaduChatStateService::~GaduChatStateService()
{
}

void GaduChatStateService::setChatService(ChatService *chatService)
{
	if (CurrentChatService)
		disconnect(CurrentChatService, SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));

	CurrentChatService = chatService;

	if (CurrentChatService)
		connect(CurrentChatService, SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));
}

void GaduChatStateService::messageReceived(const Message &message)
{
	// it seems it is what is also done and expected by GG10
	emit peerStateChanged(message.messageSender(), StatePaused);
}

void GaduChatStateService::typingNotifyEventReceived(struct gg_event *e)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(e->event.typing_notification.uin), ActionReturnNull);
	if (!contact)
		return;

	if (e->event.typing_notification.length > 0x0000)
		emit peerStateChanged(contact, StateComposing);
	else if (e->event.typing_notification.length == 0x0000)
		emit peerStateChanged(contact, StatePaused);
}

bool GaduChatStateService::shouldSendEvent()
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return false;

	if (!gaduAccountDetails->sendTypingNotification())
		return false;

	return true;
}

void GaduChatStateService::sendState(const Contact &contact, State state)
{
	if (!shouldSendEvent() || !contact)
		return;

	if (!Protocol->gaduSession())
		return;

	switch (state)
	{
		case StateComposing:
			gg_typing_notification(Protocol->gaduSession(), GaduProtocolHelper::uin(contact), 0x0001);
			break;
		case StatePaused:
		case StateGone:
			gg_typing_notification(Protocol->gaduSession(), GaduProtocolHelper::uin(contact), 0x0000);
			break;
		default:
			break;
	}
}
