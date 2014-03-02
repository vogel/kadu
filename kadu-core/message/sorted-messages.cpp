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

#include <algorithm>

SortedMessages::SortedMessages()
{
}

SortedMessages::SortedMessages(std::vector<Message> messages) :
		m_messages{std::move(messages)}
{
	std::stable_sort(std::begin(m_messages), std::end(m_messages), SortedMessages::precedes);
}

void SortedMessages::add(Message message)
{
	if (m_messages.empty())
		m_messages.emplace_back(std::move(message));

	auto upperBound = std::upper_bound(std::begin(m_messages), std::end(m_messages), message, SortedMessages::precedes);
	auto previous = *(upperBound - 1);
	if (!sameMessage(previous, message))
		m_messages.emplace(upperBound, std::move(message));
}

void SortedMessages::add(const SortedMessages &sortedMessages)
{
	auto result = std::vector<Message>{};

	std::merge(std::begin(m_messages), std::end(m_messages),
		std::begin(sortedMessages.m_messages), std::end(sortedMessages.m_messages),
		std::back_inserter(result), SortedMessages::precedes);
	result.erase(std::unique(std::begin(result), std::end(result), sameMessage), std::end(result));

	m_messages = std::move(result);
}

const std::vector<Message> & SortedMessages::messages() const
{
	return m_messages;
}

Message SortedMessages::last() const
{
	return m_messages.empty()
			? Message::null
			: m_messages.back();
}

bool SortedMessages::empty() const
{
	return m_messages.empty();
}

std::size_t SortedMessages::size() const
{
	return m_messages.size();
}

void SortedMessages::clear()
{
	m_messages.clear();
}

bool SortedMessages::precedes(const Message &left, const Message &right)
{
	if (left == right)
		return false;

	if (left.sendDate().toTime_t() < right.sendDate().toTime_t())
		return true;

	return false;
}

std::vector<Message>::const_iterator begin(const SortedMessages &sortedMessages)
{
	return std::begin(sortedMessages.messages());
}

std::vector<Message>::const_iterator end(const SortedMessages &sortedMessages)
{
	return std::end(sortedMessages.messages());
}
