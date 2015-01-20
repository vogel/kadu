/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Chat;
class ChatWidget;
class ChatWidgetActivationService;
class ChatWidgetContainerHandlerMapper;
class ChatWidgetFactory;
class ChatWidgetRepository;

enum class OpenChatActivation
{
	Ignore,
	DoNotActivate,
	Activate,
	Minimize
};

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetManager
 * @short Class for opening and closing chat widgets.
 */
class KADUAPI ChatWidgetManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWidgetManager)

public:
	Q_INVOKABLE explicit ChatWidgetManager(QObject *parent = nullptr);
	virtual ~ChatWidgetManager();

public slots:
	/**
	 * @short Open given chat.
	 *
	 * If activation is set to OpenChatActivation::Activate then newly opened
	 * chat will be activated.
	 *
	 * If activation is set to OpenChatActivation::Minimize and window is not yet
	 * opened then it will be minimized after opening.
	 */
	ChatWidget * openChat(const Chat &chat, OpenChatActivation activation);

private:
	QPointer<ChatWidgetActivationService> m_chatWidgetActivationService;
	QPointer<ChatWidgetContainerHandlerMapper> m_chatWidgetContainerHandlerMapper;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<ChatWidgetFactory> m_chatWidgetFactory;

private slots:
	INJEQT_SETTER void setChatWidgetActivationService(ChatWidgetActivationService *chatWidgetActivationService);
	INJEQT_SETTER void setChatWidgetContainerHandlerMapper(ChatWidgetContainerHandlerMapper *chatWidgetContainerHandlerMapper);
	INJEQT_SETTER void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SETTER void setChatWidgetFactory(ChatWidgetFactory *chatWidgetFactory);

};

/**
 * @}
 */
