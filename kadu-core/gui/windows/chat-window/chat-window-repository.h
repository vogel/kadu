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

#include <QtCore/QObject>

#include "exports.h"

class ChatWidget;
class ChatWindow;
class ChatWindowFactory;

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
 * New instances are created by provided @see ChatWindowFactory and can be created by
 * calling windowForChatWidget(ChatWidget) for a ChatWidget that does not hava an
 * associated ChatWindow yet. If no ChatWindowFactory is set then no new ChatWindow
 * instances will be created.
 *
 * ChatWindow instances are responsible for their own destruction.
 *
 * Repository can be tested for its content by hasWindowForChatWidget(Chat) and
 * windows() methods.
 */
class KADUAPI ChatWindowRepository : public QObject
{
	Q_OBJECT

public:
	explicit ChatWindowRepository(QObject *parent = 0);
	virtual ~ChatWindowRepository();

	/**
	 * @short Set factory for creating new ChatWindow instances.
	 * @param chatWindowFactory new factory
	 */
	void setChatWindowFactory(ChatWindowFactory *chatWindowFactory);

	/**
	 * @short Check if repository contains ChatWindow for given chat widget.
	 * @param chatWidget chatWidget to check
	 */
	bool hasWindowForChatWidget(ChatWidget * const chatWidget) const;

	/**
	 * @short Return ChatWindow for given chat.
	 * @param chatWidget chat widget to get ChatWindow for
	 * @return ChatWindow for given chat widget
	 *
	 * If chatWidget is null then nullptr is returned. If repository does contain ChatWindow then
	 * it is returned. Else, if ChatWindowFactory is set then new widget is created and returned.
	 * Otherwise nullptr is returned.
	 */
	ChatWindow * windowForChatWidget(ChatWidget * const chatWidget);

	/**
	 * @short Return complete mapping of ChatWidget* to ChatWindow* instances
	 */
	const QMap<ChatWidget *, ChatWindow *> & windows() const;

private:
	QWeakPointer<ChatWindowFactory> m_chatWindowFactory;
	QMap<ChatWidget *, ChatWindow *> m_windows;

private slots:
	void windowDestroyed(ChatWidget * const chatWidget);

};

/**
 * @}
 */
