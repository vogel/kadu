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

#include "chat-widget-factory.h"

#include "formatted-string/formatted-string-factory.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "misc/memory.h"

ChatWidgetFactory::ChatWidgetFactory(QObject *parent) :
		QObject(parent)
{
}

ChatWidgetFactory::~ChatWidgetFactory()
{
}

void ChatWidgetFactory::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

std::unique_ptr<ChatWidget> ChatWidgetFactory::createChatWidget(Chat chat)
{
	auto result = make_unique<ChatWidget>(chat);
	result.get()->setFormattedStringFactory(m_formattedStringFactory.data());

	return result;
}

#include "moc_chat-widget-factory.cpp"
