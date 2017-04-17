/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFutureWatcher>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

#include "actions/action.h"
#include "actions/actions.h"
#include "chat/buddy-chat-manager.h"
#include "chat/chat.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "message/sorted-messages.h"
#include "plugin/plugin-injected-factory.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/webkit-messages-view/webkit-messages-view.h"

#include "gui/windows/history-window-service.h"
#include "gui/windows/history-window.h"
#include "history-messages-prepender.h"
#include "history-query.h"
#include "history.h"

#include "show-history-action.h"
#include "show-history-action.moc"

ShowHistoryAction::ShowHistoryAction(QObject *parent) : ActionDescription(parent)
{
}

ShowHistoryAction::~ShowHistoryAction()
{
}

void ShowHistoryAction::setBuddyChatManager(BuddyChatManager *buddyChatManager)
{
    m_buddyChatManager = buddyChatManager;
}

void ShowHistoryAction::setHistoryWindowService(HistoryWindowService *historyWindowService)
{
    m_historyWindowService = historyWindowService;
}

void ShowHistoryAction::setHistory(History *history)
{
    m_history = history;
}

void ShowHistoryAction::init()
{
    setType(ActionDescription::TypeUser);
    setName("showHistoryAction");
    setIcon(KaduIcon("kadu_icons/history"));
    setText(tr("View Chat History"));
    setShortcut("kadu_viewhistory");

    configurationUpdated();
}

void ShowHistoryAction::configurationUpdated()
{
    ActionDescription::configurationUpdated();

    ChatHistoryQuotationTime =
        configuration()->deprecatedApi()->readNumEntry("History", "ChatHistoryQuotationTime", -24);
}

void ShowHistoryAction::actionInstanceCreated(Action *action)
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

    if (configuration()->deprecatedApi()->readNumEntry("History", "ChatHistoryCitation", 10) > 0)
    {
        int prune = configuration()->deprecatedApi()->readNumEntry("History", "ChatHistoryCitation", 10);
        menu->addAction(tr("Show last %1 messages").arg(prune), this, SLOT(showPruneMessages()))
            ->setData(chatWidgetData);
        menu->addSeparator();
    }

    menu->addAction(tr("Show messages since yesterday"), this, SLOT(showOneDayMessages()))->setData(chatWidgetData);
    menu->addAction(tr("Show messages from last 7 days"), this, SLOT(show7DaysMessages()))->setData(chatWidgetData);
    menu->addAction(tr("Show messages from last 30 days"), this, SLOT(show30DaysMessages()))->setData(chatWidgetData);
    menu->addAction(tr("Show whole history"), this, SLOT(showAllMessages()))->setData(chatWidgetData);

    action->setMenu(menu);
}

void ShowHistoryAction::actionTriggered(QAction *sender, bool toggled)
{
    Q_UNUSED(toggled)

    showDaysMessages(sender, -1);
}

void ShowHistoryAction::showPruneMessages()
{
    showDaysMessages(qobject_cast<QAction *>(sender()), 0);
}

void ShowHistoryAction::showOneDayMessages()
{
    showDaysMessages(qobject_cast<QAction *>(sender()), 1);
}

void ShowHistoryAction::show7DaysMessages()
{
    showDaysMessages(qobject_cast<QAction *>(sender()), 7);
}

void ShowHistoryAction::show30DaysMessages()
{
    showDaysMessages(qobject_cast<QAction *>(sender()), 30);
}

void ShowHistoryAction::showAllMessages()
{
    showDaysMessages(qobject_cast<QAction *>(sender()), -1);
}

void ShowHistoryAction::showDaysMessages(QAction *action, int days)
{
    Action *act = qobject_cast<Action *>(action);
    Chat actionChat = act ? act->context()->chat() : Chat::null;

    ChatWidget *chatWidget = action->data().value<ChatWidget *>();
    if (!chatWidget)
    {
        m_historyWindowService->show(actionChat);
        return;
    }

    WebkitMessagesView *chatMessagesView = chatWidget->chatMessagesView();
    if (!chatMessagesView)
    {
        m_historyWindowService->show(actionChat);
        return;
    }

    chatMessagesView->setForcePruneDisabled(0 != days);

    if (-1 == days)
    {
        m_historyWindowService->show(chatWidget->chat());
        return;
    }

    const Chat &buddyChat = m_buddyChatManager->buddyChat(chatWidget->chat());
    const Chat &messagesChat = buddyChat ? buddyChat : chatWidget->chat();
    HistoryStorage *historyStorage = m_history->currentStorage();

    if (!historyStorage)
        return;

    HistoryQuery query;
    query.setTalkable(messagesChat);

    if (0 == days)
        query.setLimit(configuration()->deprecatedApi()->readUnsignedNumEntry("History", "ChatHistoryCitation", 10));
    else
        query.setFromDate(QDate::currentDate().addDays(-days));

    new HistoryMessagesPrepender(historyStorage->messages(query), chatMessagesView);
}
