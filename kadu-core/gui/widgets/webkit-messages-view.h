/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include <QtCore/QList>

#include "configuration/configuration-aware-object.h"
#include "message/message.h"
#include "misc/memory.h"
#include "os/generic/compositing-aware-object.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-image.h"
#include "protocols/services/chat-service.h"
#include "protocols/services/chat-state-service.h"

#include "kadu-web-view.h"

#include "exports.h"

class QResizeEvent;

class Chat;
class ChatImageRequestService;
class ChatMessagesRendererFactory;
class ChatWidget;
class HtmlMessagesRenderer;

class KADUAPI WebkitMessagesView : public KaduWebView, public ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

	QPointer<ChatImageRequestService> CurrentChatImageRequestService;

	Chat CurrentChat;
	qobject_ptr<HtmlMessagesRenderer> Renderer;
	std::shared_ptr<ChatMessagesRendererFactory> m_chatMessagesRendererFactory;

	bool SupportTransparency;

	void connectChat();
	void disconnectChat();

	bool AtBottom;

	Message firstNonSystemMessage(const QVector<Message> &messages);
	void recreateRenderer();

private slots:
	void chatImageStored(const ChatImage &chatImage, const QString &fullFilePath);
	void sentMessageStatusChanged(const Message &message);

protected:
	virtual void configurationUpdated();
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void wheelEvent(QWheelEvent *e);

	virtual void compositingEnabled();
	virtual void compositingDisabled();

public:
	explicit WebkitMessagesView(const Chat &chat = Chat::null, bool supportTransparency = true, QWidget *parent = 0);
	virtual ~WebkitMessagesView();

	void setChatImageRequestService(ChatImageRequestService *chatImageRequestService);

	HtmlMessagesRenderer * renderer() { return Renderer.get(); }

	void prependMessages(const QVector<Message> &messages);

	void appendMessage(const Message &message);
	void appendMessages(const QVector<Message> &messages);

	int countMessages();

	void setForcePruneDisabled(bool disable);

	Chat chat() const { return CurrentChat; }
	void setChat(const Chat &chat);

	bool supportTransparency() { return SupportTransparency; }

public slots:
	void setChatMessagesRendererFactory(std::shared_ptr<ChatMessagesRendererFactory> chatMessagesRendererFactory);

	void clearMessages();
	void contactActivityChanged(const Contact &contact, ChatStateService::State state);
	void updateAtBottom();
	void refreshView();

	void pageUp();
	void pageDown();
	void scrollToTop();
	void scrollToBottom();
	void forceScrollToBottom();

signals:
	void messagesUpdated();

};
