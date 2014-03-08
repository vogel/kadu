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
#include <QtCore/QPointer>

class ChatWidget;
class ChatWidgetContainerHandler;
class ChatWidgetContainerHandlerRepository;
class ChatWidgetRepository;

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
 * mapping as neccessary. It also reacts on chatWidgetAcceptanceChanged(ChatWidget*)
 * from ChatWidgetContainerHandlerRepository to update mapping.
 *
 * When mapping is applied, ChatWidget is added to ChatWidgetContainerHandler.
 * When mapping is removed, ChatWidget is removed to ChatWidgetContainerHandler.
 */
class ChatWidgetContainerHandlerMapper : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetContainerHandlerMapper(QObject *parent = 0);
	virtual ~ChatWidgetContainerHandlerMapper();

	void setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository);
	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	ChatWidgetContainerHandler * chatWidgetContainerHandlerForWidget(ChatWidget *chatWidget) const;

private:
	QPointer<ChatWidgetContainerHandlerRepository> m_chatWidgetContainerHandlerRepository;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;

	QMap<ChatWidget *, ChatWidgetContainerHandler *> m_mapping;

	void mapToDefault(ChatWidget *chatWidget);
	void map(ChatWidgetContainerHandler *chatWidgetContainerHandler, ChatWidget *chatWidget);
	void unmap(ChatWidget *chatWidget);
	ChatWidgetContainerHandler * bestContainerHandler(ChatWidget *chatWidget) const;

private slots:
	void chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatWidgetAcceptanceChanged(ChatWidget *chatWidget);

	void chatWidgetAdded(ChatWidget *chatWidget);
	void chatWidgetRemoved(ChatWidget *chatWidget);

};

/**
 * @}
 */
