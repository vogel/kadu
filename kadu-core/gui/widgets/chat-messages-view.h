/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHAT_MESSAGES_VIEW
#define CHAT_MESSAGES_VIEW

#include <QtCore/QList>

#include "chat/message/message.h"
#include "protocols/protocol.h"

#include "kadu-web-view.h"

#include "exports.h"

class QResizeEvent;

class Chat;
class ChatWidget;
class HtmlMessagesRenderer;
class MessageRenderInfo;

class KADUAPI ChatMessagesView : public KaduWebView
{
	Q_OBJECT

	Chat CurrentChat;
	HtmlMessagesRenderer *Renderer;

	int LastScrollValue;
	bool LastLine;

	bool SupportTransparency;

	void connectChat();
	void disconnectChat();

protected:
	virtual void resizeEvent(QResizeEvent *e);

private slots:
	void repaintMessages();

	void pageUp();
	void pageDown();
	void scrollToLine();

	void imageReceived(const QString &imageId, const QString &imageFileName);

	void messageStatusChanged(Message::Status);

public:
	ChatMessagesView(Chat chat = Chat::null, bool supportTransparency = true, QWidget *parent = 0);
	virtual ~ChatMessagesView();

	HtmlMessagesRenderer * renderer() { return Renderer; }

	void appendMessage(const Message &message);
	void appendMessage(MessageRenderInfo *message);

	void appendMessages(const QList<Message> &messages);
	void appendMessages(const QList<MessageRenderInfo *> &messages);

 	unsigned int countMessages();

	void updateBackgroundsAndColors();

	void setForcePruneDisabled(bool disable);

	void rememberScrollBarPosition();

	Chat chat() const { return CurrentChat; }
	void setChat(Chat chat);

	bool supportTransparency() { return SupportTransparency; }

public slots:
	void clearMessages();

signals:
	void messagesUpdated();

};

#endif // CHAT_MESSAGES_VIEW
