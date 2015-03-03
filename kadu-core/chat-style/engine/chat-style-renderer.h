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

#include "chat-style/engine/chat-style-renderer-configuration.h"
#include "message/message-common.h"
#include "protocols/services/chat-state-service.h"

#include <QtCore/QVector>

class ChatImage;
class Message;
class MessageRenderInfo;

/**
 * @addtogroup ChatStyle
 * @{
 */

/**
 * @class ChatStyleRenderer
 * @short Abstract renderer for displaying messages in QWebFrame.
 * @todo Maybe provide separate interfaces for non-ready and ready renderers?
 *
 * Renderer after creation may need some time for preparation. When it is ready it emits ready() signal
 * and isReady() method starts returning true. Always check for isReady() before performing any operations.
 */
class ChatStyleRenderer : public QObject
{
	Q_OBJECT

public:
	/**
	 * @short Create new instance of ChatStyleRenderer with given configuration and parent.
	 * @param configuration configuration of ChatStyleRenderer
	 * @param parent QObject parent
	 */
	explicit ChatStyleRenderer(ChatStyleRendererConfiguration configuration, QObject *parent = nullptr);
	virtual ~ChatStyleRenderer();

	/**
	 * @return true if object can accept commands.
	 */
	bool isReady() const;

	/**
	 * @short Clear all messages in view.
	 */
	virtual void clearMessages() = 0;

	/**
	 * @short Append new message to view.
	 * @param message message to append.
	 * @param messageRenderInfo render info to use when rendering.
	 */
	virtual void appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo) = 0;

	/**
	 * @short Remove first message from view.
	 */
	virtual void removeFirstMessage() = 0;

	/**
	 * @short Display new status for message.
	 * @param id id of message that has new status.
	 * @param messageStatus new status of message.
	 */
	virtual void displayMessageStatus(const QString &id, MessageStatus messageStatus) = 0;

	/**
	 * @short Display new activity of contact.
	 * @param state new activity state of contact.
	 * @param message message to display.
	 * @param contactDisplayName display name of contact.
	 */
	virtual void displayChatState(ChatState state, const QString &message, const QString &contactDisplayName) = 0;

	/**
	 * @short Display chat image.
	 * @param chatImage chat image to be displayed.
	 * @param fileName name of file that contains image data.
	 */
	virtual void displayChatImage(const ChatImage &chatImage, const QString &fileName) = 0;

signals:
	/**
	 * @short Signal emitted when renderer is ready to work.
	 */
	void ready();

protected:
	/**
	 * @return Configuration object of renderer.
	 */
	const ChatStyleRendererConfiguration & configuration() const;

protected slots:
	void setReady();

private:
	ChatStyleRendererConfiguration m_configuration;
	bool m_ready;

};

/**
 * @}
 */
