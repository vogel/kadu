/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"
#include "exports.h"
#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatWidgetRepository;
class Chat;
class RecentChatManager;

class QWinJumpListCategory;
class QWinJumpList;

class WindowsJumpListService : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(SERVICE)

public:
	Q_INVOKABLE explicit WindowsJumpListService(QObject *parent = nullptr);
	virtual ~WindowsJumpListService();

private:
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<RecentChatManager> m_recentChatManager;

	owned_qptr<QWinJumpList> m_jumpList;

	void addChat(Chat chat);
	void addSeparator();

private slots:
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setRecentChatManager(RecentChatManager *recentChatManager);
	INJEQT_INIT void init();

	void updateJumpList();

};
