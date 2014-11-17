/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>

#include "misc/memory.h"
#include "roster/roster-service.h"

enum class JabberRosterState;
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

	QPointer<Client> XmppClient;

	QSet<Contact> m_markedForDelete;
	owned_qptr<RosterServiceTasks> m_tasks;
	QMap<JT_Roster *, Contact> ContactForTask;
	JabberRosterState State;

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
	 * Assume that all synchronized contacts were removed from roster. During roster download all still existing
	 * entries will be marked as synchronized (if not dirty). Even detached entries can be removed as detaching is
	 * only about groups and display name.
	 */
	void markContactsForDeletion();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Delete all contacts marked for deletion.
	 *
	 * All contacts that after synchronization with remote roster are still marked for deletion are deleted from local roster.
	 */
	void deleteMarkedContacts();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Check if user is intrested in seeing given roster item data.
	 * @param item roster item to check
	 * @return true, if user is intrested in seeing given roster item data
	 *
	 * See: http://xmpp.org/extensions/xep-0162.html#contacts
	 *
	 * - items with subscription='both' or subscription='to' ;
	 * - items with subscription='none' or subscription='from' and ask='subscribe'. It is ((subscription='none' or subscription='from') and ask='subscribe') ;
	 * - items with subscription='none' or subscription='from' which have a 'name' attribute or a 'group' child set. It is ((subscription='none' or subscription='from') and (name attribute or group child)).
	 */
	bool isIntrestedIn(const XMPP::RosterItem &item);

	/**
	 * @short Sets state of roster service.
	 * @param state new state
	 */
	void setState(JabberRosterState state);

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when protocol disconencted.
	 *
	 * Roster state is reset to StateNonInitialized.
	 */
	void disconnected();

	void contactAddedSlot(Contact contact);
	void contactRemovedSlot(Contact contact);
	void contactUpdatedSlot(Contact contact);

	void remoteContactUpdated(const RosterItem &item);
	void remoteContactDeleted(const RosterItem &item);

	void rosterTaskFinished();
	void rosterTaskDeleted(QObject *object);

	void rosterRequestFinished(bool success);

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return true if local update can be processed.
	 * @return true if local update can be processed
	 *
	 * Local update can only be processed when roster is in StateInitialized. Derivered services can override this
	 * method and add more conditions.
	 */
	bool canPerformLocalUpdate() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return true if remote update for given contact can be processed.
	 * @param contact contact to check
	 * @return true if remote update can be processed
	 *
	 * Remote update can only be processed for either anonymous contacts or contacts than can accept remote updates (not detached
	 * and not currently synchronizing) when there is no task for given contact.
	 */
	bool canPerformRemoteUpdate(const Contact &contact) const;

	void executeAllTasks();
	void executeTask(const RosterTask &task);

public:
	explicit JabberRosterService(Protocol *protocol, const QVector<Contact> &contacts, QObject *parent = 0);
	virtual ~JabberRosterService();

    virtual RosterServiceTasks * tasks() const override;

	JabberRosterState state() const { return State; }

	void prepareRoster();

	void setClient(Client *xmppClient);

signals:
	/**
	 * @short Signal emitted when roster is ready
	 * @param ok true, if preparing roster was successfull
	 */
	void rosterReady(bool ok);

};

}

#endif // JABBER_ROSTER_SERVICE_H
