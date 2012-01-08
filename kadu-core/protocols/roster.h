/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ROSTER_H
#define ROSTER_H

#include <QtCore/QObject>

#include "contacts/contact.h"

class RosterService;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class Roster
 * @author Rafał 'Vogel' Malinowski
 * @short Generic interface to all RosterService objects
 *
 * This class allows adding, removing and updating contacts on remote roster. Adding contacts is done by addContact() method,
 * removing - by removeContact() method. This class searchs for suitable RosterService and uses it to perform its job.
 *
 * Use this class to manipulate rosters. After adding a contact to any roster, its data is updated automatically, until
 * it is removed.
 */
class Roster : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Roster)

	static Roster *Instance;

	Roster();
	virtual ~Roster();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return singleton instance of Roster calss.
	 * @return singleton instance of Roster calss
	 */
	static Roster * instance();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return RosterService instance suitable to given contact.
	 * @param contact contact to find RosterService for
	 * @return RosterService instance suitable to given contact
	 */
	static RosterService * rosterService(const Contact &contact);

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add new contact to remote roster.
	 * @param contact new contact to be added
	 */
	void addContact(const Contact &contact) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove contact from remote roster.
	 * @param contact contact to be removed
	 */
	void removeContact(const Contact &contact) const;

};

/**
 * @}
 */

#endif // ROSTER_H
