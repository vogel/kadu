/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QKeyEvent>
#include <QtGui/QScrollBar>
#include <QtWebKit/QWebFrame>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "chat/chat-styles-manager.h"
#include "chat/chat.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engines/chat-style-engine.h"
#include "configuration/chat-configuration-holder.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-view-network-access-manager.h"
#include "message/message-render-info.h"
#include "protocols/services/chat-image-service.h"

#include "debug.h"

#include "chat-messages-view.h"

ChatMessagesView::ChatMessagesView(const Chat &chat, bool supportTransparency, QWidget *parent) :
		KaduWebView(parent), CurrentChat(chat), SupportTransparency(supportTransparency), AtBottom(true)
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
	settings()->setAttribute(QWebSettings::PluginsEnabled, true);

	QPalette p = palette();

	// This widget never has focus anyway, so there's no need for distinction
	// between active and inactive, and active highlight colors have way better
	// contrast, especially on Windows. See Kadu bug #2605.
	p.setBrush(QPalette::Inactive, QPalette::Highlight, p.brush(QPalette::Active, QPalette::Highlight));
	p.setBrush(QPalette::Inactive, QPalette::HighlightedText, p.brush(QPalette::Active, QPalette::HighlightedText));

	p.setBrush(QPalette::Base, Qt::transparent);
	setPalette(p);

	setAttribute(Qt::WA_OpaquePaintEvent, false);

	page()->currentFrame()->evaluateJavaScript(
		"XMLHttpRequest.prototype.open = function() { return false; };"
		"XMLHttpRequest.prototype.send = function() { return false; };"
	);

	configurationUpdated();

	connectChat();

	connect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(scrollToBottom()));

	ChatStylesManager::instance()->chatViewCreated(this);
}

ChatMessagesView::~ChatMessagesView()
{
 	ChatStylesManager::instance()->chatViewDestroyed(this);

	disconnectChat();
}

void ChatMessagesView::mouseReleaseEvent(QMouseEvent *e)
{
	updateAtBottom();
	KaduWebView::mouseReleaseEvent(e);
}

void ChatMessagesView::resizeEvent(QResizeEvent *e)
{
	QWebView::resizeEvent(e);

	scrollToBottom();
}

void ChatMessagesView::wheelEvent(QWheelEvent* e)
{
	updateAtBottom();
	QWebView::wheelEvent(e);
}

void ChatMessagesView::updateAtBottom()
{
	AtBottom = page()->mainFrame()->scrollBarValue(Qt::Vertical) >= page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
}

void ChatMessagesView::connectChat()
{
	if (CurrentChat.isNull() || CurrentChat.chatAccount().isNull() || !CurrentChat.chatAccount().protocolHandler())
		return;

	foreach (const Contact &contact, CurrentChat.contacts())
		connect(contact, SIGNAL(buddyUpdated()), this, SLOT(repaintMessages()));

	ChatImageService *chatImageService = CurrentChat.chatAccount().protocolHandler()->chatImageService();
	if (chatImageService)
		connect(chatImageService, SIGNAL(imageReceived(const QString &, const QString &)),
				this, SLOT(imageReceived(const QString &, const QString &)));

	ChatService *chatService = CurrentChat.chatAccount().protocolHandler()->chatService();
	if (chatService)
		connect(chatService, SIGNAL(sentMessageStatusChanged(const Message &)),
		        this, SLOT(sentMessageStatusChanged(const Message &)));
}

void ChatMessagesView::disconnectChat()
{
	if (CurrentChat.isNull())
		return;

	foreach (const Contact &contact, CurrentChat.contacts())
		disconnect(contact, 0, this, 0);

	if (CurrentChat.chatAccount().isNull() || !CurrentChat.chatAccount().protocolHandler())
		return;

	ChatImageService *chatImageService = CurrentChat.chatAccount().protocolHandler()->chatImageService();
	if (chatImageService)
		disconnect(chatImageService, 0, this, 0);

	ChatService *chatService = CurrentChat.chatAccount().protocolHandler()->chatService();
	if (chatService)
		disconnect(chatService, 0, this, 0);
}

void ChatMessagesView::setChat(const Chat &chat)
{
	disconnectChat();
	CurrentChat = chat;
	connectChat();

	Renderer->setChat(CurrentChat);
}

void ChatMessagesView::refresh()
{
	Renderer->refresh();
}

void ChatMessagesView::setForcePruneDisabled(bool disable)
{
	Renderer->setForcePruneDisabled(disable);
}

void ChatMessagesView::pageUp()
{
	QKeyEvent event(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
	keyPressEvent(&event);
}

void ChatMessagesView::pageDown()
{
	QKeyEvent event(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
	keyPressEvent(&event);
}

void ChatMessagesView::imageReceived(const QString &imageId, const QString &imageFileName)
{
	Renderer->replaceLoadingImages(imageId, imageFileName);
}

void ChatMessagesView::updateBackgroundsAndColors()
{
	Renderer->updateBackgroundsAndColors();
}

void ChatMessagesView::repaintMessages()
{
	setUpdatesEnabled(false);

	int scrollBarPosition = page()->mainFrame()->scrollBarValue(Qt::Vertical);
	Renderer->refresh();
	page()->mainFrame()->setScrollBarValue(Qt::Vertical, scrollBarPosition);

	setUpdatesEnabled(true);
}

bool ChatMessagesView::sameMessage(const Message &left, const Message &right)
{
	if (left.isNull() && right.isNull())
		return true;

	if (left.isNull() || right.isNull()) // one is null, second one is not
		return false;

	if (left.type() != right.type())
		return false;

	// In our SQL history we store datetime with accuracy to one second,
	// while for received XMPP messages we have a millisecond accuracy.
	// So to have proper results, we need to truncate those additional milliseconds.
	if (left.sendDate().toTime_t() != right.sendDate().toTime_t())
		return false;

	if (left.messageChat() != right.messageChat())
		return false;

	if (left.messageSender() != right.messageSender())
		return false;

	if (left.content() != right.content())
		return false;

	return true;
}

Message ChatMessagesView::firstNonSystemMessage(const QList<MessageRenderInfo *> &messages)
{
	foreach (MessageRenderInfo *message, messages)
		if (message->message().type() != MessageTypeSystem)
			return message->message();

	return Message::null;
}

void ChatMessagesView::prependMessages(const QVector<Message> &messages)
{
	if (messages.empty())
		return;

	// case #1: all prepended messages are already rendered
	const Message &firstMessage = messages.at(0);
	QList<MessageRenderInfo *> copyOfRendererMessages = Renderer->messages();
	foreach (const MessageRenderInfo *renderInfo, copyOfRendererMessages)
		if (sameMessage(renderInfo->message(), firstMessage))
			return;

	// case #2: some prepended messages are already rendered
	const Message &firstRenderedMessage = firstNonSystemMessage(copyOfRendererMessages);
	QList<MessageRenderInfo *> newMessages;
	foreach (const Message &message, messages)
	{
		if (sameMessage(firstRenderedMessage, message))
			break; // we already have this and following messages in our window

		newMessages.append(new MessageRenderInfo(message));
	}

	// we need to make new instances of MessageRenderInfo here
	// clearMessages will destroy existing ones
	foreach (const MessageRenderInfo *renderInfo, copyOfRendererMessages)
		newMessages.append(new MessageRenderInfo(renderInfo->message()));

	setUpdatesEnabled(false);

	Renderer->clearMessages();
	Renderer->appendMessages(newMessages);

	setUpdatesEnabled(true);

	emit messagesUpdated();
}

void ChatMessagesView::appendMessage(const Message &message)
{
	MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
	appendMessage(messageRenderInfo);
}

void ChatMessagesView::appendMessage(MessageRenderInfo *message)
{
	kdebugf();

	setUpdatesEnabled(false);

	Renderer->appendMessage(message);

	setUpdatesEnabled(true);

	emit messagesUpdated();
}

void ChatMessagesView::appendMessages(const QVector<Message> &messages)
{
	kdebugf2();

	QList<MessageRenderInfo *> rendererMessages;
	rendererMessages.reserve(messages.size());

	foreach (const Message &message, messages)
	{
		MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
		rendererMessages.append(messageRenderInfo);
	}

	appendMessages(rendererMessages);
}

void ChatMessagesView::appendMessages(const QList<MessageRenderInfo *> &messages)
{
	kdebugf2();

	setUpdatesEnabled(false);

	Renderer->appendMessages(messages);

	setUpdatesEnabled(true);

	emit messagesUpdated();
}

void ChatMessagesView::clearMessages()
{
	setUpdatesEnabled(false);

	Renderer->clearMessages();

	setUpdatesEnabled(true);

	emit messagesUpdated();
	AtBottom = true;
}

unsigned int ChatMessagesView::countMessages()
{
	return Renderer->messages().count();
}

void ChatMessagesView::sentMessageStatusChanged(const Message &message)
{
	if (CurrentChat != message.messageChat())
		return;
	Renderer->messageStatusChanged(message, message.status());
}

void ChatMessagesView::contactActivityChanged(const Contact &contact, ChatStateService::State state)
{
	Renderer->contactActivityChanged(contact, state);
}

void ChatMessagesView::scrollToTop()
{
	page()->mainFrame()->setScrollBarValue(Qt::Vertical, 0);
	updateAtBottom();
}

void ChatMessagesView::scrollToBottom()
{
	if (AtBottom)
		page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void ChatMessagesView::forceScrollToBottom()
{
	page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
	updateAtBottom();
}

void ChatMessagesView::configurationUpdated()
{
	setUserFont(ChatConfigurationHolder::instance()->chatFont().toString(), ChatConfigurationHolder::instance()->forceCustomChatFont());
}
