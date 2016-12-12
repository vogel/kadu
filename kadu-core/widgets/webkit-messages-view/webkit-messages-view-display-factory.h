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

#pragma once

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <memory>

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
 * @short Factory for creating instances of WebkitMessagesViewDisplay.
 *
 * This factory uses ChatStyleManager to select one of implementations of
 * WebkitMessagesViewDisplay to create.
 */
class KADUAPI WebkitMessagesViewDisplayFactory : public QObject
{
	Q_OBJECT

public:
	/**
	 * @short Create new instance of WebkitMessagesViewDisplayFactory.
	 * @param parent QObject parent of service.
	 */
	Q_INVOKABLE explicit WebkitMessagesViewDisplayFactory(QObject *parent = nullptr);
	virtual ~WebkitMessagesViewDisplayFactory();

	/**
	 * @short Create new WebkitMessagesViewDisplay instance for given ChatStyleRenderer.
	 * @param chatStyleRenderer ChatStyleRenderer to create WebkitMessagesViewDisplay for.
	 *
	 * Implementation of WebkitMessagesViewDisplay is selected based on configuration
	 * read from ChatStyleManager service.
	 */
	std::unique_ptr<WebkitMessagesViewDisplay> createWebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer);

private:
	QPointer<ChatStyleManager> m_chatStyleManager;
	QPointer<MessageRenderInfoFactory> m_messageRenderInfoFactory;

private slots:
	/**
	 * @short Set ChatStyleManager service instance.
	 * @param chatStyleManager ChatStyleManager service instance.
	 *
	 * This service is used to select one of implementations of WebkitMessagesViewDisplay.
	 */
	INJEQT_SET void setChatStyleManager(ChatStyleManager *chatStyleManager);

	/**
	 * @short Set MessageRenderInfoFactory service instance.
	 * @param messageRenderInfoFactory MessageRenderInfoFactory service instance.
	 *
	 * This service is passed to created instances of WebkitMessagesViewDisplay.
	 */
	INJEQT_SET void setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory);

};

/**
 * @}
 */
