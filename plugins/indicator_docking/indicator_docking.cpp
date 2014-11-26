/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "indicator_docking.h"

#include "avatars/avatar.h"
#include "chat/chat-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "message/message.h"
#include "message/unread-message-repository.h"

#include <libqmessagingmenu/qmessaging-menu-app.h>
#include <libqmessagingmenu/qmessaging-menu-source.h>

IndicatorDocking::IndicatorDocking(QObject *parent) :
	QObject{parent}
{
	m_messagingMenuApp = new QMessagingMenuApp("kadu.desktop", this);
	m_messagingMenuApp->registerMenu();

	connect(m_messagingMenuApp, SIGNAL(sourceActivated(QString)), this, SLOT(sourceActivated(QString)));
}

IndicatorDocking::~IndicatorDocking()
{
	disconnect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)), this, SLOT(unreadMessageAdded(Message)));
	disconnect(m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)), this, SLOT(unreadMessageRemoved(Message)));

	for (auto &&message : m_unreadMessageRepository->allUnreadMessages())
		unreadMessageRemoved(message);

	m_messagingMenuApp->unregisterMenu();
}

void IndicatorDocking::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void IndicatorDocking::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void IndicatorDocking::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;

	for (auto &&message : m_unreadMessageRepository->allUnreadMessages())
		unreadMessageAdded(message);

	connect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)), this, SLOT(unreadMessageAdded(Message)));
	connect(m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)), this, SLOT(unreadMessageRemoved(Message)));
}

void IndicatorDocking::unreadMessageAdded(const Message &message)
{
	auto id = message.messageChat().uuid().toString();
	auto label = title(message.messageChat());
	auto count = message.messageChat().unreadMessagesCount();
	auto avatar = message.messageChat().contacts().toContact().avatar(true);
	auto source = m_messagingMenuApp->addSource(id, label, avatar.filePath(), count);
	source->drawAttention();
}

void IndicatorDocking::unreadMessageRemoved(const Message &message)
{
	auto count = message.messageChat().unreadMessagesCount();
	if (count == 0)
	{
		auto id = message.messageChat().uuid().toString();
		auto source = m_messagingMenuApp->source(id);
		if (source)
		{
			source->removeAttention();
			m_messagingMenuApp->removeSource(id);
		}
	}
}

void IndicatorDocking::sourceActivated(const QString &id)
{
	auto chat = m_chatManager->byUuid(QUuid{id});
	if (!chat)
		return;

	m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
}

#include "moc_indicator_docking.cpp"
