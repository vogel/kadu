/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/chat-widget/chat-widget-factory.h"

#include "chat-widget-repository.h"

ChatWidgetRepository::ChatWidgetRepository(QObject *parent) :
		QObject(parent)
{
}

ChatWidgetRepository::~ChatWidgetRepository()
{
}

void ChatWidgetRepository::setChatWidgetFactory(ChatWidgetFactory *chatWidgetFactory)
{
	m_chatWidgetFactory = chatWidgetFactory;
}

bool ChatWidgetRepository::hasWidgetForChat(const Chat &chat) const
{
	return m_widgets.contains(chat);
}

ChatWidget * ChatWidgetRepository::widgetForChat(const Chat &chat)
{
	if (hasWidgetForChat(chat))
		return m_widgets.value(chat);

	if (!m_chatWidgetFactory)
		return nullptr;

	auto result = m_chatWidgetFactory.data()->createChatWidget(chat);
	connect(result.get(), SIGNAL(widgetDestroyed(Chat)), this, SLOT(widgetDestroyed(Chat)));
	m_widgets.insert(chat, result.get());

	emit chatWidgetCreated(result.get());

	return result.release();
}

const QMap<Chat, ChatWidget *> & ChatWidgetRepository::widgets() const
{
	return m_widgets;
}

void ChatWidgetRepository::widgetDestroyed(const Chat &chat)
{
	if (!hasWidgetForChat(chat))
		return;

	emit chatWidgetDestroyed(m_widgets.value(chat));
	m_widgets.remove(chat);
}
