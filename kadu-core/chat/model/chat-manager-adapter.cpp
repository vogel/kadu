/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/model/chat-list-model.h"
#include "chat/chat-manager.h"

#include "chat-manager-adapter.h"

ChatManagerAdapter::ChatManagerAdapter(ChatListModel *model) :
		QObject(model), Model(model)
{
	Model->setChats(ChatManager::instance()->allItems().values().toVector());

	ChatManager *manager = ChatManager::instance();
	connect(manager, SIGNAL(chatAdded(Chat)),
			this, SLOT(chatAdded(Chat)), Qt::DirectConnection);
	connect(manager, SIGNAL(chatRemoved(Chat)),
			this, SLOT(chatRemoved(Chat)), Qt::DirectConnection);
}

ChatManagerAdapter::~ChatManagerAdapter()
{
	ChatManager *manager = ChatManager::instance();
	disconnect(manager, 0, this, 0);
}

void ChatManagerAdapter::chatAdded(const Chat &chat)
{
	Model->addChat(chat);
}

void ChatManagerAdapter::chatRemoved(const Chat &chat)
{
	Model->removeChat(chat);
}
