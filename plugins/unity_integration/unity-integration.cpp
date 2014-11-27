/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "unity-integration.h"

#include "libqunity/qunity.h"

#include "message/unread-message-repository.h"

UnityIntegration::UnityIntegration(QObject *parent) :
	QObject{parent}
{
	m_unity = new QUnity{"kadu.desktop", this};
}

UnityIntegration::~UnityIntegration()
{
	disconnect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)), this, SLOT(unreadMessagesChanged()));
	disconnect(m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)), this, SLOT(unreadMessagesChanged()));

	unreadMessagesChanged();
}

void UnityIntegration::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;

	unreadMessagesChanged();

	connect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)), this, SLOT(unreadMessagesChanged()));
	connect(m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)), this, SLOT(unreadMessagesChanged()));
}

void UnityIntegration::unreadMessagesChanged()
{
	m_unity->updateCount(m_unreadMessageRepository->allUnreadMessages().count());
}

#include "moc_unity-integration.cpp"
