/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_WIDGET_CONTAINER_H
#define CHAT_WIDGET_CONTAINER_H

class ChatWidget;

/**
 * @class ChatWidgetContainer
 * @brief Abstract class for parents of ChatWidget objects.
 */
class ChatWidgetContainer
{
public:
	virtual ~ChatWidgetContainer() {}

	/**
	 * @short Activates given chat widget in this ChatWidgetContainer.
	 * @param chatWidget chat widget to activate
	 *
	 * Activates given chat widget in this ChatWidgetContainer. This container window will be
	 * activated if neccessary.
	 */
	virtual void activateChatWidget(ChatWidget *chatWidget) = 0;

	/**
	 * @short Send alert about given chat widget in this ChatWidgetContainer.
	 * @param chatWidget chat widget to send alert about
	 *
	 * Send alert about given chat widget in this ChatWidgetContainer.
	 */
	virtual void alertChatWidget(ChatWidget *chatWidget) = 0;

	/**
	 * @short Closes given chat widget in this ChatWidgetContainer.
	 * @param chatWidget chat widget to close
	 *
	 * Closes given chat widget in this ChatWidgetContainer.
	 */
	virtual void closeChatWidget(ChatWidget *chatWidget) = 0;

	/**
	 * @short Check if given chat widget is active in this ChatWidgetContainer.
	 * @param chatWidget chat widget to check
	 * @return true if given chat widget is active
	 *
	 * Check if given chat widget is active in this ChatWidgetContainer.
	 */
	virtual bool isChatWidgetActive(ChatWidget *chatWidget) = 0;
};

#endif // CHAT_WIDGET_CONTAINER_H
