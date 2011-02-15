/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "contacts/contact.h"
#include "contacts/contact-manager.h"

#include "gadu-protocol.h"

#include "gadu-chat-state-service.h"

GaduChatStateService::GaduChatStateService(GaduProtocol *parent) :
	ChatStateService(parent), Protocol(parent)
{

}

#ifdef GADU_HAVE_TYPING_NOTIFY
void GaduChatStateService::handleEventTypingNotify(struct gg_event *e)
{
	Contact contact = ContactManager::instance()->byId(Protocol->account(), QString::number(e->event.typing_notification.uin, ActionReturnNull));
	if (!contact)
		return;

	if (e->event.typing_notification.length == 0x0001)
		emit contactActivityChanged(StateComposing, contact);
	else if (e->event.typing_notification.length == 0x0000)
		emit contactActivityChanged(StatePaused, contact);
}
#endif // GADU_HAVE_TYPING_NOTIFY


bool GaduChatStateService::shouldSendEvent()
{
		return true;
}

void GaduChatStateService::composingStarted(const Chat &chat)
{
	Q_UNUSED(chat)
}

void GaduChatStateService::composingStopped(const Chat &chat)
{
	Q_UNUSED(chat)
}

void GaduChatStateService::chatWidgetClosed(const Chat &chat)
{
	Q_UNUSED(chat)
}

void GaduChatStateService::chatWidgetActivated(const Chat &chat)
{
	Q_UNUSED(chat)
}

void GaduChatStateService::chatWidgetDeactivated(const Chat &chat)
{
	Q_UNUSED(chat)
}
