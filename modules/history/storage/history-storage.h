#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include <QtCore/QDate>
#include <QtCore/QObject>

#include "chat/chat.h"
#include "chat/chat_message.h"
#include "contacts/contact.h"

class HistoryStorage : public QObject
{
	Q_OBJECT

private slots:
	virtual void messageReceived(Chat *chat, Contact contact, const QString &message) = 0;
	virtual void messageSent(Chat *chat, const QString &message) = 0;

public:
	virtual QList<Chat *> chatsList() = 0;
	virtual QList<QDate> datesForChat(Chat *chat) = 0;
	virtual QList<ChatMessage *> getMessages(Chat *chat, QDate date = QDate(), int limit = 0) = 0;
	virtual int getMessagesCount(Chat *chat, QDate date = QDate()) = 0;

};

#endif
