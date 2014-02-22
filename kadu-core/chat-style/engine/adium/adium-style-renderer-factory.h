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

#pragma once

#include "chat-style/engine/chat-style-renderer-factory.h"

#include <QtCore/QPointer>

class AdiumStyle;
class MessageHtmlRendererService;

class AdiumStyleRendererFactory : public ChatStyleRendererFactory
{

public:
	explicit AdiumStyleRendererFactory(std::shared_ptr<AdiumStyle> style);
	virtual ~AdiumStyleRendererFactory();

	void setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService);

	virtual qobject_ptr<ChatStyleRenderer> createChatStyleRenderer(ChatStyleRendererConfiguration configuration) override;

private:
	QPointer<MessageHtmlRendererService> m_messageHtmlRendererService;

	std::shared_ptr<AdiumStyle> m_style;

};
