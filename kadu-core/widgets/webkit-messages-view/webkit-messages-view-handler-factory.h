/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatStyleManager;
class ChatStyleRenderer;
class WebkitMessagesViewDisplayFactory;
class WebkitMessagesViewHandler;

class KADUAPI WebkitMessagesViewHandlerFactory : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit WebkitMessagesViewHandlerFactory(QObject *parent = nullptr);
	virtual ~WebkitMessagesViewHandlerFactory();

	owned_qptr<WebkitMessagesViewHandler> createWebkitMessagesViewHandler(not_owned_qptr<ChatStyleRenderer> chatStyleRenderer, QObject *parent);

private:
	QPointer<ChatStyleManager> m_chatStyleManager;
	QPointer<WebkitMessagesViewDisplayFactory> m_webkitMessagesViewDisplayFactory;

private slots:
	INJEQT_SET void setChatStyleManager(ChatStyleManager *chatStyleManager);
	INJEQT_SET void setWebkitMessagesViewDisplayFactory(WebkitMessagesViewDisplayFactory *webkitMessagesViewDisplayFactory);

};
