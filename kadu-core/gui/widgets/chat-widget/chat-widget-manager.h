/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

class Chat;
class ChatWidget;
class ChatWidgetRepository;

enum class OpenChatActivation
{
	DoNotActivate,
	Activate
};

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetManager
 * @short Manager of all ChatWidget instances.
 * @todo replace handleNewChatWidget with better mechanism
 * @todo create some kind of per-container manager than can store its own configuration
 *
 * This singleton is responsible for all ChatWidget instances in Kadu.
 */
class KADUAPI ChatWidgetManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWidgetManager)

public:
	/**
	 * @short Returns manager's singleton instance.
	 * @return manager's singleton instance
	 *
	 * Returns manager's singleton instance.
	 */
	static ChatWidgetManager * instance();

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	/**
	 * @short Returns ChatWidget for given chat.
	 * @return ChatWidget for given chat
	 * @param chat chat for returned ChatWidget
	 *
	 * Returns ChatWidget for given chat.
	 */
	ChatWidget * byChat(const Chat &chat);

public slots:
	void openChat(const Chat &chat, OpenChatActivation activation);

	/**
	 * @short Close ChatWidget for given chat.
	 * @param chat chat to close chat widget for
	 *
	 * This method closes ChatWidget for given chat.
	 */
	void closeChat(const Chat &chat);

private:
	static ChatWidgetManager *m_instance;

	QWeakPointer<ChatWidgetRepository> m_chatWidgetRepository;

	ChatWidgetManager();
	virtual ~ChatWidgetManager();

};

/**
 * @}
 */
