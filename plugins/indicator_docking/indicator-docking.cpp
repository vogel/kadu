/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "indicator-docking.h"

#include "avatars/avatar.h"
#include "chat/chat-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "message/message.h"
#include "message/unread-message-repository.h"
#include "status/status-container.h"

#include <libqmessagingmenu/qmessaging-menu-app.h>
#include <libqmessagingmenu/qmessaging-menu-source.h>
#include <libqmessagingmenu/qmessaging-menu-status.h>

IndicatorDocking::IndicatorDocking(QObject *parent) :
	QObject{parent}
{
	m_messagingMenuApp = new QMessagingMenuApp{"kadu.desktop", this};
	m_messagingMenuApp->registerMenu();

	connect(m_messagingMenuApp, SIGNAL(sourceActivated(QString)), this, SLOT(sourceActivated(QString)));
	connect(m_messagingMenuApp, SIGNAL(statusChanged(QMessagingMenuStatus)), this, SLOT(statusChanged(QMessagingMenuStatus)));
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

void IndicatorDocking::setStatusContainer(StatusContainer *statusContainer)
{
	m_statusContainer = statusContainer;

	statusContainerUpdated(m_statusContainer);
	connect(m_statusContainer, SIGNAL(statusUpdated(StatusContainer*)), this, SLOT(statusContainerUpdated(StatusContainer*)));
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

void IndicatorDocking::statusChanged(QMessagingMenuStatus status)
{
	auto currentStatus = m_statusContainer->status();
	switch (status)
	{
		case QMessagingMenuStatus::Available:
			currentStatus.setType(StatusTypeOnline);
			break;
		case QMessagingMenuStatus::Away:
			currentStatus.setType(StatusTypeAway);
			break;
		case QMessagingMenuStatus::Busy:
			currentStatus.setType(StatusTypeDoNotDisturb);
			break;
		case QMessagingMenuStatus::Invisible:
			currentStatus.setType(StatusTypeInvisible);
			break;
		case QMessagingMenuStatus::Offline:
			currentStatus.setType(StatusTypeOffline);
			break;
	}

	m_statusContainer->setStatus(currentStatus, SourceUser);
}

void IndicatorDocking::statusContainerUpdated(StatusContainer *statusContainer)
{
	switch (statusContainer->status().type())
	{
		case StatusTypeFreeForChat:
		case StatusTypeOnline:
			m_messagingMenuApp->setStatus(QMessagingMenuStatus::Available);
			break;
		case StatusTypeAway:
			m_messagingMenuApp->setStatus(QMessagingMenuStatus::Away);
			break;
		case StatusTypeNotAvailable:
		case StatusTypeDoNotDisturb:
			m_messagingMenuApp->setStatus(QMessagingMenuStatus::Busy);
			break;
		case StatusTypeInvisible:
			m_messagingMenuApp->setStatus(QMessagingMenuStatus::Invisible);
			break;
		case StatusTypeOffline:
			m_messagingMenuApp->setStatus(QMessagingMenuStatus::Offline);
			break;
		default:
			break;
	}
}

#include "moc_indicator-docking.cpp"
