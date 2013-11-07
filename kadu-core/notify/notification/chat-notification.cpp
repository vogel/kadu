/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"

#include "chat-notification.h"

ChatNotification::ChatNotification(const Chat &chat, const QString &type, const KaduIcon &icon) :
		AccountNotification(chat.chatAccount(), type, icon), CurrentChat(chat)
{
	addCallback(tr("Chat"), SLOT(callbackAccept()), "callbackAccept()");
	addCallback(tr("Ignore"), SLOT(callbackDiscard()), "callbackDiscard()");
}

void ChatNotification::openChat()
{
	close();

	Core::instance()->chatWidgetManager()->openChat(chat(), OpenChatActivation::Activate);
}

void ChatNotification::callbackAccept()
{
	openChat();
}

#include "moc_chat-notification.cpp"
