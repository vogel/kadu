/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message-filter.h"

#include "message-filter-service.h"

MessageFilterService::MessageFilterService(QObject *parent) :
		QObject(parent)
{
}

MessageFilterService::~MessageFilterService()
{
}

void MessageFilterService::registerOutgoingMessageFilter(MessageFilter *filter)
{
	if (!filter || OutgoingMessageFilters.contains(filter))
		return;

	OutgoingMessageFilters.append(filter);
}

void MessageFilterService::unregisterOutgoingMessageFilter(MessageFilter *filter)
{
	OutgoingMessageFilters.removeAll(filter);
}

bool MessageFilterService::acceptOutgoingMessage(const Chat &chat, const QString &message)
{
	foreach (MessageFilter *filter, OutgoingMessageFilters)
		if (!filter->acceptMessage(chat, message))
			return false;
	return true;
}

void MessageFilterService::registerIncomingMessageFilter(MessageFilter *filter)
{
	if (!filter || IncomingMessageFilters.contains(filter))
		return;

	IncomingMessageFilters.append(filter);
}

void MessageFilterService::unregisterIncomingMessageFilter(MessageFilter *filter)
{
	IncomingMessageFilters.removeAll(filter);
}

bool MessageFilterService::acceptIncomingMessage(const Chat &chat, const QString &message)
{
	foreach (MessageFilter *filter, IncomingMessageFilters)
		if (!filter->acceptMessage(chat, message))
			return false;
	return true;
}
