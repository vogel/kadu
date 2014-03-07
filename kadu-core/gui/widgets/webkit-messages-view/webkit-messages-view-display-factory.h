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

#include "exports.h"

#include <memory>
#include <QtCore/QObject>
#include <QtCore/QPointer>

class ChatStyleManager;
class ChatStyleRenderer;
class MessageRenderInfoFactory;
class WebkitMessagesViewDisplay;

/**
 * @addtogroup WebkitMessagesView
 * @{
 */

/**
 * @class WebkitMessagesViewDisplayFactory
 * @short Factory for creating instances of @see WebkitMessagesViewDisplay.
 *
 * This factory uses @see ChatStyleManager to select one of implementations of
 * @see WebkitMessagesViewDisplay to create.
 */
class KADUAPI WebkitMessagesViewDisplayFactory : public QObject
{
	Q_OBJECT

public:
	/**
	 * @short Create new instance of WebkitMessagesViewDisplayFactory.
	 * @param parent QObject parent of service.
	 */
	explicit WebkitMessagesViewDisplayFactory(QObject *parent = nullptr);
	virtual ~WebkitMessagesViewDisplayFactory();

	/**
	 * @short Set ChatStyleManager service instance.
	 * @param chatStyleManager ChatStyleManager service instance.
	 *
	 * This service is used to select one of implementations of @see WebkitMessagesViewDisplay.
	 */
	void setChatStyleManager(ChatStyleManager *chatStyleManager);

	/**
	 * @short Set MessageRenderInfoFactory service instance.
	 * @param messageRenderInfoFactory MessageRenderInfoFactory service instance.
	 *
	 * This service is passed to created instances of @see WebkitMessagesViewDisplay.
	 */
	void setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory);

	/**
	 * @short Create new @see WebkitMessagesViewDisplay instance for given @see ChatStyleRenderer.
	 * @param chatStyleRenderer @see ChatStyleRenderer to create @see WebkitMessagesViewDisplay for.
	 *
	 * Implementation of @see WebkitMessagesViewDisplay is selected based on configuration
	 * read from @see ChatStyleManager service.
	 */
	std::unique_ptr<WebkitMessagesViewDisplay> createWebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer);

private:
	QPointer<ChatStyleManager> m_chatStyleManager;
	QPointer<MessageRenderInfoFactory> m_messageRenderInfoFactory;

};

/**
 * @}
 */
