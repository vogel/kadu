/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/widgets/webkit-messages-view/message-limiter.h"
#include "message/message.h"
#include "message/sorted-messages.h"
#include "misc/memory.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

#include <QtCore/QObject>

class ChatImage;
class ChatStyleRenderer;
class MessageLimiter;
class WebkitMessagesViewDisplay;

enum class MessageLimitPolicy;

/**
 * @addtogroup WebkitMessagesView
 * @{
 */

/**
 * @class WebkitMessagesViewHandler
 * @short Class that wraps together ChatStyleRenderer and WebkitMessagesViewDisplay into one place.
 *
 * This class is used to connect ChatStyleRenderer with WebkitMessagesViewDisplay. It
 * stores list of messages that should be displayed in renderer and passes them to it as soon
 * as renderer is ready. It also accepts new messages into list and removes old ones based on set
 * MessageLimitPolicy and messages limit.
 */
class KADUAPI WebkitMessagesViewHandler : public QObject
{
	Q_OBJECT

public:
	/**
	 * @short Create new instance of WebkitMessagesViewHandler.
	 * @param chatStyleRenderer Renderer that will be used to render messages.
	 * @param messagesDisplay Object to handle order of rendering messages.
	 * @param parent QObject parent.
	 */
	explicit WebkitMessagesViewHandler(not_owned_qptr<ChatStyleRenderer> chatStyleRenderer,
			std::unique_ptr<WebkitMessagesViewDisplay> messagesDisplay, QObject *parent = nullptr);
	virtual ~WebkitMessagesViewHandler();

	/**
	 * @short Set limit of messages to display.
	 * @param limit New limit of messages to display.
	 * @todo Think of a way to combine it with setMessageLimitPolicy - currently impossible due to setForcePruneDisabled.
	 *
	 * If limit is not zero and current MessageLimitPolicy is set to MessageLimitPolicy::Value current list of messages
	 * will be trimmed to have no more elements than @p limit. This trimming will occur each time new message is added.
	 * Only messages from begining of list are removed.
	 */
	void setMessageLimit(unsigned limit);

	/**
	 * @short Set message limit policy.
	 * @param messageLimitPolicy New message limit policy.
	 *
	 * See effects of setMessageLimit(unsigned).
	 */
	void setMessageLimitPolicy(MessageLimitPolicy messageLimitPolicy);

	/**
	 * @return List of current messages.
	 *
	 * This list may be different from list of displayed messages if ChatStyleRenderer
	 * is still not ready for accepting commands.
	 */
	const SortedMessages & messages() const { return m_messages; }

	/**
	 * @short Add new message to list and try to display it.
	 * @param message Message to add.
	 *
	 * Message is added in such a way that chronological order is kept. If message limiting
	 * is on, then some messages may be removed from begining of new list before updating display. In
	 * some cases just added message can be removed (if it is earliest on the list).
	 */
	void add(const Message &message);

	/**
	 * @short Add number of messages to lsit and try to display them.
	 * @param message Sorted list of messages to add.
	 *
	 * Message are added in such a way that chronological order is kept. If message limiting
	 * is on, then some messages may be removed from begining of new list before updating display. In
	 * some cases a few of added message can be removed (if it is earliest on the list). To block
	 * this behavior, change message limit policy.
	 */
	void add(const SortedMessages &messages);

	/**
	 * @short Remove all messages from list.
	 */
	void clear();

	/**
	 * @short Display new status for given message.
	 * @param id Id of message.
	 * @param status Status of message.
	 *
	 * Status will only be displayed if ChatStyleRenderer is ready and message with this id is
	 * currently being displayed.
	 */
	void displayMessageStatus(const QString &id, MessageStatus status);

	/**
	 * @short Display new chat state.
	 * @param contact Contact that is responsible for state change.
	 * @param state New chat state.
	 *
	 * Chat state will only be displayed if ChatStyleRenderer is ready.
	 */
	void displayChatState(const Contact &contact, ChatStateService::State state);

	/**
	 * @short Display chat image.
	 * @param chatImage Chat image to display.
	 * @param fileName Name of file that contains image data.
	 *
	 * This method passes to ChatStyleRenderer information about image that was just downloaded and
	 * saved. That makes possible for javascript code to replace placeholder image with real one.
	 */
	void displayChatImage(const ChatImage &chatImage, const QString &fileName);

private:
	not_owned_qptr<ChatStyleRenderer> m_chatStyleRenderer;
	std::unique_ptr<WebkitMessagesViewDisplay> m_messagesDisplay;

	MessageLimiter m_messagesLimiter;
	SortedMessages m_messages;

	void limitAndDisplayMessages();

private slots:
	void rendererReady();

};

/**
 * @}
 */
