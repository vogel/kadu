#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include <QtCore/QObject>

#include "chat/chat.h"
#include "contacts/contact.h"

class HistoryStorage : public QObject
{
	Q_OBJECT

private slots:
	virtual void messageReceived(Chat *chat, Contact contact, const QString &message) = 0;
	virtual void messageSent(Chat *chat, const QString &message) = 0;

// public:



};

#endif
