/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PENDING_MESSAGES_MANAGER_H
#define PENDING_MESSAGES_MANAGER_H

#include <QtCore/QObject>

#include <time.h>

#include "chat/message/message.h"
#include "contacts/contact.h"
#include "contacts/contact-remove-predicate-object.h"

#include "exports.h"

class Chat;
class QString;

/**
	Represents message queue awaiting to be read. Messages in this class can be also stored between alternate application launches.
	\class PendingMessagesManager
	\brief Pending messages queue.
**/
class KADUAPI PendingMessagesManager : public QObject, ContactRemovePredicateObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PendingMessagesManager)

	static PendingMessagesManager * Instance;

	/**
		\fn PendingMessagesManager(QObject *parent=0, const char *name=0)
		Standard constructor.
		\param parent control parent. Default 0.
		\param name control name. Default 0.
	**/
	PendingMessagesManager();

	typedef QList<Message> PendingMsgsList;
	PendingMsgsList msgs;

    	bool removeContactFromStorage(Contact contact);

public:

	static PendingMessagesManager * instance();

	/**
		\fn bool pendingMsgs(Contact contact) const
		Checks, if there are any pending messages for given contact.
		\param contact given contact.
		\return true, if there is any message, otherwise - false.
	**/
	bool pendingMsgs(Contact contact) const;

	/**
		\fn bool pendingMsgs() const
		Cheks, if there are any messages in a queue at all.
		\return true, if there is any message, otherwise - false.
	**/
	bool pendingMsgs() const;

	/**
		\fn unsigned int pendingMsgsCount(Chat *chat) const
		Checks, if there are any pending messages for given chat.
		\param contacts given contact list.
		\return pending messages count.
	**/
	unsigned int pendingMsgsCount(Chat *chat) const;

	/**
		\fn int count() const
		Number of all pending messages in queues.
		\return pending messages count.
	**/
	int count() const;

	/**
		\fn Message &operator[](int index)
		\return message from queue with given index.
	**/
	Message &operator[](int index);

public slots:
	/**
		\fn void openMessages()
		Calls method ChatWidgetManager::openPendingMsgs.
	**/
	void openMessages();

	/**
		\fn void deleteMsg(int index)
		Removes message with given index from queue.
		\param index index of a message to be removed
	**/
	void deleteMsg(int index);

	/**
		\fn void addMsg(Account *account, ContactList contacts, QString msg, time_t time)
		Appends new message to queue.
		\param account user account
		\param contacts senders list.
		\param msg message content. TODO: const QString &
		\param time message send time.
	**/
	void addMsg(const Message &msg);

	/**
		\fn void loadConfiguration(XmlConfigFile *configurationStorage)
		Loads message queue from configuration file. Method called on Kadu startup.
	**/
	void loadConfiguration(XmlConfigFile *configurationStorage);

	/**
		\fn void storeConfiguration(XmlConfigFile *configurationStorage)
		Stores message queue in configuration file. Method called on Kadu shutdown.
	**/
	void storeConfiguration(XmlConfigFile *configurationStorage);

signals:
	/**
		\fn void messageFromUserAdded(Contact contact)
		Signal emitted, when new message from given contact was added to queue.
	**/
	void messageFromUserAdded(Contact contact);

	/**
		\fn void messageFromUserDeleted(Contact contact)
		Signal emitted, when message from given contact was removed from queue.
	**/
	void messageFromUserDeleted(Contact contact);

};

#endif //PENDING_MESSAGES_MANAGER_H
