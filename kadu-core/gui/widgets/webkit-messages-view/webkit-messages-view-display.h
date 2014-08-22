/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "message/sorted-messages.h"
#include "exports.h"

#include <QtCore/QPointer>

class ChatStyleRenderer;
class MessageRenderInfoFactory;

enum class MessageRenderHeaderBehavior;

/**
 * @addtogroup WebkitMessagesView
 * @{
 */

/**
 * @class WebkitMessagesViewDisplay
 * @short Class for displaying list of messages using ChatStyleRenderer.
 * @todo Make constructor parametr std::unique_ptr.
 *
 * This class assumes that it is the only object that operates on messages in
 * ChatStyleRenderer. Thanks to that it can optimize updating messages
 * by not refreshing whole view when not neccessary - new messages can be appended
 * and deleted from begining of the list can be removed one by one.
 *
 * This class is abstract - displayMessages(SortedMessages) must be reimplemented
 * by inheriting classes.
 */
class KADUAPI WebkitMessagesViewDisplay
{

public:
	/**
	 * @short Create new WebkitMessagesViewDisplay operating on provided ChatStyleRenderer.
	 * @param chatStyleRenderer Renderer used to display messages.
	 */
	explicit WebkitMessagesViewDisplay(ChatStyleRenderer &chatStyleRenderer);
	virtual ~WebkitMessagesViewDisplay();

	/**
	 * @short Set instance of MessageRenderInfoFactory service.
	 * @param messageRenderInfoFactory Instance of MessageRenderInfoFactory service.
	 *
	 * MessageRenderInfoFactory service is used to get information about how message
	 * should be rendered after previous one (with/without header, which fonts to use).
	 */
	void setMessageRenderInfoFactory(MessageRenderInfoFactory *messageRenderInfoFactory);

	/**
	 * @short Display new list of messages.
	 * @param messages New list of messages to display.
	 */
	virtual void displayMessages(SortedMessages messages) = 0;

protected:
	/**
	 * @return Renderer used to display messages.
	 */
	ChatStyleRenderer & chatStyleRenderer() const;

	using I = decltype(begin(std::declval<SortedMessages>()));
	/**
	 * @short Appends range of messages to current renderer.
	 * @param from Iterator to first message in range.
	 * @param to Iterator to one-past-last message in range.
	 * @param previousMessage Message that is displayed just before *from one (or null message).
	 * @param headerBehavior Flag that alters when messages are displayed with headers and when not.
	 */
	void appendMessagesRange(I from, I to, Message previousMessage, MessageRenderHeaderBehavior headerBehavior) const;

private:
	QPointer<MessageRenderInfoFactory> m_messageRenderInfoFactory;

	ChatStyleRenderer &m_chatStyleRenderer;

};

/**
 * @}
 */
