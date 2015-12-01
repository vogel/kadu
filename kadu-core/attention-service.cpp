/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "attention-service.h"

#include "message/unread-message-repository.h"

AttentionService::AttentionService(QObject *parent) :
		QObject{parent},
		m_unreadMessageRepository{nullptr}
{
}

AttentionService::~AttentionService()
{
}

void AttentionService::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;
	connect(m_unreadMessageRepository, SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageCountChanged()));
	connect(m_unreadMessageRepository, SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageCountChanged()));
}

bool AttentionService::needAttention()
{
	return m_unreadMessageRepository->hasUnreadMessages();
}

void AttentionService::unreadMessageCountChanged()
{
	emit needAttentionChanged(needAttention());
}

#include "moc_attention-service.cpp"