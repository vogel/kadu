/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "exports.h"

class ChatWidget;
class ChatWindow;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWindowRepository
 * @short Repository of ChatWindow instances.
 *
 * This repository holds instances of ChatWindow class.
 *
 * Instances are added and removed by addChatWidget(ChatWindow*) and
 * removeChatWidget(ChatWindow*) methods.
 * Access for them is provided by windowForChatWidget(Chat) and windows() methods.
 *
 * ChatWindow is also automatically removed when it is destroyed.
 */
class KADUAPI ChatWindowRepository : public QObject
{
	Q_OBJECT

public:
	explicit ChatWindowRepository(QObject *parent = nullptr);
	virtual ~ChatWindowRepository();

	/**
	 * @short Add new chatWindow to repository.
	 *
	 * Add new chatWindow to repository only if it is valid and not already in repository.
	 */
	void addChatWindow(ChatWindow *chatWindow);

	/**
	 * @short Remove chatWindow from repository.
	 *
	 * Remove chatWindow from repository only if it is  already in repository.
	 */
	void removeChatWindow(ChatWindow *chatWindow);

	/**
	 * @short Return ChatWindow for given chatWidget.
	 * @param chatWidget chatWidget to get ChatWindow for
	 * @return ChatWindow for given chatWidget
	 *
	 * If chatWidget is null then nullptr is returned. If repository does contain ChatWidget then
	 * it is returned. Else nullptr is returned.
	 */
	ChatWindow * windowForChatWidget(ChatWidget * const chatWidget);

	/**
	 * @short Return complete mapping of ChatWidget* to ChatWindow* instances
	 */
	const QMap<ChatWidget *, ChatWindow *> & windows() const;

private:
	QMap<ChatWidget *, ChatWindow *> m_windows;

private slots:
	void windowDestroyed(ChatWindow *chatWindow);

};

/**
 * @}
 */
