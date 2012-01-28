/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "chat/model/chats-list-model.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "icons/kadu-icon.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "gui/widgets/timeline-chat-messages-view.h"
#include "model/dates-model-item.h"
#include "search/history-search-parameters.h"
#include "storage/history-messages-storage.h"
#include "chats-buddies-splitter.h"

#include "chat-history-tab.h"

ChatHistoryTab::ChatHistoryTab(QWidget *parent) :
		HistoryTab(true, parent)
{
}

ChatHistoryTab::~ChatHistoryTab()
{
}

void ChatHistoryTab::clearChatHistory()
{
	if (!talkableTree()->actionContext())
		return;

	const Chat &chat = talkableTree()->actionContext()->chat();
	if (!chat)
		return;

	if (historyMessagesStorage())
	{
		historyMessagesStorage()->deleteMessages(chat);
		updateData();
	}

	displayTalkable(Talkable(), false);
}

void ChatHistoryTab::modifyTalkablePopupMenu(const QScopedPointer<QMenu> &menu)
{
	if (!menu)
		return;

	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Chat History"), this, SLOT(clearChatHistory()));
}

void ChatHistoryTab::talkablesAvailable()
{
	if (!TalkableToSelect.isValidChat())
		return;

	QModelIndexList indexesToSelect;

	Chat chat = TalkableToSelect.toChat();
	TalkableToSelect = Talkable();

	if (chat.contacts().size() == 1)
		indexesToSelect = modelChain()->indexListForValue(chat.contacts().begin()->ownerBuddy());
	else if (chat.contacts().size() > 1)
		indexesToSelect = modelChain()->indexListForValue(chat);

	if (1 == indexesToSelect.size())
	{
		talkableTree()->selectionModel()->select(indexesToSelect.at(0), QItemSelectionModel::ClearAndSelect);
		talkableTree()->scrollTo(indexesToSelect.at(0), QAbstractItemView::EnsureVisible);
		displayTalkable(chat, false);
	}
	else
		talkableTree()->selectionModel()->select(QModelIndex(), QItemSelectionModel::ClearAndSelect);
}

void ChatHistoryTab::updateData()
{
	setMessages(QVector<Message>());

	if (!historyMessagesStorage())
	{
		setTalkables(QVector<Talkable>());
		displayTalkable(Talkable(), false);
		return;
	}

	setFutureTalkables(historyMessagesStorage()->talkables());
}

void ChatHistoryTab::selectTalkable(const Talkable &talkable)
{
	TalkableToSelect = talkable;
}
