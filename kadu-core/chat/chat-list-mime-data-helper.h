/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_LIST_MIME_DATA_HELPER_H
#define CHAT_LIST_MIME_DATA_HELPER_H

#include <QtCore/QList>
#include <QtCore/QStringList>

class QMimeData;

class Chat;

class ChatListMimeDataHelper
{
	static QLatin1String MimeType;

public:
	static QStringList mimeTypes();
	static QMimeData * toMimeData(const QList<Chat> &chatList);
	static QList<Chat> fromMimeData(const QMimeData *mimeData);

};

#endif // CHAT_LIST_MIME_DATA_HELPER_H
