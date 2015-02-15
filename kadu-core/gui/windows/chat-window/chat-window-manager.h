/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatWidgetManager;
class ChatWindowStorage;
class ChatWindowRepository;

class ChatWindowManager : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatWindowManager(QObject *parent = 0);
	virtual ~ChatWindowManager();

	void openStoredChatWindows();
	void storeOpenedChatWindows();

private:
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ChatWindowRepository> m_chatWindowRepository;
	QPointer<ChatWindowStorage> m_chatWindowStorage;

private slots:
	INJEQT_SETTER void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SETTER void setChatWindowRepository(ChatWindowRepository *chatWindowRepository);
	INJEQT_SETTER void setChatWindowStorage(ChatWindowStorage *chatWindowStorage);

};
