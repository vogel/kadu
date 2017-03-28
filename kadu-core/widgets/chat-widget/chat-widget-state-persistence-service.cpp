/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-state-persistence-service.h"

#include "html/html-conversion.h"
#include "html/html-string.h"
#include "html/normalized-html-string.h"
#include "widgets/chat-widget/chat-widget-repository.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/custom-input.h"

ChatWidgetStatePersistenceService::ChatWidgetStatePersistenceService()
{
}

ChatWidgetStatePersistenceService::~ChatWidgetStatePersistenceService()
{
}

void ChatWidgetStatePersistenceService::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
    m_chatWidgetRepository = chatWidgetRepository;
}

void ChatWidgetStatePersistenceService::init()
{
    connect(
        m_chatWidgetRepository, &ChatWidgetRepository::chatWidgetAdded, this,
        &ChatWidgetStatePersistenceService::restoreChatWidgetState);
    connect(
        m_chatWidgetRepository, &ChatWidgetRepository::chatWidgetRemoved, this,
        &ChatWidgetStatePersistenceService::storeChatWidgetState);
}

void ChatWidgetStatePersistenceService::storeChatWidgetState(ChatWidget *chatWidget)
{
    auto content = chatWidget->edit()->htmlMessage();
    if (!content.string().isEmpty())
        chatWidget->chat().addProperty("chat-widget-state:message", content.string(), CustomProperties::Storable);
    else
        chatWidget->chat().removeProperty("chat-widget-state:message");
}

void ChatWidgetStatePersistenceService::restoreChatWidgetState(ChatWidget *chatWidget)
{
    auto html =
        normalizeHtml(HtmlString{chatWidget->chat().property("chat-widget-state:message", QString{}).toString()});
    chatWidget->edit()->setHtml(QString{R"(<div style="white-space: pre-wrap;">%1</div>)"}.arg(html.string()));

    auto textCursor = chatWidget->edit()->textCursor();
    textCursor.movePosition(QTextCursor::End);
    chatWidget->edit()->setTextCursor(textCursor);
}
