/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JABBER_ROSTER_SERVICE_H
#define JABBER_ROSTER_SERVICE_H

#include "protocols/services/roster-service.h"

class Buddy;
class Contact;
class JabberProtocol;

namespace XMPP
{

class Client;
class JT_Roster;
class RosterItem;

class JabberRosterService : public RosterService
{
	Q_OBJECT

	QWeakPointer<Client> XmppClient;

	QMap<JT_Roster *, Contact> ContactForTask;

	static QStringList buddyGroups(const Buddy &buddy);
	static const QString & itemDisplay(const RosterItem &item);

	void ensureContactHasBuddyWithDisplay(const Contact &contact, const QString &display);
	JT_Roster * createContactTask(const Contact &contact);

	void connectToClient();
	void disconnectFromClient();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Mark all contacts of given account to deletion.
	 *
	 * Assume that all synchronized contacts was removed from roster. During roster download all still existing
	 * entries will be marked as synchronized (if not dirty). Even detached entries can be removed as detaching is
	 * only about groups and display name.
	 */
	void markContactsForDeletion();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Delete all contacts marked to deletion.
	 *
	 * All contacts that after roster updated are still marked for deletion are deleted from local roster.
	 */
	void deleteMarkedContacts();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Execute all stored RosterTask.
	 *
	 * This method executes all stored tasks. List of not-executed tasks will be empty after this call.
	 */
	void executeAllTasks();

private slots:
	void remoteContactUpdated(const RosterItem &item);
	void remoteContactDeleted(const RosterItem &item);

	void rosterTaskFinished();
	void rosterTaskDeleted(QObject *object);

	void rosterRequestFinished(bool success);

protected:
	virtual bool canPerformLocalUpdate() const;
	virtual void executeTask(const RosterTask &task);

protected slots:
	virtual void updateContact(const Contact &contact);

public:
	explicit JabberRosterService(JabberProtocol *protocol);
	virtual ~JabberRosterService();

	virtual void prepareRoster();

	void setClient(Client *xmppClient);

public slots:
	virtual void addContact(const Contact &contact);
	virtual void removeContact(const Contact &contact);

};

}

#endif // JABBER_ROSTER_SERVICE_H
