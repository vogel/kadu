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
 * @class WebkitMessagesViewClearingDisplay
 * @short Implementation of WebkitMessagesViewDisplay that clears view every time a message is removed.
 *
 * This implementation of WebkitMessagesViewDisplay clears view every time a message is removed
 * (even if from begining of list). This must be done when messages headers are configured to appear
 * only when neccessary as there is no way in styles to recreate header of message that didn't have one.
 */
class KADUAPI WebkitMessagesViewClearingDisplay : public WebkitMessagesViewDisplay
{

public:
	/**
	 * @short Create new WebkitMessagesViewClearingDisplay operating on provided ChatStyleRenderer.
	 * @param chatStyleRenderer Renderer used to display messages.
	 */
	explicit WebkitMessagesViewClearingDisplay(ChatStyleRenderer &chatStyleRenderer);
	virtual ~WebkitMessagesViewClearingDisplay();

	/**
	 * @short Display new list of messages.
	 * @param messages New list of messages to display.
	 *
	 * If new list contains the same set of messages that previous one with some additional
	 * ones at the end, new messages are appended to view. In other case, view is cleared,
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
