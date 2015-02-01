/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Chat;
class ChatWidget;
class ChatWidgetContainerHandler;
class ChatWidgetContainerHandlerRepository;
class ChatWidgetRepository;
enum class OpenChatActivation;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetContainerHandlerMapper
 * @short Mapper between ChatWidget and ChatWidgetContainerHandler.
 *
 * This class maps all ChatWidget from given ChatWidgetRepository to
 * ChatWidgetContainerHandler from given ChatWidgetContainerHandlerRepository.
 *
 * It reacts on changes of content of two provided repositories and adds/removes
 * mapping as neccessary. It also reacts on chatAcceptanceChanged(Chat)
 * from ChatWidgetContainerHandlerRepository to update mapping.
 *
 * When mapping is applied, ChatWidget is added to ChatWidgetContainerHandler.
 * When mapping is removed, ChatWidget is removed from ChatWidgetContainerHandler.
 */
class ChatWidgetContainerHandlerMapper : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatWidgetContainerHandlerMapper(QObject *parent = 0);
	virtual ~ChatWidgetContainerHandlerMapper();

	/**
	 * @short Return ChatWidgetContainerHandler for given @p chat.
	 *
	 * If chat is currently not mappet, returns nullptr.
	 */
	ChatWidgetContainerHandler * chatWidgetContainerHandlerForChat(Chat chat) const;

	/**
	 * @short Maps @p chat to first ChatWidgetContainerHandler that accepts it.
	 * @param chat chat to accept
	 * @param activation activation mode for new chat
	 * @return ChatWidget instance created for @p chat and mapped to first ChatWidgetContainerHandler.
	 */
	ChatWidget * createHandledChatWidget(Chat chat, OpenChatActivation activation);

private:
	QPointer<ChatWidgetContainerHandlerRepository> m_chatWidgetContainerHandlerRepository;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;

	QMap<Chat, ChatWidgetContainerHandler *> m_mapping;

	void map(ChatWidgetContainerHandler *chatWidgetContainerHandler, Chat chat);
	void unmap(Chat chat);
	ChatWidgetContainerHandler * bestContainerHandler(Chat chat) const;

private slots:
	INJEQT_SETTER void setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository);
	INJEQT_SETTER void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	void chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatAcceptanceChanged(Chat chat);
	void chatWidgetRemoved(ChatWidget *chatWidget);

};

/**
 * @}
 */
