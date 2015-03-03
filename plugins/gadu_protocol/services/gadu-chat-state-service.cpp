/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/contact-manager.h"
#include "protocols/services/chat-state.h"

#include "helpers/gadu-protocol-helper.h"
#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"

#include "gadu-chat-state-service.h"

GaduChatStateService::GaduChatStateService(Account account, QObject *parent) :
		ChatStateService(account, parent), SendTypingNotifications(false)
{
}

GaduChatStateService::~GaduChatStateService()
{
}

void GaduChatStateService::setConnection(GaduConnection *connection)
{
	Connection = connection;
}

void GaduChatStateService::setSendTypingNotifications(bool sendTypingNotifications)
{
	SendTypingNotifications = sendTypingNotifications;
}

void GaduChatStateService::messageReceived(const Message &message)
{
	// it seems it is what is also done and expected by GG10
	emit peerStateChanged(message.messageSender(), ChatState::Paused);
}

void GaduChatStateService::handleEventTypingNotify(struct gg_event *e)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(e->event.typing_notification.uin), ActionReturnNull);
	if (!contact)
		return;

	if (e->event.typing_notification.length > 0x0000)
		emit peerStateChanged(contact, ChatState::Composing);
	else if (e->event.typing_notification.length == 0x0000)
		emit peerStateChanged(contact, ChatState::Paused);
}

void GaduChatStateService::sendState(const Contact &contact, ChatState state)
{
	if (!SendTypingNotifications || !contact)
		return;

	if (!Connection || !Connection.data()->hasSession())
		return;

	auto writableSessionToken = Connection.data()->writableSessionToken();
	switch (state)
	{
		case ChatState::Composing:
			gg_typing_notification(writableSessionToken.rawSession(), GaduProtocolHelper::uin(contact), 0x0001);
			break;
		case ChatState::Paused:
		case ChatState::Gone:
			gg_typing_notification(writableSessionToken.rawSession(), GaduProtocolHelper::uin(contact), 0x0000);
			break;
		default:
			break;
	}
}

#include "moc_gadu-chat-state-service.cpp"
