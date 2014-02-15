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

#include "chat/style-engine/chat-messages-renderer.h"

#include "chat/style-engine/kadu-style-engine/kadu-chat-syntax.h"

class KaduChatMessagesRenderer : public ChatMessagesRenderer
{

public:
	explicit KaduChatMessagesRenderer(KaduChatSyntax syntax);
	virtual ~KaduChatMessagesRenderer() {}

	virtual void clearMessages(HtmlMessagesRenderer *) override;
	virtual void appendMessages(HtmlMessagesRenderer *, const QVector<Message> &) override;
	virtual void appendMessage(HtmlMessagesRenderer *, const Message &) override;
	virtual void pruneMessage(HtmlMessagesRenderer *) override;
	virtual void refreshView(HtmlMessagesRenderer *, bool useTransparency = false) override;
	virtual void messageStatusChanged(HtmlMessagesRenderer *, Message, MessageStatus) override;
	virtual void contactActivityChanged(HtmlMessagesRenderer *, ChatStateService::State, const QString &, const QString &) override;
	virtual void chatImageAvailable(HtmlMessagesRenderer *, const ChatImage &chatImage, const QString &fileName) override;

private:
	KaduChatSyntax m_syntax;
	QString m_jsCode;

	QString formatMessage(const Message &message, const Message &after);
	void repaintMessages(HtmlMessagesRenderer *page);
	QString scriptsAtEnd(const QString &html);

};
