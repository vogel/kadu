/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QKeyEvent>
#include <QtGui/QScrollBar>
#include <QtWebKit/QWebFrame>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "chat/chat_message.h"
#include "chat/chat_styles_manager.h"
#include "chat/style-engines/chat_style_engine.h"
#include "protocols/services/chat-image-service.h"

#include "chat-widget.h"
#include "debug.h"

#include "chat_messages_view.h"

ChatMessagesView::ChatMessagesView(QWidget *parent) : KaduTextBrowser(parent),
	LastScrollValue(0), LastLine(false), Chat(0), PrevMessage(0), PruneEnabled(true)
{
	setMinimumSize(QSize(100,100));

	if (parent)
		Chat = dynamic_cast<ChatWidget *>(parent);

 	ChatStylesManager::instance()->chatViewCreated(this);

	// TODO: for me with empty styleSheet if has artifacts on scrollbars...
	// maybe Qt bug?
  	setStyleSheet("QWidget { }");

	Protocol *protocol = AccountManager::instance()->defaultAccount()->protocol();
	ChatImageService *chatImageService = protocol->chatImageService();
	if (chatImageService)
			connect(chatImageService, SIGNAL(imageReceived(const QString &, const QString &)),
				this, SLOT(imageReceived(const QString &, const QString &)));


	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(scrollToLine()));

	settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

	setFocusPolicy(Qt::NoFocus);
}

ChatMessagesView::~ChatMessagesView()
{
	qDeleteAll(Messages);
	Messages.clear();
 	ChatStylesManager::instance()->chatViewDestroyed(this);

	Account *account = AccountManager::instance()->defaultAccount();
	if (!account)
		return;

	Protocol *protocol = account->protocol();
	if (!protocol)
		return;

	ChatImageService *chatImageService = protocol->chatImageService();
	if (chatImageService)
			disconnect(chatImageService, SIGNAL(imageReceived(const QString &, const QString &)),
				this, SLOT(imageReceived(const QString &, const QString &)));
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

void ChatMessagesView::imageReceived(const QString &messageId, const QString &messagePath)
{
	foreach (ChatMessage *message, Messages)
		message->replaceLoadingImages(messageId, messagePath);

 	ChatStylesManager::instance()->currentEngine()->refreshView(this);
}

void ChatMessagesView::updateBackgroundsAndColors()
{
	QString myBackgroundColor = config_file.readEntry("Look", "ChatMyBgColor");
	QString myFontColor = config_file.readEntry("Look", "ChatMyFontColor");
	QString myNickColor = config_file.readEntry("Look", "ChatMyNickColor");
	QString usrBackgroundColor = config_file.readEntry("Look", "ChatUsrBgColor");
	QString usrFontColor = config_file.readEntry("Look", "ChatUsrFontColor");
	QString usrNickColor = config_file.readEntry("Look", "ChatUsrNickColor");

	foreach(ChatMessage *message, Messages)
	{
		switch (message->type())
		{
			case TypeSent:
				message->setColorsAndBackground(myBackgroundColor, myNickColor, myFontColor);
				break;

			case TypeReceived:
				message->setColorsAndBackground(usrBackgroundColor, usrNickColor, usrFontColor);
				break;

			case TypeSystem:
				break;
		}

	}
}

void ChatMessagesView::repaintMessages()
{
	ChatStylesManager::instance()->currentEngine()->refreshView(this);
}

void ChatMessagesView::appendMessage(ChatMessage *message)
{
	kdebugf();

	connect(message, SIGNAL(statusChanged(Message::Status)),
			 this, SLOT(repaintMessages()));
	Messages.append(message);

	pruneMessages();

	ChatStylesManager::instance()->currentEngine()->appendMessage(this, message);
}

void ChatMessagesView::appendMessages(QList<ChatMessage *> messages)
{
	kdebugf2();

	foreach (ChatMessage *message, messages)
		connect(message, SIGNAL(statusChanged(Message::Status)),
				this, SLOT(repaintMessages()));

	Messages += messages;

	pruneMessages();

	ChatStylesManager::instance()->currentEngine()->appendMessages(this, messages);
}

void ChatMessagesView::pruneMessages()
{
	kdebugf();

	if (!PruneEnabled || ChatStylesManager::instance()->prune() == 0)
		return;

	if (Messages.count() < ChatStylesManager::instance()->prune())
		return;

	QList<ChatMessage *>::iterator start = Messages.begin();
	QList<ChatMessage *>::iterator stop = Messages.end() - ChatStylesManager::instance()->prune();
	for (QList<ChatMessage *>::iterator it = start; it != stop; ++it)
	{
		disconnect(*it, SIGNAL(statusChanged(Message::Status)),
				 this, SLOT(repaintMessages()));
		delete *it;
		ChatStylesManager::instance()->currentEngine()->pruneMessage(this);
	}

	Messages.erase(start, stop);
}

void ChatMessagesView::clearMessages()
{
	foreach (ChatMessage *message, Messages)
		disconnect(message, SIGNAL(statusChanged(Message::Status)),
				 this, SLOT(repaintMessages()));

	qDeleteAll(Messages);
	Messages.clear();
	PrevMessage = 0;
 	ChatStylesManager::instance()->currentEngine()->clearMessages(this);
}

unsigned int ChatMessagesView::countMessages()
{
	return Messages.count();
}

void ChatMessagesView::resizeEvent(QResizeEvent *e)
{
 	LastScrollValue = page()->currentFrame()->scrollBarValue(Qt::Vertical);
 	LastLine = (LastScrollValue == page()->currentFrame()->scrollBarMaximum(Qt::Vertical));

 	KaduTextBrowser::resizeEvent(e);

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

