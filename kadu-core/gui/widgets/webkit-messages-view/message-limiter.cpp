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

#include "message-limiter.h"

#include "gui/widgets/webkit-messages-view/message-limit-policy.h"

MessageLimiter::MessageLimiter() :
		m_limit{0},
		m_messageLimitPolicy{MessageLimitPolicy::None}
{
}

void MessageLimiter::setLimit(unsigned int limit)
{
	m_limit = limit;
}

void MessageLimiter::setLimitPolicy(MessageLimitPolicy messageLimitPolicy)
{
	m_messageLimitPolicy = messageLimitPolicy;
}

SortedMessages MessageLimiter::limitMessages(SortedMessages sortedMessages) const
{
	if (m_limit == 0 || MessageLimitPolicy::None == m_messageLimitPolicy || sortedMessages.size() <= m_limit)
		return sortedMessages;

	auto messages = decltype(sortedMessages.messages()){};
	std::copy(end(sortedMessages) - m_limit, end(sortedMessages), std::back_inserter(messages));
	return SortedMessages{messages};
}
