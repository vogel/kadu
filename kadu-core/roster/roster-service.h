/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "protocols/services/protocol-service.h"

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class Protocol;
class RosterServiceTasks;

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
 * Implementation depends on protocol.
 *
 * For example, XMPP uses tasks based roster. For XMPP tasks() return valid instance of RosterServiceTasks.
 * For GaduGadu it returns nullptr, as GaduGadu uses batch mode to update roster.
 */
class KADUAPI RosterService : public ProtocolService
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of RosterService bound to given Account.
	 * @param protocol to bound this service to
	 * @param contacts initial list of account contacts that should be stored in Roster
	 */
	explicit RosterService(const QVector<Contact> &contacts, Protocol *protocol);
	virtual ~RosterService();

	/**
	 * @return RosterServiceTasks instance that contains all roster tasks
	 *
	 * GaduGadu does not support this - all roster changes are done in one go, and it may be impossible
	 * to split them in tasks. In Gadu if we have the same version of contact list as server then it can
	 * be safely updated in one go. If not - server version is merged with local one. That means local
	 * deletion may be reverted. So GaduGadu return null for this method.
	 *
	 * XMPP fully supports roster task.
	 */
	virtual RosterServiceTasks * tasks() const;

	/**
	 * @return List of contacts on local roster. These contacts are watched for changes.
	 */
	const QVector<Contact> & contacts() const;

public slots:
	/**
	 * @short Add new contact to roster.
	 * @param contact new contact
	 *
	 * If given contact is valid and belongs to current account, then new roster task for adding item to roster is created.
	 * If it is possible, task is executed immediately. If not, it is stored for later execution.
	 */
	void addContact(const Contact &contact);

	/**
	 * @short Remove contact from roster.
	 * @param contact contact to remove
	 *
	 * If given contact is valid and belongs to current account, then new roster task for deleting item from roster is created.
	 * If it is possible, task is executed immediately. If not, it is stored for later execution.
	 */
	void removeContact(const Contact &contact);

signals:
	/**
	 * @short Signal emitted when new contact is added to roster
	 * @param contact added contact
	 *
	 * Signal is not emitted for contacts added as initial in constructor.
	 * Signal is emitted for detached and attached contacts.
	 */
	void contactAdded(Contact contact);

	/**
	 * @short Signal emitted when contact is removed from roster
	 * @param contact removed contact
	 *
	 * Signal is emitted for detached and attached contacts.
	 */
	void contactRemoved(Contact contact);

	/**
	 * @short Signal emitted when contact is updated on roster
	 * @param contact updated contact
	 *
	 * Signal is emitted for detached and attached contacts.
	 */
	void contactUpdated(Contact contact);

	/**
	 * @short Signal emitted when contact is updated locally
	 * @param contact updated contact
	 *
	 * Signal is emitted for detached and attached contacts. Is not emitted when changes
	 * comes from synchronization from remote roster.
	 */
	void contactUpdatedLocally(Contact contact);

protected:
	/**
	 * @short Resets all synchronizing contacts to desynchronized.
	 *
	 * Should be used just after connection has been made. In case when synchronization operation was interrupted
	 * and roster entry can be lest in synchronizing state. This method fixes that by resetting it to desynchronized.
	 */
	void fixupInitialState();

private:
	QVector<Contact> m_contacts;

	/**
	 * @short Connects to data emitted by given contact when its roster data changes.
	 * @param contact contact to connect to
	 */
	void connectContact(const Contact &contact);

	/**
	 * @short Disconnects data emitted by given contact when its roster data changes.
	 * @param contact contact to disconnect from
	 */
	void disconnectContact(const Contact &contact);

private slots:
	/**
	 * @short Slot called when data of contact or contact's owner buddy changed.
	 *
	 * This slot can only by called for contacts that were previously added to roster using addContact() methods
	 * and were not removed.
	 */
	void contactUpdated();

	/**
	 * @short Slot called when data of contact or contact's owner buddy changed locally.
	 *
	 * This slot can only by called for contacts that were previously added to roster using addContact() methods
	 * and were not removed.
	 */
	void contactUpdatedLocally();

};

/**
 * @}
 */
