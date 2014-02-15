/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message.h"
#include "protocols/services/chat-state-service.h"

#include <QtCore/QVector>

class ChatImage;
class HtmlMessagesRenderer;

class ChatMessagesRenderer
{

public:
	virtual ~ChatMessagesRenderer() {}

	virtual void clearMessages(HtmlMessagesRenderer *) = 0;
	virtual void appendMessages(HtmlMessagesRenderer *, const QVector<Message> &) = 0;
	virtual void appendMessage(HtmlMessagesRenderer *, const Message &) = 0;
	virtual void pruneMessage(HtmlMessagesRenderer *) = 0;
	virtual void refreshView(HtmlMessagesRenderer *, bool useTransparency = false) = 0;
	virtual void messageStatusChanged(HtmlMessagesRenderer *, Message, MessageStatus) = 0;
	virtual void contactActivityChanged(HtmlMessagesRenderer *, ChatStateService::State, const QString &, const QString &) = 0;
	virtual void chatImageAvailable(HtmlMessagesRenderer *, const ChatImage &chatImage, const QString &fileName) = 0;

};
