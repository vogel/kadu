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

#include "sorted-messages.h"

#include "chat/chat.h"
#include "formatted-string/formatted-string.h"

bool SortedMessages::earlier(const Message &left, const Message &right)
{
	if (left == right)
		return false;

	if (left.receiveDate() < right.receiveDate())
		return true;

	return false;
}

bool SortedMessages::same(const Message &left, const Message &right)
{
	if (left.isNull() && right.isNull())
		return true;

	if (left.isNull() || right.isNull()) // one is null, second one is not
		return false;

	if (left.type() != right.type())
		return false;

	// In our SQL history we store datetime with accuracy to one second,
	// while for received XMPP messages we have a millisecond accuracy.
	// So to have proper results, we need to truncate those additional milliseconds.
	if (left.receiveDate().toTime_t() != right.receiveDate().toTime_t())
		return false;

	if (left.messageChat() != right.messageChat())
		return false;

	if (left.messageSender() != right.messageSender())
		return false;

	if (*left.content() != *right.content())
		return false;

	return true;
}

SortedMessages::SortedMessages()
{
}

SortedMessages::SortedMessages(std::vector<Message> messages) :
		m_messages{std::move(messages)}
{
}

void SortedMessages::add(Message message)
{
	m_messages.add(std::move(message));
}

void SortedMessages::merge(const SortedMessages &sortedMessages)
{
	m_messages.merge(sortedMessages.messages());
}

const std::vector<Message> & SortedMessages::messages() const
{
	return m_messages.content();
}

Message SortedMessages::last() const
{
	return m_messages.empty()
			? Message::null
			: m_messages.content().back();
}

bool SortedMessages::empty() const
{
	return m_messages.empty();
}

std::vector<Message>::size_type SortedMessages::size() const
{
	return m_messages.size();
}

void SortedMessages::clear()
{
	m_messages.clear();
}

std::vector<Message>::const_iterator begin(const SortedMessages &sortedMessages)
{
	return begin(sortedMessages.messages());
}

std::vector<Message>::const_iterator end(const SortedMessages &sortedMessages)
{
	return end(sortedMessages.messages());
}
