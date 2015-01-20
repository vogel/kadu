/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QObject>
#include <injeqt/injeqt.h>

class ChatWidgetContainerHandler;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetContainerHandlerRepository
 * @short Repository of ChatWidgetContainerHandler.
 *
 * This repository holds instances of ChatWidgetContainerHandler.
 * These instances are not owned by this repository.
 */
class KADUAPI ChatWidgetContainerHandlerRepository : public QObject
{
	Q_OBJECT

public:
	using iterator = QList<ChatWidgetContainerHandler *>::iterator;

	Q_INVOKABLE explicit ChatWidgetContainerHandlerRepository(QObject *parent = nullptr);
	virtual ~ChatWidgetContainerHandlerRepository();

	void registerChatWidgetContainerHandler(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void unregisterChatWidgetContainerHandler(ChatWidgetContainerHandler *chatWidgetContainerHandler);

	iterator begin();
	iterator end();

	QList<ChatWidgetContainerHandler *> chatWidgetContainerHandlers() const;

signals:
	void chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);

private:
	QList<ChatWidgetContainerHandler *> m_chatWidgetContainerHandlers;

};

inline ChatWidgetContainerHandlerRepository::iterator begin(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
	return chatWidgetContainerHandlerRepository->begin();
}

inline ChatWidgetContainerHandlerRepository::iterator end(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
	return chatWidgetContainerHandlerRepository->end();
}

/**
 * @}
 */
