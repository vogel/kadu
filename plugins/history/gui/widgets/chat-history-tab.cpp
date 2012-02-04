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

#include <QtCore/QModelIndexList>
#include <QtGui/QAbstractItemView>

#include "contacts/contact-set.h"
#include "gui/widgets/talkable-tree-view.h"
#include "model/model-chain.h"

#include "gui/widgets/timeline-chat-messages-view.h"
#include "storage/history-messages-storage.h"

#include "chat-history-tab.h"

ChatHistoryTab::ChatHistoryTab(QWidget *parent) :
		HistoryMessagesTab(parent)
{
	timelineView()->setTalkableVisible(false);
	timelineView()->setTitleVisible(true);

	setClearHistoryMenuItemTitle(tr("&Clear Chat History"));
}

ChatHistoryTab::~ChatHistoryTab()
{
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

void ChatHistoryTab::selectTalkable(const Talkable &talkable)
{
	TalkableToSelect = talkable;
}
