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

#include "message/sorted-messages.h"
#include "exports.h"

enum class MessageLimitPolicy;

/**
 * @addtogroup WebkitMessagesView
 * @{
 */

/**
 * @class MessageLimiter
 * @short Class for limiting number of messages.
 *
 * When policy is MessageLimitPolicy::Value and limit is set to number greater than
 * zero, this class will remove a few first messages from list to keep its size at most
 * at limit value.
 */
class KADUAPI MessageLimiter
{

public:
	/**
	 * @short Create new limiter with MessageLimitPolicy::None policy.
	 */
	MessageLimiter();

	/**
	 * @short Set new limit of number of messages.
	 * @param limit New limit of number of messages.
	 *
	 * This value only matters if greater than zero and limit policy is
	 * set to MessageLimitPolicy::Value.
	 */
	void setLimit(unsigned limit);

	/**
	 * @short Set new policy of limiter.
	 * @param messageLimitPolicy New policy of limiter.
	 */
	void setLimitPolicy(MessageLimitPolicy messageLimitPolicy);

	/**
	 * @short Return messages with limited number.
	 * @param sortedMessages Messages to limit.
	 * @return sortedMessages with few (or none) first messages removed.
	 *
	 * When policy is MessageLimitPolicy::Value and limit is set to number greater than
	 * zero, this method will return sortedMessages with first few messages removed, so size
	 * of returned value is at most at limit value.
	 */
	SortedMessages limitMessages(SortedMessages sortedMessages) const;

private:
	unsigned m_limit;
	MessageLimitPolicy m_messageLimitPolicy;

};

/**
 * @}
 */
