/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "webkit-messages-view.h"

#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/chat-style-renderer-configuration.h"
#include "chat-style/engine/chat-style-renderer-factory.h"
#include "chat-style/engine/chat-style-renderer.h"
#include "contacts/contact-set.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/scoped-updates-disabler.h"
#include "gui/widgets/chat-view-network-access-manager.h"
#include "gui/widgets/webkit-messages-view/message-limit-policy.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-handler-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-handler.h"
#include "misc/paths-provider.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-image-service.h"
#include "protocols/services/chat-service.h"
#include "services/chat-image-request-service.h"

#include <QtGui/QKeyEvent>
#include <QtWebKitWidgets/QWebFrame>

WebkitMessagesView::WebkitMessagesView(const Chat &chat, bool supportTransparency, QWidget *parent) :
		KaduWebView{parent},
		m_chat{chat},
		m_forcePruneDisabled{},
		m_supportTransparency{supportTransparency},
		m_atBottom{true}
{
	auto oldManager = page()->networkAccessManager();
	auto newManager = make_owned<ChatViewNetworkAccessManager>(oldManager, this);
	newManager->setImageStorageService(Core::instance()->imageStorageService());
	page()->setNetworkAccessManager(newManager.get());

	// TODO: for me with empty styleSheet if has artifacts on scrollbars...
	// maybe Qt bug?
	setStyleSheet("QWidget { }");
	setFocusPolicy(Qt::NoFocus);
	setMinimumSize(QSize(100,100));
	settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
	settings()->setAttribute(QWebSettings::PluginsEnabled, true);

	auto p = palette();

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

	connect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(scrollToBottom()));
	connect(ChatStyleManager::instance(), SIGNAL(chatStyleConfigurationUpdated()),
			this, SLOT(chatStyleConfigurationUpdated()));

	configurationUpdated();
	connectChat();
}

WebkitMessagesView::~WebkitMessagesView()
{
	disconnectChat();
}

void WebkitMessagesView::setChatImageRequestService(ChatImageRequestService *chatImageRequestService)
{
	if (m_chatImageRequestService)
		disconnect(m_chatImageRequestService.data(), nullptr, this, nullptr);

	m_chatImageRequestService = chatImageRequestService;

	if (m_chatImageRequestService)
		connect(m_chatImageRequestService.data(), SIGNAL(chatImageStored(ChatImage,QString)), this, SLOT(chatImageStored(ChatImage,QString)));
}

void WebkitMessagesView::setWebkitMessagesViewHandlerFactory(WebkitMessagesViewHandlerFactory *webkitMessagesViewHandlerFactory)
{
	m_webkitMessagesViewHandlerFactory = webkitMessagesViewHandlerFactory;
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
	m_atBottom = page()->mainFrame()->scrollBarValue(Qt::Vertical) >= page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
}

void WebkitMessagesView::connectChat()
{
	if (m_chat.isNull() || m_chat.chatAccount().isNull() || !m_chat.chatAccount().protocolHandler())
		return;

	for (auto const &contact : m_chat.contacts())
		connect(contact, SIGNAL(buddyUpdated()), this, SLOT(refreshView()));

	auto chatService = m_chat.chatAccount().protocolHandler()->chatService();
	if (chatService)
		connect(chatService, SIGNAL(sentMessageStatusChanged(const Message &)),
		        this, SLOT(sentMessageStatusChanged(const Message &)));
}

void WebkitMessagesView::disconnectChat()
{
	if (m_chat.isNull())
		return;

	for (auto const &contact : m_chat.contacts())
		disconnect(contact, nullptr, this, nullptr);

	if (m_chat.chatAccount().isNull() || !m_chat.chatAccount().protocolHandler())
		return;

	auto chatImageService = m_chat.chatAccount().protocolHandler()->chatImageService();
	if (chatImageService)
		disconnect(chatImageService, nullptr, this, nullptr);

	auto chatService = m_chat.chatAccount().protocolHandler()->chatService();
	if (chatService)
		disconnect(chatService, nullptr, this, nullptr);
}

void WebkitMessagesView::setChat(const Chat &chat)
{
	disconnectChat();
	m_chat = chat;
	connectChat();

	refreshView();
}

void WebkitMessagesView::setForcePruneDisabled(bool disable)
{
	m_forcePruneDisabled = disable;
	if (disable)
		m_handler->setMessageLimitPolicy(MessageLimitPolicy::None);
	else
	{
		m_handler->setMessageLimitPolicy(MessageLimitPolicy::Value);
		chatStyleConfigurationUpdated();
	}
}

void WebkitMessagesView::chatStyleConfigurationUpdated()
{
	m_handler->setMessageLimit(ChatStyleManager::instance()->prune());
}

void WebkitMessagesView::refreshView()
{
	if (!m_chatStyleRendererFactory || !m_webkitMessagesViewHandlerFactory)
		return;

	auto chatStyleRenderer = m_chatStyleRendererFactory->createChatStyleRenderer(rendererConfiguration());
	auto handler = m_webkitMessagesViewHandlerFactory.data()->createWebkitMessagesViewHandler(std::move(chatStyleRenderer), page()->mainFrame());
	setWebkitMessagesViewHandler(std::move(handler));
}

ChatStyleRendererConfiguration WebkitMessagesView::rendererConfiguration()
{
	QFile file{Application::instance()->pathsProvider()->dataPath() + QLatin1String("scripts/chat-scripts.js")};
	auto javaScript = file.open(QIODevice::ReadOnly | QIODevice::Text)
			? file.readAll()
			: QString{};
	auto transparency = ChatConfigurationHolder::instance()->useTransparency() && supportTransparency() && isCompositingEnabled();
	return ChatStyleRendererConfiguration{chat(), *page()->mainFrame(), javaScript, transparency};
}

void WebkitMessagesView::setWebkitMessagesViewHandler(owned_qptr<WebkitMessagesViewHandler> handler)
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	auto scrollBarPosition = page()->mainFrame()->scrollBarValue(Qt::Vertical);

	auto messages = m_handler
			? m_handler->messages()
			: SortedMessages{};
	m_handler = std::move(handler);
	setForcePruneDisabled(m_forcePruneDisabled);
	m_handler->add(messages);

	page()->mainFrame()->setScrollBarValue(Qt::Vertical, scrollBarPosition);
}

void WebkitMessagesView::pageUp()
{
	auto event = QKeyEvent{QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier};
	keyPressEvent(&event);
}

void WebkitMessagesView::pageDown()
{
	auto event = QKeyEvent{QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier};
	keyPressEvent(&event);
}

void WebkitMessagesView::chatImageStored(const ChatImage &chatImage, const QString &fullFilePath)
{
	m_handler->displayChatImage(chatImage, fullFilePath);
}

void WebkitMessagesView::add(const Message &message)
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	m_handler->add(message);
	emit messagesUpdated();
}

void WebkitMessagesView::add(const SortedMessages &messages)
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	m_handler->add(messages);
	emit messagesUpdated();
}

SortedMessages WebkitMessagesView::messages() const
{
	return m_handler->messages();
}

void WebkitMessagesView::setChatStyleRendererFactory(std::shared_ptr<ChatStyleRendererFactory> chatStyleRendererFactory)
{
	m_chatStyleRendererFactory = chatStyleRendererFactory;

	refreshView();
}

void WebkitMessagesView::clearMessages()
{
	ScopedUpdatesDisabler updatesDisabler{*this};
	m_handler->clear();
	emit messagesUpdated();
	m_atBottom = true;
}

int WebkitMessagesView::countMessages()
{
	return m_handler
			? m_handler->messages().size()
			: 0;
}

void WebkitMessagesView::sentMessageStatusChanged(const Message &message)
{
	if (m_chat != message.messageChat())
		return;
	m_handler->displayMessageStatus(message.id(), message.status());
}

void WebkitMessagesView::contactActivityChanged(const Contact &contact, ChatState state)
{
	m_handler->displayChatState(contact, state);
}

void WebkitMessagesView::scrollToTop()
{
	page()->mainFrame()->setScrollBarValue(Qt::Vertical, 0);
	updateAtBottom();
}

void WebkitMessagesView::scrollToBottom()
{
	if (m_atBottom)
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
	refreshView();
}

void WebkitMessagesView::compositingEnabled()
{
	refreshView();
}

void WebkitMessagesView::compositingDisabled()
{
	refreshView();
}

#include "moc_webkit-messages-view.cpp"
