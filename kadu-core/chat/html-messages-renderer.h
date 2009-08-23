/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HTML_MESSAGES_RENDERER
#define HTML_MESSAGES_RENDERER

#include <QtCore/QObject>

class QWebPage;

class Chat;
class ChatMessage;

class HtmlMessagesRenderer : public QObject
{
	Q_OBJECT

	Chat *MyChat;
	QList<ChatMessage *> MyChatMessages;
	ChatMessage *LastMessage;
	QWebPage *MyWebPage;

	bool PruneEnabled;

	void pruneMessages();

public:
	HtmlMessagesRenderer(Chat *chat, QObject *parent = 0);
	virtual ~HtmlMessagesRenderer();

	Chat * chat() { return MyChat; }
	void setChat(Chat *chat);

	QWebPage * webPage() { return MyWebPage; }

	bool pruneEnabled() { return PruneEnabled; }
	void setPruneEnabled(bool pruneEnabled);

	QString content();

	QList<ChatMessage *> messages() { return MyChatMessages; }
	void appendMessage(ChatMessage *message);
	void appendMessages(QList<ChatMessage *> messages);
	void clearMessages();

	ChatMessage * lastMessage() { return LastMessage; }
	void setLastMessage(ChatMessage *message);

	void refresh();
	void replaceLoadingImages(const QString &imageId, const QString &imagePath);
	void updateBackgroundsAndColors();

};

#endif // HTML_MESSAGES_RENDERER
