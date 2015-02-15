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

#include "gui/widgets/webkit-messages-view/webkit-messages-view-display.h"

#include "message/sorted-messages.h"
#include "exports.h"

/**
 * @addtogroup WebkitMessagesView
 * @{
 */

/**
 * @class WebkitMessagesViewRemovingDisplay
 * @short Implementation of WebkitMessagesViewDisplay that tries to not clear view when possible.
 *
 * This implementation of WebkitMessagesViewDisplay does not clear view if only changes in messages
 * list are at begining (removing of old messages) and at end (appending new messages). This can only
 * by done when messages headers are configured to appear always.
 */
class KADUAPI WebkitMessagesViewRemovingDisplay : public WebkitMessagesViewDisplay
{

public:
	/**
	 * @short Create new WebkitMessagesViewRemovingDisplay operating on provided ChatStyleRenderer.
	 * @param chatStyleRenderer Renderer used to display messages.
	 */
	explicit WebkitMessagesViewRemovingDisplay(ChatStyleRenderer &chatStyleRenderer);
	virtual ~WebkitMessagesViewRemovingDisplay();

	/**
	 * @short Display new list of messages.
	 * @param messages New list of messages to display.
	 *
	 * If new list contains the same set of messages that previous one with some additional
	 * ones at the end and some removed from beggigin, new messages are appended to view
	 * and missing ones from begining are removed. In other case, view is cleared,
	 * and all messages are displayed again.
	 *
	 * If new list of messages is exactly the same as old one nothing is done.
	 */
	virtual void displayMessages(SortedMessages messages) override;

private:
	SortedMessages m_currentMessages;

};

/**
 * @}
 */
