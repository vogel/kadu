/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QMenu>

#include "chat/aggregate-chat-manager.h"
#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"

#include "gui/windows/history-window.h"

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

	ChatHistoryQuotationTime = config_file.readNumEntry("History", "ChatHistoryQuotationTime", -744);
}

void ShowHistoryActionDescription::actionInstanceCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	Chat chat = action->chat();

	// not a menu
	if (!chatEditBox || chat != chatEditBox->chat())
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	// TODO: check if this is proper parenting
	QMenu *menu = new QMenu(qobject_cast<QWidget *>(action->parent()));

	if (config_file.readBoolEntry("Chat", "ChatPrune", false))
	{
		int prune = config_file.readNumEntry("Chat", "ChatPruneLen", 20);
		menu->addAction(tr("Show last %1 messages").arg(prune))->setData(0);
		menu->addSeparator();
	}

	menu->addAction(tr("Show messages since yesterday"))->setData(1);
	menu->addAction(tr("Show messages from last 7 days"))->setData(7);
	menu->addAction(tr("Show messages from last 30 days"))->setData(30);
	menu->addAction(tr("Show whole history"))->setData(-1);

	connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(showMoreMessages(QAction *)));

	action->setMenu(menu);
}

void ShowHistoryActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	Chat chat = action->chat();
	HistoryWindow::show(chat);
}

void ShowHistoryActionDescription::showMoreMessages(QAction *action)
{
	if (!History::instance()->currentStorage())
		return;

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender()->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	bool ok;
	int days = action->data().toInt(&ok);

	if (!ok)
		return;

	ChatMessagesView *chatMessagesView = chatWidget->chatMessagesView();
	if (!chatMessagesView)
		return;

	Chat chat = AggregateChatManager::instance()->aggregateChat(chatWidget->chat());

	chatMessagesView->setForcePruneDisabled(0 != days);
	QVector<Message> messages;

	if (-1 == days)
	{
		HistoryWindow::show(chatWidget->chat());
		return;
	}
	else if (0 != days)
	{
		QDate since = QDate::currentDate().addDays(-days);
		messages = History::instance()->currentStorage()->messagesSince(chat ? chat : chatWidget->chat(), since);
	}
	else
	{
		QDateTime backTo = QDateTime::currentDateTime().addDays(ChatHistoryQuotationTime/24);
		messages = History::instance()->currentStorage()->messagesBackTo(chat ? chat : chatWidget->chat(), backTo, config_file.readNumEntry("Chat", "ChatPruneLen", 20));
	}

	chatMessagesView->clearMessages();
	chatMessagesView->appendMessages(messages);
}
