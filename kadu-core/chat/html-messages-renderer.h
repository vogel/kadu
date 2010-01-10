/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HTML_MESSAGES_RENDERER_H
#define HTML_MESSAGES_RENDERER_H

#include <QtCore/QObject>

#include "chat/message/message.h"

class QWebPage;

class Chat;
class MessageRenderInfo;

class HtmlMessagesRenderer : public QObject
{
	Q_OBJECT

	Chat MyChat;
	QList<MessageRenderInfo *> MyChatMessages;
	MessageRenderInfo *LastMessage;
	QWebPage *MyWebPage;

	bool PruneEnabled;
	bool ForcePruneDisabled;

	void pruneMessages();

public:
	HtmlMessagesRenderer(Chat chat, QObject *parent = 0);
	virtual ~HtmlMessagesRenderer();

	Chat  chat() { return MyChat; }
	void setChat(Chat chat);

	QWebPage * webPage() { return MyWebPage; }

	bool pruneEnabled() { return !ForcePruneDisabled && PruneEnabled; }
	void setPruneEnabled(bool pruneEnabled);

	bool forcePruneDisabled() { return ForcePruneDisabled; }
	void setForcePruneDisabled(bool forcePruneDisabled);

	QString content();

	QList<MessageRenderInfo *> messages() { return MyChatMessages; }
	void appendMessage(MessageRenderInfo *message);
	void appendMessages(QList<MessageRenderInfo *> messages);
	void clearMessages();

	MessageRenderInfo * lastMessage() { return LastMessage; }
	void setLastMessage(MessageRenderInfo *message);

	void refresh();
	void replaceLoadingImages(const QString &imageId, const QString &imagePath);
	void updateBackgroundsAndColors();
	void messageStatusChanged(Message message, Message::Status status);

};

#endif // HTML_MESSAGES_RENDERER_H
