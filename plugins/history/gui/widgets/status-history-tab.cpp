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

#include "buddies/buddy-set.h"
#include "gui/actions/action-context.h"

#include "storage/history-messages-storage.h"

#include "status-history-tab.h"

StatusHistoryTab::StatusHistoryTab(QWidget *parent) :
		HistoryTab(false, parent)
{
	setClearHistoryMenuItemTitle(tr("&Clear Status History"));
}

StatusHistoryTab::~StatusHistoryTab()
{
}

void StatusHistoryTab::clearTalkableHistory(ActionContext *actionContext)
{
	Q_ASSERT(actionContext);
	Q_ASSERT(historyMessagesStorage());

	const BuddySet &buddies = actionContext->buddies();
	foreach (const Buddy &buddy, buddies)
		historyMessagesStorage()->deleteMessages(buddy);
}
