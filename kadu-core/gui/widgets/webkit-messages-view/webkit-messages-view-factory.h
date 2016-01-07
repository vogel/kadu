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

class Chat;
class ChatStyleRendererFactoryProvider;
class InjectedFactory;
class WebkitMessagesView;
class WebkitMessagesViewHandlerFactory;

class KADUAPI WebkitMessagesViewFactory : public QObject
{
	Q_OBJECT

public:
	explicit WebkitMessagesViewFactory(QObject *parent = nullptr);
	virtual ~WebkitMessagesViewFactory();

	void setChatStyleRendererFactoryProvider(ChatStyleRendererFactoryProvider *chatStyleRendererFactoryProvider);
	void setInjectedFactory(InjectedFactory *injectedFactory);
	void setWebkitMessagesViewHandlerFactory(WebkitMessagesViewHandlerFactory *webkitMessagesViewHandlerFactory);

	owned_qptr<WebkitMessagesView> createWebkitMessagesView(Chat chat, bool supportTransparency, QWidget *parent);

private:
	QPointer<ChatStyleRendererFactoryProvider> m_chatStyleRendererFactoryProvider;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<WebkitMessagesViewHandlerFactory> m_webkitMessagesViewHandlerFactory;

};
