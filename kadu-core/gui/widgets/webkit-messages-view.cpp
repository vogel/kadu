/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "core/core.h"
#include "formatted-string/formatted-string.h"
#include "gui/scoped-updates-disabler.h"
#include "gui/widgets/chat-view-network-access-manager.h"
#include "protocols/services/chat-image-service.h"
#include "services/chat-image-request-service.h"
#include "services/image-storage-service.h"

#include "debug.h"

#include "webkit-messages-view.h"

WebkitMessagesView::WebkitMessagesView(const Chat &chat, bool supportTransparency, QWidget *parent) :
		KaduWebView(parent), CurrentChat(chat), SupportTransparency(supportTransparency), AtBottom(true)
{
	Renderer = new HtmlMessagesRenderer(CurrentChat, this);

	QNetworkAccessManager *oldManager = Renderer->webPage()->networkAccessManager();
	ChatViewNetworkAccessManager *newManager = new ChatViewNetworkAccessManager(oldManager, this);
	newManager->setImageStorageService(Core::instance()->imageStorageService());
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

WebkitMessagesView::~WebkitMessagesView()
{
 	ChatStylesManager::instance()->chatViewDestroyed(this);

	disconnectChat();
}

void WebkitMessagesView::setChatImageRequestService(ChatImageRequestService *chatImageRequestService)
{
	if (CurrentChatImageRequestService)
		disconnect(CurrentChatImageRequestService.data(), 0, this, 0);

	CurrentChatImageRequestService = chatImageRequestService;

	if (CurrentChatImageRequestService)
		connect(CurrentChatImageRequestService.data(), SIGNAL(chatImageStored(ChatImage,QString)), this, SLOT(chatImageStored(ChatImage,QString)));
}

void WebkitMessagesView::mouseReleaseEvent(QMouseEvent *e)
{
	updateAtBottom();
	KaduWebView::mouseReleaseEvent(e);
}

void WebkitMessagesView::resizeEvent(QResizeEvent *e)
{
	QWebView::resizeEvent(e);

	scrollToBottom();
}

void WebkitMessagesView::wheelEvent(QWheelEvent* e)
{
	updateAtBottom();
	QWebView::wheelEvent(e);
}

void WebkitMessagesView::updateAtBottom()
{
	AtBottom = page()->mainFrame()->scrollBarValue(Qt::Vertical) >= page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
}

void WebkitMessagesView::connectChat()
{
	if (CurrentChat.isNull() || CurrentChat.chatAccount().isNull() || !CurrentChat.chatAccount().protocolHandler())
		return;

	foreach (const Contact &contact, CurrentChat.contacts())
		connect(contact, SIGNAL(buddyUpdated()), this, SLOT(repaintMessages()));

	ChatService *chatService = CurrentChat.chatAccount().protocolHandler()->chatService();
	if (chatService)
		connect(chatService, SIGNAL(sentMessageStatusChanged(const Message &)),
		        this, SLOT(sentMessageStatusChanged(const Message &)));
}

void WebkitMessagesView::disconnectChat()
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

void WebkitMessagesView::setChat(const Chat &chat)
{
	disconnectChat();
	CurrentChat = chat;
	connectChat();

	Renderer->setChat(CurrentChat);
}

void WebkitMessagesView::refresh()
{
	Renderer->refresh();
}

void WebkitMessagesView::setForcePruneDisabled(bool disable)
{
	Renderer->setForcePruneDisabled(disable);
}

void WebkitMessagesView::pageUp()
{
	QKeyEvent event(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
	keyPressEvent(&event);
}

void WebkitMessagesView::pageDown()
{
	QKeyEvent event(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
	keyPressEvent(&event);
}

void WebkitMessagesView::chatImageStored(const ChatImage &chatImage, const QString &fullFilePath)
{
	Renderer->chatImageAvailable(chatImage, fullFilePath);
}

void WebkitMessagesView::repaintMessages()
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	int scrollBarPosition = page()->mainFrame()->scrollBarValue(Qt::Vertical);
	Renderer->refresh();
	page()->mainFrame()->setScrollBarValue(Qt::Vertical, scrollBarPosition);
}

bool WebkitMessagesView::sameMessage(const Message &left, const Message &right)
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

	if (*left.content() != *right.content())
		return false;

	return true;
}

Message WebkitMessagesView::firstNonSystemMessage(const QVector<Message> &messages)
{
	auto it = std::find_if(std::begin(messages), std::end(messages),
		[](const Message &message){ return message.type() != MessageTypeSystem; }
	);
	return it != std::end(messages)
			? *it
			: Message::null;
}

void WebkitMessagesView::prependMessages(const QVector<Message> &messages)
{
	if (messages.empty())
		return;

	auto const &rendererMessages = Renderer->messages();

	// case #1: all prepended messages are already rendered
	auto const &firstMessage = messages.at(0);
	auto hasFirstMessage = std::any_of(std::begin(rendererMessages), std::end(rendererMessages),
		[&firstMessage,this](const Message &message){ return sameMessage(message, firstMessage); }
	);
	if (hasFirstMessage)
		return;

	// case #2: some prepended messages are already rendered
	auto const &firstRenderedMessage = firstNonSystemMessage(rendererMessages);
	auto newMessages = QVector<Message>{};
	for (auto const &message : messages)
	{
		if (sameMessage(firstRenderedMessage, message))
			break; // we already have this and following messages in our window

		newMessages.append(message);
	}

	// clearMessages will destroy existing ones
	for (auto const &message : rendererMessages)
		newMessages.append(message);

	ScopedUpdatesDisabler updatesDisabler{*this};
	Renderer->clearMessages();
	Renderer->appendMessages(newMessages);
	emit messagesUpdated();
}

void WebkitMessagesView::appendMessage(const Message &message)
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	Renderer->appendMessage(message);
	emit messagesUpdated();
}

void WebkitMessagesView::appendMessages(const QVector<Message> &messages)
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	Renderer->appendMessages(messages);
	emit messagesUpdated();
}

void WebkitMessagesView::clearMessages()
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	Renderer->clearMessages();
	emit messagesUpdated();
	AtBottom = true;
}

int WebkitMessagesView::countMessages()
{
	return Renderer->messages().count();
}

void WebkitMessagesView::sentMessageStatusChanged(const Message &message)
{
	if (CurrentChat != message.messageChat())
		return;
	Renderer->messageStatusChanged(message, message.status());
}

void WebkitMessagesView::contactActivityChanged(const Contact &contact, ChatStateService::State state)
{
	Renderer->contactActivityChanged(contact, state);
}

void WebkitMessagesView::scrollToTop()
{
	page()->mainFrame()->setScrollBarValue(Qt::Vertical, 0);
	updateAtBottom();
}

void WebkitMessagesView::scrollToBottom()
{
	if (AtBottom)
		page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void WebkitMessagesView::forceScrollToBottom()
{
	page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
	updateAtBottom();
}

void WebkitMessagesView::configurationUpdated()
{
	setUserFont(ChatConfigurationHolder::instance()->chatFont().toString(), ChatConfigurationHolder::instance()->forceCustomChatFont());
}

#include "moc_webkit-messages-view.cpp"
