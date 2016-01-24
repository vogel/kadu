/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "chat/chat-manager.h"
#include "chat/model/chat-list-model.h"
#include "chat/model/chat-manager-adapter.h"
#include "core/injected-factory.h"
#include "core/myself.h"

#include "talkable-model.h"

TalkableModel::TalkableModel(QObject *parent) :
		KaduMergedProxyModel{parent},
		m_includeMyself{false}
{
}

TalkableModel::~TalkableModel()
{
}

void TalkableModel::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void TalkableModel::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void TalkableModel::setMyself(Myself *myself)
{
	m_myself = myself;
}

void TalkableModel::init()
{
	m_chats = m_injectedFactory->makeOwned<ChatListModel>(this);
	m_injectedFactory->makeOwned<ChatManagerAdapter>(m_chatManager, m_chats);
	m_buddies = m_injectedFactory->makeOwned<BuddyListModel>(this);
	m_buddiesAdapter = m_injectedFactory->makeOwned<BuddyManagerAdapter>(m_buddies);

	auto models = QList<QAbstractItemModel *>{};
	models.append(m_chats);
	models.append(m_buddies);
	setModels(models);
}

void TalkableModel::setIncludeMyself(bool includeMyself)
{
	if (m_includeMyself == includeMyself)
		return;

	m_includeMyself = includeMyself;
	if (m_includeMyself)
		m_buddies->addBuddy(m_myself->buddy());
	else
		m_buddies->removeBuddy(m_myself->buddy());
}

bool TalkableModel::includeMyself() const
{
	return m_includeMyself;
}

#include "moc_talkable-model.cpp"
