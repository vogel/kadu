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

#include "message/message.h"
#include "misc/sorted-unique-vector.h"
#include "exports.h"

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class SortedMessages
 * @short Vector of messages sorted by time.
 */
class KADUAPI SortedMessages
{

public:
	/**
	 * @short Return true is mesage \left is earlier than \right.
	 */
	static bool earlier(const Message &left, const Message &right);

	/**
	 * @short Return true is mesage \left is the same as \right.
	 *
	 * Messages are considered the same if they are the same object, or
	 * all of they properties are the same (timestamp properties are compared
	 * with one-second accuracy).
	 */
	static bool same(const Message &left, const Message &right);

	/**
	 * @short Create empty object.
	 */
	SortedMessages();

	/**
	 * @short Create object from provided vector of messages.
	 * @param messages messages to put into object
	 *
	 * All messages are sorted and non-unique ones are removed.
	 */
	explicit SortedMessages(std::vector<Message> messages);

	/**
	 * @short Add new message.
	 * @param message message to add
	 *
	 * Message is added in proper place using earlier method to determine order. If message
	 * is already on list it will not be added.
	 */
	void add(Message message);

	/**
	 * @short Merge with another sorted list of messages.
	 * @param sortedMessages sorted list to merge with
	 *
	 * All messages from @p sortedMessages are added in proper place using earlier method to
	 * determine order. All non-unique messages are discared.
	 */
	void merge(const SortedMessages &sortedMessages);

	/**
	 * @return Vector of all stored messages.
	 */
	const std::vector<Message> & messages() const;

	/**
	 * @return Last message on list or Message::null.
	 */
	Message last() const;

	/**
	 * @return true if no message is stored in list.
	 */
	bool empty() const;

	/**
	 * @return number of messsages on list.
	 */
	std::vector<Message>::size_type size() const;

	/**
	 * @short Removes all messages.
	 */
	void clear();

private:
	sorted_unique_vector<Message, SortedMessages::earlier, SortedMessages::same> m_messages;

};

/**
 * @return begin iterator to content of SortedMessages.
 */
KADUAPI std::vector<Message>::const_iterator begin(const SortedMessages &sortedMessages);

/**
 * @return end iterator to content of SortedMessages.
 */
KADUAPI std::vector<Message>::const_iterator end(const SortedMessages &sortedMessages);

/**
 * @}
 */
