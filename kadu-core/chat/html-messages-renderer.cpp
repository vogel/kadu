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

#include "chat/chat-message.h"
#include "chat/chat-styles-manager.h"
#include "chat/style-engines/chat-style-engine.h"
#include "configuration/configuration-file.h"

#include "html-messages-renderer.h"

HtmlMessagesRenderer::HtmlMessagesRenderer(Chat *chat, QObject *parent) :
		QObject(parent), MyChat(chat), LastMessage(0), PruneEnabled(true)
{
	MyWebPage = new QWebPage(this);
	MyWebPage->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

	ChatStylesManager::instance()->currentEngine()->refreshView(this);
}

HtmlMessagesRenderer::~HtmlMessagesRenderer()
{
	qDeleteAll(MyChatMessages);
	MyChatMessages.clear();
}

void HtmlMessagesRenderer::setChat(Chat *chat)
{
	MyChat = chat;
}

void HtmlMessagesRenderer::setPruneEnabled(bool pruneEnabled)
{
	PruneEnabled = pruneEnabled;
}

QString HtmlMessagesRenderer::content()
{
	return MyWebPage->mainFrame()->toHtml();
}

void HtmlMessagesRenderer::pruneMessages()
{
	if (!PruneEnabled || ChatStylesManager::instance()->prune() == 0)
		return;
	
	if (MyChatMessages.count() <= ChatStylesManager::instance()->prune())
		return;
	
	QList<ChatMessage *>::iterator start = MyChatMessages.begin();
	QList<ChatMessage *>::iterator stop = MyChatMessages.end() - ChatStylesManager::instance()->prune();
	for (QList<ChatMessage *>::iterator it = start; it != stop; ++it)
	{
		delete *it;
		ChatStylesManager::instance()->currentEngine()->pruneMessage(this);
	}
	
	MyChatMessages.erase(start, stop);
}

void HtmlMessagesRenderer::appendMessage(ChatMessage *message)
{
	MyChatMessages.append(message);
	pruneMessages();

	ChatStylesManager::instance()->currentEngine()->appendMessage(this, message);
}

void HtmlMessagesRenderer::appendMessages(QList<ChatMessage *> messages)
{
	MyChatMessages.append(messages);
	pruneMessages();

	ChatStylesManager::instance()->currentEngine()->appendMessages(this, messages);
}

void HtmlMessagesRenderer::clearMessages()
{
	qDeleteAll(MyChatMessages);
	MyChatMessages.clear();

	LastMessage = 0;
	ChatStylesManager::instance()->currentEngine()->clearMessages(this);
}

void HtmlMessagesRenderer::setLastMessage(ChatMessage *message)
{
	LastMessage = message;
}

void HtmlMessagesRenderer::refresh()
{
	ChatStylesManager::instance()->currentEngine()->refreshView(this);
}

void HtmlMessagesRenderer::replaceLoadingImages(const QString &imageId, const QString &imagePath)
{
	foreach (ChatMessage *message, MyChatMessages)
		message->replaceLoadingImages(imageId, imagePath);

	refresh();
}

void HtmlMessagesRenderer::updateBackgroundsAndColors()
{
	QString myBackgroundColor = config_file.readEntry("Look", "ChatMyBgColor");
	QString myFontColor = config_file.readEntry("Look", "ChatMyFontColor");
	QString myNickColor = config_file.readEntry("Look", "ChatMyNickColor");
	QString usrBackgroundColor = config_file.readEntry("Look", "ChatUsrBgColor");
	QString usrFontColor = config_file.readEntry("Look", "ChatUsrFontColor");
	QString usrNickColor = config_file.readEntry("Look", "ChatUsrNickColor");
	
	foreach (ChatMessage *message, MyChatMessages)
	{
		switch (message->message().type())
		{
			case Message::TypeSent:
				message->setColorsAndBackground(myBackgroundColor, myNickColor, myFontColor);
				break;

			case Message::TypeReceived:
				message->setColorsAndBackground(usrBackgroundColor, usrNickColor, usrFontColor);
				break;
		}
		
	}
}
