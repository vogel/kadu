/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QMenu>
#include <injeqt/injeqt.h>

class ChatDataExtractor;
class ChatTypeManager;
class ChatWidgetRepository;
class IconsManager;
class RecentChatManager;

class KADUAPI RecentChatsMenu : public QMenu
{
	Q_OBJECT

public:
	explicit RecentChatsMenu(QWidget *parent = nullptr);
	virtual ~RecentChatsMenu();

signals:
	void chatsListAvailable(bool available);

private:
	QPointer<ChatDataExtractor> m_chatDataExtractor;
	QPointer<ChatTypeManager> m_chatTypeManager;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<IconsManager> m_iconsManager;
	QPointer<RecentChatManager> m_recentChatManager;

	bool m_recentChatsMenuNeedsUpdate;

private slots:
	INJEQT_SET void setChatDataExtractor(ChatDataExtractor *chatDataExtractor);
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setRecentChatManager(RecentChatManager *recentChatManager);
	INJEQT_INIT void init();

	void invalidate();
	void checkIfListAvailable();
	void update();
	void iconThemeChanged();

};
