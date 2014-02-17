/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#include "chat/chat-styles-manager.h"
#include "chat/style-engine/chat-messages-renderer.h"
#include "chat/style-engine/chat-messages-renderer-provider.h"
#include "core/core.h"
#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"

#include "html-messages-renderer.h"

HtmlMessagesRenderer::HtmlMessagesRenderer(Chat chat, QWebPage *parent) :
		QObject{parent}, m_chat{std::move(chat)}, m_pruneEnabled{true}, m_forcePruneDisabled{false}
{
	parent->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
}

HtmlMessagesRenderer::~HtmlMessagesRenderer()
{
}

void HtmlMessagesRenderer::setChat(Chat chat)
{
	m_chat = std::move(chat);
}

QWebPage * HtmlMessagesRenderer::webPage() const
{
	return static_cast<QWebPage *>(parent());
}

void HtmlMessagesRenderer::setForcePruneDisabled(bool forcePruneDisabled)
{
	m_forcePruneDisabled = forcePruneDisabled;
	pruneMessages();
}

QString HtmlMessagesRenderer::content()
{
	return webPage()->mainFrame()->toHtml();
}

bool HtmlMessagesRenderer::pruneEnabled()
{
	return !m_forcePruneDisabled && m_pruneEnabled;
}

void HtmlMessagesRenderer::pruneMessages()
{
	if (m_forcePruneDisabled)
		return;

	if (ChatStylesManager::instance()->prune() == 0)
	{
		m_pruneEnabled = false;
		return;
	}

	if (m_messages.count() <= ChatStylesManager::instance()->prune())
	{
		m_pruneEnabled = false;
		return;
	}

	m_pruneEnabled = true;

	auto start = m_messages.begin();
	auto stop = m_messages.end() - ChatStylesManager::instance()->prune();
	for (auto it = start; it != stop; ++it)
		Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->pruneMessage(this);

	m_messages.erase(start, stop);
}

void HtmlMessagesRenderer::appendMessage(const Message &message)
{
	m_messages.append(message);
	pruneMessages();

	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->appendMessage(this, message);
}

void HtmlMessagesRenderer::appendMessages(const QVector<Message> &messages)
{
	auto engineMessages = QVector<Message>{};
	for (auto message : messages)
	{
		engineMessages.append(message);
		m_messages.append(message);
	}

//  Do not prune messages here. When we are adding many massages to renderer, probably
//  we want all of them to be visible on message view. This also fixes crash from
//  bug #1963 . This crash occured, when we are trying to
//  cite more messages from history, than our message pruning setting
//	pruneMessages();

	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->appendMessages(this, engineMessages);
}

void HtmlMessagesRenderer::clearMessages()
{
	m_messages.clear();

	m_lastMessage = Message::null;
	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->clearMessages(this);
}

void HtmlMessagesRenderer::setLastMessage(Message message)
{
	m_lastMessage = message;
}

void HtmlMessagesRenderer::refresh()
{
	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->refreshView(this);
}

void HtmlMessagesRenderer::refreshView(bool useTransparency)
{
	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->refreshView(this, useTransparency);
}

void HtmlMessagesRenderer::chatImageAvailable(const ChatImage &chatImage, const QString &fileName)
{
	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->chatImageAvailable(this, chatImage, fileName);
}

void HtmlMessagesRenderer::messageStatusChanged(Message message, MessageStatus status)
{
	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->messageStatusChanged(this, message, status);
}

void HtmlMessagesRenderer::contactActivityChanged(const Contact &contact, ChatStateService::State state)
{
	auto display = contact.display(true);
	auto message = QString{};
	switch (state)
	{
		case ChatStateService::StateActive:
			message = tr("%1 is active").arg(display);
			break;
		case ChatStateService::StateComposing:
			message = tr("%1 is composing...").arg(display);
			break;
		case ChatStateService::StateGone:
			message = tr("%1 is gone").arg(display);
			break;
		case ChatStateService::StateInactive:
			message = tr("%1 is inactive").arg(display);
			break;
		case ChatStateService::StateNone:
			break;
		case ChatStateService::StatePaused:
			message = tr("%1 has paused composing").arg(display);
			break;
	}
	Core::instance()->chatMessagesRendererProvider()->chatMessagesRenderer()->contactActivityChanged(this, state, message, display);
}

#include "moc_html-messages-renderer.cpp"
