/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef ROSTER_SERVICE_H
#define ROSTER_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy.h"
#include "contacts/contact.h"

#include "exports.h"

#include "protocols/services/protocol-service.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class RosterService
 * @author Rafał 'Vogel' Malinowski
 * @short Roster protocol service allows adding, removing and updating contacts on remote roster.
 *
 * This service allows adding, removing and updating contacts on remote roster. Every added contact is watched
 * for changes and updated automatically, until it is removed.
 *
 * Four methods should be overrided by subclasses: addContact(), removeContact(), updateContact() and prepareRoster().
 * First two of them returns true if contact was properly added or removed and false otherwise.
 *
 * Roster service has a state. Local updates are propagated to remote server only when state is StateInitialized.
 * During operations it is changed to StateProcessingRemoteUpdate or StateProcessingLocalUpdate (depending on
 * source of change). StateNonInitialized is used before calling prepareRoster() and StateInitializing during this
 * call.
 *
 * Signal rosterRead() is emited after calling prepareRoster() when implementation decidec that preparation was finished.
 */
class KADUAPI RosterService : public ProtocolService
{
	Q_OBJECT

public:
	/**
	 * @enum RosterState
	 * @author Rafał 'Vogel' Malinowski
	 * @short State of roster service.
	 */
	enum RosterState {
		/**
		 * Roster service was not initialized and cannot perform any operation except prepareRoster().
		 */
		StateNonInitialized,
		/**
		 * Roster service is during prepareRoster() operation.
		 */
		StateInitializing,
		/**
		 * Roster is initialized and ready to accept local or remote changes.
		 */
		StateInitialized,
		/**
		 * Remote update was detected and is not being processed to update local roster.
		 */
		StateProcessingRemoteUpdate,
		/**
		 * Local update was detected and is not being processed to update remote roster.
		 */
		StateProcessingLocalUpdate
	};

private:
	RosterState State;
	QList<Contact> Contacts;

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when protocol disconencted.
	 *
	 * Roster state is reset to StateNonInitialized.
	 */
	void disconnected();

	/**
	 * @enum RosterState
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when data of contact or contact's owner buddy changed.
	 *
	 * This slot can only by called for contacts that were previously added to roster using addContact() methods
	 * and were not removed.
	 */
	void contactUpdated();

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return true if local update can be processed.
	 * @return true if local update can be processed
	 *
	 * Local update can only be processed when roster is in StateInitialized. Derivered services can override this
	 * method and add another conditions.
	 */
	virtual bool canPerformLocalUpdate() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short sets state of roster service
	 * @param state new state
	 */
	void setState(RosterState state);

protected slots:
	/**
	 * @enum RosterState
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called when data of contact or contact's owner buddy changed.
	 *
	 * This method is only called for contacts that were previously added to roster using addContact() methods
	 * and were not removed and state of roster service is StateInitialized.
	 *
	 * Derivered services must reimplement this method.
	 */
	virtual void updateContact(const Contact &contact) = 0;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of RosterService bound to given Protocol.
	 * @param protocol protocol to bound this service to
	 */
	explicit RosterService(Protocol *protocol);
	virtual ~RosterService();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return current state of this service.
	 * @return current state of this service
	 */
	RosterState state() const { return State; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Prepare roster to its work.
	 *
	 * This method must be reimplemented by derivered services. Depending on protocol it should download remote roster,
	 * upload local one, merge both or do nothig. After successfull (or not) preparation rosterReady() signal must be
	 * emited.
	 */
	virtual void prepareRoster() = 0;

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add new contact to roster.
	 * @param contact new contact
	 * @return true if adding was successfull
	 *
	 * This method add new contact to roster. Derivered services should reimplement this method and call
	 * RosterService::addContact at begining and check it return value - when false, no remote adding should be done.
	 *
	 * This implementation adds contact to internal list and returns true if contact was not added before. It also
	 * starts watching on changes on this contact.
	 */
	virtual bool addContact(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove contact from roster.
	 * @param contact contact to remove
	 * @return true if removing was successfull
	 *
	 * This method removes contact from roster. Derivered services should reimplement this method and call
	 * RosterService::removeContact at begining and check it return value - when false, no remote removing should be done.
	 *
	 * This implementation removes contact from internal list and returns true if contact was added before. It also
	 * stops watching on changes on this contact.
	 */
	virtual bool removeContact(const Contact &contact);

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when prepareRoster() operation is finished
	 * @param ok true, if preparing roster was successfull
	 */
	void rosterReady(bool ok);

};

/**
 * @}
 */

#endif // ROSTER_SERVICE_H
