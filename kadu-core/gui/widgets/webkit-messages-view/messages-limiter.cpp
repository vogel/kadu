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

#include "messages-limiter.h"

MessagesLimiter::MessagesLimiter() :
		m_limit{0}
{
}

void MessagesLimiter::setLimit(unsigned int limit)
{
	m_limit = limit;
}

SortedMessages MessagesLimiter::limitMessages(SortedMessages sortedMessages)
{
	if (m_limit == 0 || sortedMessages.size() <= m_limit)
		return sortedMessages;

	auto messages = decltype(sortedMessages.messages()){};
	std::copy(end(sortedMessages) - m_limit, end(sortedMessages), std::back_inserter(messages));
	return SortedMessages{messages};
}
