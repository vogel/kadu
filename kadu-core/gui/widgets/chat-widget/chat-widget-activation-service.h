/*
 * %kadu copyright begin%
 * Copyright 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "exports.h"

class ChatWidget;
class ChatWidgetContainerHandler;
class ChatWidgetContainerHandlerMapper;
class ChatWidgetContainerHandlerRepository;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetActivationService
 * @short Class for handling activation of chat widgets.
 *
 * Activation of chat widget is not deterministic so it can only be done by
 * tryActivateChatWidget(ChatWidget*) method. Chat activity status can be checked
 * by isChatWidgetActive(ChatWidget*).
 */
class KADUAPI ChatWidgetActivationService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatWidgetActivationService(QObject *parent = nullptr);
	virtual ~ChatWidgetActivationService();

	bool isChatWidgetActive(ChatWidget *chatWidget) const;
	void tryActivateChatWidget(ChatWidget *chatWidget);
	void tryMinimizeChatWidget(ChatWidget *chatWidget);

signals:
	void chatWidgetActivated(ChatWidget *chatWidget);

private:
	QPointer<ChatWidgetContainerHandlerMapper> m_chatWidgetContainerHandlerMapper;
	QPointer<ChatWidgetContainerHandlerRepository> m_chatWidgetContainerHandlerRepository;

private slots:
	INJEQT_SETTER void setChatWidgetContainerHandlerMapper(ChatWidgetContainerHandlerMapper *chatWidgetContainerHandlerMapper);
	INJEQT_SETTER void setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository);

	void chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);

};

/**
 * @}
 */
