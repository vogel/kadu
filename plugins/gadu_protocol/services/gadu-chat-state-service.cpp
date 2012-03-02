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

#include "helpers/gadu-protocol-helper.h"

#include "gadu-chat-state-service.h"

GaduChatStateService::GaduChatStateService(Protocol *parent) :
		ChatStateService(parent), GaduSession(0), SendTypingNotifications(false)
{
}

GaduChatStateService::~GaduChatStateService()
{
}

void GaduChatStateService::setGaduSession(gg_session *gaduSession)
{
	GaduSession = gaduSession;
}

void GaduChatStateService::setSendTypingNotifications(bool sendTypingNotifications)
{
	SendTypingNotifications = sendTypingNotifications;
}

void GaduChatStateService::messageReceived(const Message &message)
{
	// it seems it is what is also done and expected by GG10
	emit peerStateChanged(message.messageSender(), StatePaused);
}

void GaduChatStateService::handleEventTypingNotify(struct gg_event *e)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(e->event.typing_notification.uin), ActionReturnNull);
	if (!contact)
		return;

	if (e->event.typing_notification.length > 0x0000)
		emit peerStateChanged(contact, StateComposing);
	else if (e->event.typing_notification.length == 0x0000)
		emit peerStateChanged(contact, StatePaused);
}

void GaduChatStateService::sendState(const Contact &contact, State state)
{
	if (!SendTypingNotifications || !contact)
		return;

	if (!GaduSession)
		return;

	static_cast<GaduProtocol *>(protocol())->disableSocketNotifiers();
	switch (state)
	{
		case StateComposing:
			gg_typing_notification(GaduSession, GaduProtocolHelper::uin(contact), 0x0001);
			break;
		case StatePaused:
		case StateGone:
			gg_typing_notification(GaduSession, GaduProtocolHelper::uin(contact), 0x0000);
			break;
		default:
			break;
	}
	static_cast<GaduProtocol *>(protocol())->enableSocketNotifiers();
}
