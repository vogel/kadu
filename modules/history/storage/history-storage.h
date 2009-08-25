#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include <QtCore/QDate>
#include <QtCore/QObject>

#include "chat/chat.h"
#include "chat/chat-message.h"
#include "contacts/contact.h"

#include "../history_exports.h"

class Message;

class HISTORYAPI HistoryStorage : public QObject
{
	Q_OBJECT

private slots:
	virtual void messageReceived(const Message &message) = 0;
	virtual void messageSent(const Message &message) = 0;

public:
	virtual QList<Chat *> chatsList() = 0;
	virtual QList<QDate> datesForChat(Chat *chat) = 0;
	virtual QList<ChatMessage *> messages(Chat *chat, QDate date = QDate(), int limit = 0) = 0;
	virtual int messagesCount(Chat *chat, QDate date = QDate()) = 0;

	virtual void appendMessage(const Message &message) = 0;

	virtual void clearChatHistory(Chat *chat) = 0;

};

#endif
