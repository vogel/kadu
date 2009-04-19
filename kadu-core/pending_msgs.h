/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#include <QtCore/QObject>

#include <time.h>

#include "contacts/contact.h"
#include "contacts/contact-set.h"

#include "exports.h"

class QString;

/**
	Represents message queue awaiting to be read. Messages in this class can be also stored between alternate application launches.
	\class PendingMsgs
	\brief Pending messages queue.
**/
class KADUAPI PendingMsgs : public QObject
{
	Q_OBJECT

public:
	struct Element;

private:
	typedef QList<Element> PendingMsgsList;
	PendingMsgsList msgs;

public:
	/**
		Structure representing a message awaiting in a queue.
		\struct Element
		\brief Message awaiting in a queue.
	**/
	struct Element
	{
		ContactSet contacts; /*!< Message senders list. */
		QString proto; /*!< Protocol name. */
		QString msg; /*!< Message content. */
		time_t time; /*!< Send time. */
		Element();
	};

	/**
		\fn PendingMsgs(QObject *parent=0, const char *name=0)
		Standard constructor.
		\param parent control parent. Default 0.
		\param name control name. Default 0.
	**/
	PendingMsgs(QObject *parent = 0);

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
		\fn unsigned int pendingMsgsCount(ContactList contacts) const
		Checks, if there are any pending messages for given contact list.
		\param contacts given contact list.
		\return pending messages count.
	**/
	unsigned int pendingMsgsCount(ContactSet contacts) const;

	/**
		\fn int count() const
		Number of all pending messages in queues.
		\return pending messages count.
	**/
	int count() const;

	/**
		\fn Element &operator[](int index)
		\return message from queue with given index.
	**/
	Element &operator[](int index);

public slots:
	/**
		\fn void openMessages()
		Calls method ChatManagerOld::openPendingMsgs.
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
	void addMsg(Account *account, Contact sender, ContactSet receipients, QString msg, time_t time);

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

extern KADUAPI PendingMsgs pending;

#endif
