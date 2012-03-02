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
	ChatStateService(parent), Protocol(parent)
{
	if (Protocol->chatService())
		connect(Protocol->chatService(), SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));
}

void GaduChatStateService::messageReceived(const Message &message)
{
	// it seems it is what is also done and expected by GG10
	emit contactActivityChanged(StatePaused, message.messageSender());
}

void GaduChatStateService::handleEventTypingNotify(struct gg_event *e)
{
	Contact contact = ContactManager::instance()->byId(Protocol->account(), QString::number(e->event.typing_notification.uin), ActionReturnNull);
	if (!contact)
		return;

	if (e->event.typing_notification.length > 0x0000)
		emit contactActivityChanged(StateComposing, contact);
	else if (e->event.typing_notification.length == 0x0000)
		emit contactActivityChanged(StatePaused, contact);
}

bool GaduChatStateService::shouldSendEvent()
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());
	if (!gaduAccountDetails)
		return false;

	if (!gaduAccountDetails->sendTypingNotification())
		return false;

	return true;
}

void GaduChatStateService::composingStarted(const Chat &chat)
{
	if (!shouldSendEvent())
		return;

	Contact contact = chat.contacts().toContact();
	if (!contact)
		return;

	if (!Protocol->gaduSession())
		return;

	static_cast<GaduProtocol *>(Protocol)->disableSocketNotifiers();
	gg_typing_notification(Protocol->gaduSession(), GaduProtocolHelper::uin(contact), 0x0001);
	static_cast<GaduProtocol *>(Protocol)->enableSocketNotifiers();
}

void GaduChatStateService::composingStopped(const Chat &chat)
{
	if (!shouldSendEvent())
		return;

	Contact contact = chat.contacts().toContact();
	if (!contact)
		return;

	if (!Protocol->gaduSession())
		return;

	static_cast<GaduProtocol *>(Protocol)->disableSocketNotifiers();
	gg_typing_notification(Protocol->gaduSession(), GaduProtocolHelper::uin(contact), 0x0000);
	static_cast<GaduProtocol *>(Protocol)->enableSocketNotifiers();
}

void GaduChatStateService::chatWidgetClosed(const Chat &chat)
{
	composingStopped(chat);
}

void GaduChatStateService::chatWidgetActivated(const Chat &chat)
{
	Q_UNUSED(chat)
}

void GaduChatStateService::chatWidgetDeactivated(const Chat &chat)
{
	Q_UNUSED(chat)
}
