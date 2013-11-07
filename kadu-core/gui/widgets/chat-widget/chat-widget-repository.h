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
 * Instances are added and removed by addChatWidget(ChatWidget*) and
 * removeChatWidget(ChatWidget*) methods.
 * Access for them is provided by widgetForChat(Chat) and widgets() methods.
 *
 * ChatWidget is also automatically removed when it is destroyed.
 *
 * Signals chatWidgetAdded(ChatWidget*) and chatWidgetRemoved(ChatWidget*)
 * are used to notify about changing content of this repository.
 */
class KADUAPI ChatWidgetRepository : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetRepository(QObject *parent = 0);
	virtual ~ChatWidgetRepository();

	/**
	 * @short Add new chatWidget to repository.
	 *
	 * Add new chatWidget to repository only if it is valid and not already in repository.
	 * Signal chatWidgetAdded(ChatWidget*) is emitted after successfull add.
	 */
	void addChatWidget(ChatWidget *chatWidget);

	/**
	 * @short Remove chatWidget from repository.
	 *
	 * Remove chatWidget from repository only if it is  already in repository.
	 * Signal chatWidgetRemoved(ChatWidget*) is emitted after successfull removal.
	 */
	void removeChatWidget(ChatWidget *chatWidget);

	/**
	 * @short Return ChatWidget for given chat.
	 * @param chat chat to get ChatWidget for
	 * @return ChatWidget for given chat
	 *
	 * If chat is null then nullptr is returned. If repository does contain ChatWidget then
	 * it is returned. Else nullptr is returned.
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
	void chatWidgetAdded(ChatWidget *chatWidget);

	/**
	 * @short Signal emitted when new ChatWidget was destroyed in this repository.
	 * @param chatWidget just destroyed ChatWidget instance
	 */
	void chatWidgetRemoved(ChatWidget *chatWidget);

private:
	QMap<Chat, ChatWidget *> m_widgets;

private slots:
	void widgetDestroyed(ChatWidget *chatWidget);

};

/**
 * @}
 */
