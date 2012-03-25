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

#ifndef SQL_CHATS_MAPPING_H
#define SQL_CHATS_MAPPING_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

#include "chat/chat.h"

class SqlAccountsMapping;
class SqlContactsMapping;

/**
 * @addtogroup SqlHistory
 * @{
 */

/**
 * @class SqlChatsMapping
 * @author Rafał 'Vogel' Malinowski
 * @short This class is responsible for mapping IM chats to database entries in kadu_chats table.
 *
 * This class is responsible for mapping IM chats to database entries in kadu_chats table of database
 * provided in costructor. This class does not check for validity of database tables.
 *
 * Only chats used in history have mapping to entry in kadu_chats table in databse. If this class is asked for
 * database id of chat that does not have one, then it is given a new id.
 *
 * Each chat has custom property named sql_history:id that holds database id. If this property is not
 * positive integer, then given chat does not have databse id. Value of this property can be easily obtained by
 * @link idByChat @endlink method.
 *
 * Also a chat can be obtained from database id by calling @link chatById @endlink. This method can return
 * Chat::null if no valid mapping exists for given id.
 *
 * This class holds instance of @link SqlAccountsMapping @endlink and @link SqlContactsMapping @endlink to get
 * account to database mapping and contact to database mapping that are required to store full chat information in database.
 */
class SqlChatsMapping : public QObject
{
	Q_OBJECT

	const QSqlDatabase &Database;
	SqlAccountsMapping *AccountsMapping;
	SqlContactsMapping *ContactsMapping;
	QMap<int, Chat> ChatMapping;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add mapping of given id to given contact.
	 * @param id id to map
	 * @param contact contact to map
	 *
	 * This method operates only on internal QMap and chats's custom properties. No database changes
	 * are performed.
	 */
	void addMapping(int id, const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Load mapping from database.
	 *
	 * This method loads mapping from database. All database entries from kadu_chats table are matched
	 * to existing chats. Entries without valid chats will be ignored by this method.
	 */
	void loadMappingsFromDatabase();

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Update SQL mapping of changed chat.
	 * @param chat changes chat
	 *
	 * This method is called when chat's data changes. Probably an update is required in mapping table.
	 */
	void chatUpdated(const Chat &chat);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of SqlChatsMapping class.
	 * @param database database with kadu_chats table
	 * @param accountsMapping object providing account to database mapping, must be not null
	 * @param contactsMapping object providing chat to database mapping, must be not null
	 * @param parent QObject parent of new SqlChatsMapping object
	 *
	 * Database provided in this constructor is not checked for existence or validity of kadu_chats table. It is caller responsibility
	 * to provide proper database.
	 */
	explicit SqlChatsMapping(const QSqlDatabase &database, SqlAccountsMapping *accountsMapping, SqlContactsMapping *contactsMapping, QObject *parent = 0);
	virtual ~SqlChatsMapping();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return Chat matched with given id from database.
	 * @param sqlId id from database
	 *
	 * This method return a Chat that matches with given id from database. If no matching contact is found, Chat::null is returned.
	 */
	Chat chatById(int sqlId) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return sql id matched with given chat.
	 * @param chat chat to match to sql id
	 * @param create if true then valid id will be created if not already available
	 *
	 * This method return sql id that matches with given chat. If no matching id is found and create is false, 0 is returned. If create
	 * is true and no id is available then new one will be created and assigned to given contact.
	 */
	int idByChat(const Chat &chat, bool create);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove chat from mapping.
	 * @param chat chat to remove
	 *
	 * Given chat is removed from mapping. It can by re-added by using @link idByChat @endlink method.
	 */
	void removeChat(const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return current mappings.
	 * @param return current mappings
	 */
	const QMap<int, Chat> & mapping() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Converts chat to string that can be stored in database.
	 * @param chat chat to convert
	 * @return String representation of given chat.
	 *
	 * Currently Kadu supports only two types of chat: Contact and ContactSet chats. String representation of these chats look like:
	 * <type-name>;<contact-id-1>;<contact-id-1>....
	 */
	QString chatToString(const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Converts string from database to chat.
	 * @param account account of converted chat
	 * @param string string  to convert
	 * @return Chat that is representated by given string.
	 *
	 * This method return Chat::null only if given string can not be translated to a valid chat. No items are added to @link ChatManager @endlink.
	 */
	Chat stringToChat(const Account &account, const QString &string);

};

/**
 * @}
 */

#endif // SQL_CHATS_MAPPING_H
