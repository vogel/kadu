/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFutureWatcher>
#include <QtGui/QAction>
#include <QtGui/QMenu>

#include "chat/buddy-chat-manager.h"
#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"
#include "message/sorted-messages.h"

#include "gui/windows/history-window.h"
#include "history-messages-prepender.h"
#include "history-query.h"
#include "history.h"

#include "show-history-action-description.h"

ShowHistoryActionDescription::ShowHistoryActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeUser);
	setName("showHistoryAction");
	setIcon(KaduIcon("kadu_icons/history"));
	setText(tr("View Chat History"));
	setShortcut("kadu_viewhistory");

	registerAction();

	configurationUpdated();
}

ShowHistoryActionDescription::~ShowHistoryActionDescription()
{
}

void ShowHistoryActionDescription::configurationUpdated()
{
	ActionDescription::configurationUpdated();

	ChatHistoryQuotationTime = config_file.readNumEntry("History", "ChatHistoryQuotationTime", -24);
}

void ShowHistoryActionDescription::actionInstanceCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox || !chatEditBox->chatWidget())
		return;

	QVariant chatWidgetData = QVariant::fromValue(chatEditBox->chatWidget());
	action->setData(chatWidgetData);

	// not a menu
	if (action->context()->chat() != chatEditBox->actionContext()->chat())
		return;

	// no parents for menu as it is destroyed manually by Action class
	QMenu *menu = new QMenu();

	if (config_file.readNumEntry("History", "ChatHistoryCitation", 10) > 0)
	{
		int prune = config_file.readNumEntry("History", "ChatHistoryCitation", 10);
		menu->addAction(tr("Show last %1 messages").arg(prune), this, SLOT(showPruneMessages()))->setData(chatWidgetData);
		menu->addSeparator();
	}

	menu->addAction(tr("Show messages since yesterday"), this, SLOT(showOneDayMessages()))->setData(chatWidgetData);
	menu->addAction(tr("Show messages from last 7 days"), this, SLOT(show7DaysMessages()))->setData(chatWidgetData);
	menu->addAction(tr("Show messages from last 30 days"), this, SLOT(show30DaysMessages()))->setData(chatWidgetData);
	menu->addAction(tr("Show whole history"), this, SLOT(showAllMessages()))->setData(chatWidgetData);

	action->setMenu(menu);
}

void ShowHistoryActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	showDaysMessages(sender, -1);
}

void ShowHistoryActionDescription::showPruneMessages()
{
	showDaysMessages(qobject_cast<QAction *>(sender()), 0);
}

void ShowHistoryActionDescription::showOneDayMessages()
{
	showDaysMessages(qobject_cast<QAction *>(sender()), 1);
}

void ShowHistoryActionDescription::show7DaysMessages()
{
	showDaysMessages(qobject_cast<QAction *>(sender()), 7);
}

void ShowHistoryActionDescription::show30DaysMessages()
{
	showDaysMessages(qobject_cast<QAction *>(sender()), 30);
}

void ShowHistoryActionDescription::showAllMessages()
{
	showDaysMessages(qobject_cast<QAction *>(sender()), -1);
}

void ShowHistoryActionDescription::showDaysMessages(QAction *action, int days)
{
	Action *act = qobject_cast<Action *>(action);
	Chat actionChat = act ? act->context()->chat() : Chat::null;

	ChatWidget *chatWidget = action->data().value<ChatWidget *>();
	if (!chatWidget)
	{
		HistoryWindow::show(actionChat);
		return;
	}

	WebkitMessagesView *chatMessagesView = chatWidget->chatMessagesView();
	if (!chatMessagesView)
	{
		HistoryWindow::show(actionChat);
		return;
	}

	chatMessagesView->setForcePruneDisabled(0 != days);

	if (-1 == days)
	{
		HistoryWindow::show(chatWidget->chat());
		return;
	}

	const Chat &buddyChat = BuddyChatManager::instance()->buddyChat(chatWidget->chat());
	const Chat &messagesChat = buddyChat ? buddyChat : chatWidget->chat();
	HistoryStorage *historyStorage = History::instance()->currentStorage();

	if (!historyStorage)
		return;

	HistoryQuery query;
	query.setTalkable(messagesChat);

	if (0 == days)
		query.setLimit(config_file.readUnsignedNumEntry("History", "ChatHistoryCitation", 10));
	else
		query.setFromDate(QDate::currentDate().addDays(-days));

	new HistoryMessagesPrepender(historyStorage->messages(query), chatMessagesView);
}

#include "moc_show-history-action-description.cpp"
