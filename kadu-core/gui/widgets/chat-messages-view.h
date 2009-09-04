/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef CHAT_MESSAGES_VIEW
#define CHAT_MESSAGES_VIEW

#include <QtCore/QList>

#include "protocols/protocol.h"

#include "kadu-text-browser.h"

#include "exports.h"

class Chat;
class ChatWidget;
class MessageRenderInfo;
class QResizeEvent;

class HtmlMessagesRenderer;

class KADUAPI ChatMessagesView : public KaduTextBrowser
{
	Q_OBJECT

	Chat *CurrentChat;
	HtmlMessagesRenderer *Renderer;

	int LastScrollValue;
	bool LastLine;

	void connectChat();
	void disconnectChat();

protected:
	virtual void resizeEvent(QResizeEvent *e);

private slots:
	void repaintMessages();
	
	void pageUp();
	void pageDown();
	void scrollToLine();

	void imageReceived(const QString &imageId, const QString &imagePath);

public:
	ChatMessagesView(Chat *chat = 0, QWidget *parent = 0);
	virtual ~ChatMessagesView();

	HtmlMessagesRenderer * renderer() { return Renderer; }

	void appendMessage(Message message);
	void appendMessage(MessageRenderInfo *message);

	void appendMessages(QList<Message> messages);
	void appendMessages(QList<MessageRenderInfo *> messages);

 	unsigned int countMessages();

	void updateBackgroundsAndColors();

	void setPruneEnabled(bool enable);
	void setForcePruneDisabled(bool disable);

	void rememberScrollBarPosition();

	Chat *chat() const { return CurrentChat; }
	void setChat(Chat *chat);

public slots:
	void clearMessages();

};

#endif // CHAT_MESSAGES_VIEW
