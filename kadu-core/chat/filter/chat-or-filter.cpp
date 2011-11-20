/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"

#include "chat-or-filter.h"

ChatOrFilter::ChatOrFilter(QObject *parent) :
		ChatFilter(parent)
{
}

bool ChatOrFilter::acceptChat(const Chat &chat)
{
	foreach (ChatFilter * const filter, Filters)
		if (filter->acceptChat(chat))
			return true;

	return false;
}

void ChatOrFilter::addFilter(ChatFilter *const filter)
{
	if (Filters.contains(filter))
		return;

	Filters.append(filter);
	connect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
	emit filterChanged();
}

void ChatOrFilter::removeFilter(ChatFilter *const filter)
{
	if (Filters.removeAll(filter) == 0)
		return;

	disconnect(filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
	emit filterChanged();
}
