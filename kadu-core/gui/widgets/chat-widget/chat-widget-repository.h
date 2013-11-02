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

#include "chat/chat.h"
#include "exports.h"

class ChatWidget;
class ChatWidgetFactory;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetRepository
 * @short Repository of ChatWidget instances.
 *
 * This repository holds instances of ChatWidget class.
 *
 * New instances are created by provided @see ChatWidgetFactory and can be created by
 * calling widgetForChat(Chat) for a Chat that does not hava an associated ChatWidget yet.
 * If no ChatWidgetFactory is set then no new ChatWidget instances will be created.
 * After creation chatWidgetCreated(ChatWidget*) signal is emitted.
 *
 * ChatWidget instances are responsible for their own destruction. When destruction is detected
 * chatWidgetDestroyed(ChatWidget*) signal is emitted.
 *
 * Repository can be tested for its content by hasWidgetForChat(Chat) and widgets() methods.
 */
class KADUAPI ChatWidgetRepository : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetRepository(QObject *parent = 0);
	virtual ~ChatWidgetRepository();

	/**
	 * @short Set factory for creating new ChatWidget instances.
	 * @param chatWidgetFactory new factory
	 */
	void setChatWidgetFactory(ChatWidgetFactory *chatWidgetFactory);

	/**
	 * @short Check if repository contains ChatWidget for given chat.
	 * @param chat chat to check
	 */
	bool hasWidgetForChat(const Chat &chat) const;

	/**
	 * @short Return ChatWidget for given chat.
	 * @param chat chat to get ChatWidget for
	 * @return ChatWidget for given chat
	 *
	 * If chat is null then nullptr is returned. If repository does contain ChatWidget then
	 * it is returned. Else, if ChatWidgetFactory is set then new widget is created,
	 * chatWidgetCreated(ChatWidget*) signal is emitted and this new widget is returned.
	 * Otherwise nullptr is returned.
	 */
	ChatWidget * widgetForChat(const Chat &chat);

	/**
	 * @short Return complete mapping of Chat to ChatWidget* instances
	 */
	const QMap<Chat, ChatWidget *> & widgets() const;

signals:
	/**
	 * @short Signal emitted when new ChatWidget was created for this repository.
	 * @param chatWidget newly created ChatWidget instance
	 */
	void chatWidgetCreated(ChatWidget *chatWidget);

	/**
	 * @short Signal emitted when new ChatWidget was destroyed in this repository.
	 * @param chatWidget just destroyed ChatWidget instance
	 */
	void chatWidgetDestroyed(ChatWidget *chatWidget);

private:
	QWeakPointer<ChatWidgetFactory> m_chatWidgetFactory;
	QMap<Chat, ChatWidget *> m_widgets;

private slots:
	void widgetDestroyed(const Chat &chat);

};

/**
 * @}
 */
