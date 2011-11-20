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

#ifndef CHAT_UNREAD_FILTER_H
#define CHAT_UNREAD_FILTER_H

#include "chat/filter/chat-filter.h"
#include "exports.h"

class KADUAPI ChatUnreadFilter : public ChatFilter
{
	Q_OBJECT

	bool Enabled;

public:
	ChatUnreadFilter(QObject *parent = 0);

	virtual bool acceptChat(const Chat &chat);

	void setEnabled(bool enabled);

};

#endif // CHAT_UNREAD_FILTER_H
