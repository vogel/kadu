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

#ifndef SQL_CONTACTS_MAPPING_H
#define SQL_CONTACTS_MAPPING_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

#include "contacts/contact.h"

class SqlAccountsMapping;

/**
 * @addtogroup SqlHistory
 * @{
 */

/**
 * @class SqlContactsMapping
 * @author Rafał 'Vogel' Malinowski
 * @short This class is responsible for mapping IM contacts to database entries in kadu_contacts table.
 *
 * This class is responsible for mapping IM contacts to database entries in kadu_contacts table of database
 * provided in costructor. This class does not check for validity of database tables.
 *
 * Only contacts used in history have mapping to entry in kadu_contacts table in databse. If this class is asked for
 * database id of contact that does not have one, then it is given a new id. Contact updates (changing accounts and/or
 * id) are reflected in database.
 *
 * Each contact has custom property named sql_history:id that holds database id. If this property is not
 * positive integer, then given contact does not have databse id. Value of this property can be easily obtained by
 * @link idByContact @endlink method.
 *
 * Also a contact can be obtained from database id by calling @link contactById @endlink. This method can return
 * Contact::null if no valid mapping exists for given id.
 *
 * This class holds instance of @link SqlAccountsMapping @endlink to get account to database mapping that is required
 * to store full contact information in database.
 */
class SqlContactsMapping : public QObject
{
	Q_OBJECT

	const QSqlDatabase &Database;
	SqlAccountsMapping *AccountsMapping;
	QMap<int, Contact> ContactMapping;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add mapping of given id to given contact.
	 * @param id id to map
	 * @param contact contact to map
	 *
	 * This method operates only on internal QMap and contact's custom properties. No database changes
	 * are performed.
	 */
	void addMapping(int id, const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Load mapping from database.
	 *
	 * This method loads mapping from database. All database entries from kadu_contacts table are matched
	 * to existing contacts. Entries without valid contacts will be ignored by this method.
	 */
	void loadMappingsFromDatabase();

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Update kadu_contacts entry to new contact's data.
	 * @param contact updated contact
	 *
	 * This slot is called every time an contact that registered in @link ContactManager @endlink changes. Database data
	 * is updated to match new data of changed contact only if this contact is already in database.
	 */
	void contactUpdated(const Contact &contact);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of SqlContactsMapping class.
	 * @param database database with kadu_contact table
	 * @param accountsMapping object providing account to database mapping, must be not null
	 * @param parent QObject parent of new SqlContactsMapping object
	 *
	 * Database provided in this constructor is not checked for existence or validity of kadu_contacts table. It is caller responsibility
	 * to provide proper database.
	 */
	explicit SqlContactsMapping(const QSqlDatabase &database, SqlAccountsMapping *accountsMapping, QObject *parent = 0);
	virtual ~SqlContactsMapping();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return Contact matched with given id from database.
	 * @param sqlId id from database
	 *
	 * This method return a Contact that matches with given id from database. If no matching contact is found, Contact::null is returned.
	 */
	Contact contactById(int sqlId) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return sql id matched with given contact.
	 * @param contact contact to match to sql id
	 * @param create if true then valid id will be created if not already available
	 *
	 * This method return sql id that matches with given contact. If no matching id is found and create is false, 0 is returned. If create
	 * is true and no id is available then new one will be created and assigned to given contact.
	 */
	int idByContact(const Contact &contact, bool create);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return current mappings.
	 * @param return current mappings
	 */
	const QMap<int, Contact> & mapping() const;

};

/**
 * @}
 */

#endif // SQL_CONTACTS_MAPPING_H
