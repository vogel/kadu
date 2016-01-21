/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <injeqt/injeqt.h>
#include <memory>

class ChatManager;
class Chat;

class QMimeData;

class ChatListMimeDataService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatListMimeDataService(QObject *parent = nullptr);
	virtual ~ChatListMimeDataService();

	QStringList mimeTypes();
	std::unique_ptr<QMimeData> toMimeData(const QList<Chat> &chatList);

	QList<Chat> fromMimeData(const QMimeData *mimeData);

private:
	QPointer<ChatManager> m_chatManager;

	QLatin1String m_mimeType;

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);

};
