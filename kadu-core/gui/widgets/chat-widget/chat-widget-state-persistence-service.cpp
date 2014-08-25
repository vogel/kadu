/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-html-visitor.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/custom-input.h"

ChatWidgetStatePersistenceService::ChatWidgetStatePersistenceService()
{
}

ChatWidgetStatePersistenceService::~ChatWidgetStatePersistenceService()
{
}

void ChatWidgetStatePersistenceService::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetAdded(ChatWidget*)),
			this, SLOT(restoreChatWidgetState(ChatWidget*)));
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetRemoved(ChatWidget*)),
			this, SLOT(storeChatWidgetState(ChatWidget*)));
}

void ChatWidgetStatePersistenceService::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

void ChatWidgetStatePersistenceService::storeChatWidgetState(ChatWidget *chatWidget)
{
	auto content = chatWidget->edit()->formattedString();
	if (!content->isEmpty())
	{
		FormattedStringHtmlVisitor html{};
		content->accept(&html);
		chatWidget->chat().addProperty("chat-widget-state:message", html.result(), CustomProperties::Storable);
	}
	else
		chatWidget->chat().removeProperty("chat-widget-state:message");
}

void ChatWidgetStatePersistenceService::restoreChatWidgetState(ChatWidget *chatWidget)
{
	auto html = chatWidget->chat().property("chat-widget-state:message", QString{}).toString();
	auto formattedString = m_formattedStringFactory->fromHtml(html);
	chatWidget->edit()->setFormattedString(*formattedString);

	auto textCursor = chatWidget->edit()->textCursor();
	textCursor.movePosition(QTextCursor::End);
	chatWidget->edit()->setTextCursor(textCursor);
}
