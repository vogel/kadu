/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "autoresponder-configuration.h"

#include "chat/chat.h"
#include "message/message-filter.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <injeqt/injeqt.h>

class ChatWidgetRepository;
class ChatWidget;
class MessageManager;

class AutoresponderMessageFilter : public QObject, public MessageFilter
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AutoresponderMessageFilter(QObject *parent = nullptr);
	virtual ~AutoresponderMessageFilter();

	void setConfiguration(const AutoresponderConfiguration &configuration);

	virtual bool acceptMessage(const Message &message);

public slots:
	void chatWidgetClosed(ChatWidget *chat);

private:
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<MessageManager> m_messageManager;

	AutoresponderConfiguration Configuration;

	QSet<Chat> RepliedChats;

private slots:
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_INIT void init();

};
