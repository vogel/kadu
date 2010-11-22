/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QKeyEvent>
#include <QtGui/QScrollBar>
#include <QtWebKit/QWebFrame>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "chat/style-engines/chat-style-engine.h"
#include "chat/message/message-render-info.h"
#include "chat/message/message-shared.h"
#include "chat/chat.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "gui/widgets/chat-view-network-access-manager.h"
#include "protocols/services/chat-image-service.h"

#include "debug.h"

#include "chat-messages-view.h"

ChatMessagesView::ChatMessagesView(Chat chat, bool supportTransparency, QWidget *parent) :
		KaduWebView(parent), CurrentChat(chat),
		LastScrollValue(0), LastLine(false), SupportTransparency(supportTransparency)
{
	Renderer = new HtmlMessagesRenderer(CurrentChat, this);

	QNetworkAccessManager *oldManager = Renderer->webPage()->networkAccessManager();
	ChatViewNetworkAccessManager *newManager = new ChatViewNetworkAccessManager(oldManager, this);
	Renderer->webPage()->setNetworkAccessManager(newManager);

	// TODO: for me with empty styleSheet if has artifacts on scrollbars...
	// maybe Qt bug?
	setStyleSheet("QWidget { }");
	setFocusPolicy(Qt::NoFocus);
	setMinimumSize(QSize(100,100));
	setPage(Renderer->webPage());
	settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

	connectChat();
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(scrollToLine()));

	ChatStylesManager::instance()->chatViewCreated(this);
}

ChatMessagesView::~ChatMessagesView()
{
 	ChatStylesManager::instance()->chatViewDestroyed(this);

	disconnectChat();
}

void ChatMessagesView::connectChat()
{
	if (CurrentChat.isNull() || CurrentChat.chatAccount().isNull() || !CurrentChat.chatAccount().protocolHandler())
		return;

	ChatImageService *chatImageService = CurrentChat.chatAccount().protocolHandler()->chatImageService();
	if (chatImageService)
		connect(chatImageService, SIGNAL(imageReceived(const QString &, const QString &)),
				this, SLOT(imageReceived(const QString &, const QString &)));
}

void ChatMessagesView::disconnectChat()
{
	if (CurrentChat.isNull() || CurrentChat.chatAccount().isNull() || !CurrentChat.chatAccount().protocolHandler())
		return;

	ChatImageService *chatImageService = CurrentChat.chatAccount().protocolHandler()->chatImageService();
	if (chatImageService)
		disconnect(chatImageService, SIGNAL(imageReceived(const QString &, const QString &)),
				this, SLOT(imageReceived(const QString &, const QString &)));
}

void ChatMessagesView::setChat(Chat chat)
{
	disconnectChat();
	CurrentChat = chat;
	connectChat();

	Renderer->setChat(CurrentChat);
}

void ChatMessagesView::setForcePruneDisabled(bool disable)
{
	Renderer->setForcePruneDisabled(disable);
}

void ChatMessagesView::pageUp()
{
	QKeyEvent event(QEvent::KeyPress, 0x01000016, Qt::NoModifier);
	keyPressEvent(&event);
}

void ChatMessagesView::pageDown()
{
	QKeyEvent event(QEvent::KeyPress, 0x01000017, Qt::NoModifier);
	keyPressEvent(&event);
}

void ChatMessagesView::imageReceived(const QString &imageId, const QString &imageFileName)
{
	rememberScrollBarPosition();
	Renderer->replaceLoadingImages(imageId, imageFileName);
}

void ChatMessagesView::updateBackgroundsAndColors()
{
	Renderer->updateBackgroundsAndColors();
}

void ChatMessagesView::repaintMessages()
{
	rememberScrollBarPosition();
	Renderer->refresh();
}

void ChatMessagesView::appendMessage(Message message)
{
	MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
	appendMessage(messageRenderInfo);
}

void ChatMessagesView::appendMessage(MessageRenderInfo *message)
{
	kdebugf();

	connect(message->message(), SIGNAL(statusChanged(Message::Status)),
				this, SLOT(messageStatusChanged(Message::Status)));

	rememberScrollBarPosition();

	Renderer->appendMessage(message);

	emit messagesUpdated();
}

void ChatMessagesView::appendMessages(QList<Message> messages)
{
	kdebugf2();

	foreach (Message message, messages)
		appendMessage(message);
}

void ChatMessagesView::appendMessages(QList<MessageRenderInfo *> messages)
{
	kdebugf2();

	foreach (MessageRenderInfo *message, messages)
		connect(message->message(), SIGNAL(statusChanged(Message::Status)),
				this, SLOT(messageStatusChanged(Message::Status)));
	rememberScrollBarPosition();

	Renderer->appendMessages(messages);
	emit messagesUpdated();
}

void ChatMessagesView::clearMessages()
{
	Renderer->clearMessages();
	emit messagesUpdated();
}

unsigned int ChatMessagesView::countMessages()
{
	return Renderer->messages().count();
}

void ChatMessagesView::messageStatusChanged(Message::Status status)
{
	if (!sender())
		return;
	rememberScrollBarPosition();
	Renderer->messageStatusChanged(Message(sender()), status);
}

void ChatMessagesView::resizeEvent(QResizeEvent *e)
{
 	LastScrollValue = page()->currentFrame()->scrollBarValue(Qt::Vertical);
 	LastLine = (LastScrollValue == page()->currentFrame()->scrollBarMaximum(Qt::Vertical));

 	KaduWebView::resizeEvent(e);

	scrollToLine();
}

void ChatMessagesView::rememberScrollBarPosition()
{
	LastScrollValue = page()->currentFrame()->scrollBarValue(Qt::Vertical);
	LastLine = (LastScrollValue == page()->currentFrame()->scrollBarMaximum(Qt::Vertical));
}

void ChatMessagesView::scrollToLine()
{
 	if (LastLine)
 		page()->currentFrame()->setScrollBarValue(Qt::Vertical, page()->currentFrame()->scrollBarMaximum(Qt::Vertical));
 	else
 		page()->currentFrame()->setScrollBarValue(Qt::Vertical, LastScrollValue);
}
