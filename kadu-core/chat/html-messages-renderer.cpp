/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#include "chat/chat-styles-manager.h"
#include "chat/style-engines/chat-style-engine.h"

#include "html-messages-renderer.h"

HtmlMessagesRenderer::HtmlMessagesRenderer(Chat *chat, QObject *parent) :
		QObject(parent), MyChat(chat), LastMessage(0)
{
	MyWebPage = new QWebPage(this);
	MyWebPage->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

	ChatStylesManager::instance()->currentEngine()->refreshView(this);
}

HtmlMessagesRenderer::~HtmlMessagesRenderer()
{
}

void HtmlMessagesRenderer::setChat(Chat *chat)
{
	MyChat = chat;
}

QString HtmlMessagesRenderer::content()
{
	return MyWebPage->mainFrame()->toHtml();
}

void HtmlMessagesRenderer::appendMessage(ChatMessage *message)
{
	MyChatMessages.append(message);
}

void HtmlMessagesRenderer::appendMessages(QList<ChatMessage *> messages)
{
	MyChatMessages.append(messages);
}

void HtmlMessagesRenderer::setLastMessage(ChatMessage *message)
{
	LastMessage = message;
}
