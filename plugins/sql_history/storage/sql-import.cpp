/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "misc/path-conversion.h"

#include "sql-import.h"

#define CURRENT_SCHEMA_VERSION 3

quint16 SqlImport::databaseSchemaVersion(QSqlDatabase &database)
{
	// no schema_version table
	if (!database.tables().contains("schema_version"))
	{
		if (!database.tables().contains("kadu_messages"))
			return 0; // first run of module, so no version available
		else
			return 1; // first slow version of SQL module
	}

	QSqlQuery query(database);
	query.prepare("SELECT version FROM schema_version");

	if (!query.exec()) // looks like broken database, we should make a fatal error or something now
		return 0;

	if (!query.next()) // looks like broken database, we should make a fatal error or something now
		return 0;

	return query.value(0).toUInt();
}

bool SqlImport::importNeeded(QSqlDatabase &database)
{
	return CURRENT_SCHEMA_VERSION > databaseSchemaVersion(database);
}

void SqlImport::initTables(QSqlDatabase &database)
{
	initKaduSchemaTable(database);
	initKaduMessagesTable(database);
	initKaduStatusesTable(database);
	initKaduSmsTable(database);
}

void SqlImport::initKaduSchemaTable(QSqlDatabase &database)
{
	QSqlQuery query(database);

	query.prepare("CREATE TABLE IF NOT EXISTS schema_version(version id INTEGER);");
	query.exec();

	query.prepare("DELETE FROM schema_version;");
	query.exec();

	query.prepare(QString("INSERT INTO schema_version (version) VALUES (%1);").arg(CURRENT_SCHEMA_VERSION));
	query.exec();
}

void SqlImport::initKaduMessagesTable(QSqlDatabase &database)
{
	QSqlQuery query(database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	query.exec();

	query.prepare("PRAGMA synchronous = OFF;");
	query.exec();

	query.prepare("PRAGMA foreign_keys = ON;");
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_chats ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"uuid VARCHAR(16));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_contacts ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"uuid VARCHAR(16));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_dates ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"date VARCHAR(8));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_message_contents ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"content TEXT,"
			"attributes VARCHAR(25));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_messages ("
			"chat_id INTEGER,"
			"contact_id INTEGER,"
			"date_id INTEGER,"
			"send_time TIMESTAMP,"
			"receive_time TIMESTAMP,"
			"content_id INTEGER,"
			"is_outgoing BOOL,"
			"FOREIGN KEY (chat_id) REFERENCES kadu_chats(id),"
			"FOREIGN KEY (contact_id) REFERENCES kadu_contacts(id),"
			"FOREIGN KEY (date_id) REFERENCES kadu_dates(id),"
			"FOREIGN KEY (content_id) REFERENCES kadu_message_contents(id));"
	);
	query.exec();
}

void SqlImport::initKaduStatusesTable(QSqlDatabase &database)
{
	QSqlQuery query(database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	query.exec();

	query.prepare("PRAGMA synchronous = OFF;");
	query.exec();

	query.prepare(
		"CREATE TABLE kadu_statuses ("
			"contact VARCHAR(255),"
			"status VARCHAR(255),"
			"set_time TIMESTAMP,"
			"description TEXT);"
	);
	query.exec();
}

void SqlImport::initKaduSmsTable(QSqlDatabase &database)
{
	QSqlQuery query(database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	query.exec();

	query.prepare("PRAGMA synchronous = OFF;");
	query.exec();

	query.prepare(
		"CREATE TABLE kadu_sms ("
			"receipient VARCHAR(255),"
			"send_time TIMESTAMP,"
			"content TEXT);"
	);
	query.exec();
}

void SqlImport::initIndexes(QSqlDatabase &database)
{
	QSqlQuery query(database);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_chat_pk ON kadu_chats (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_chat_uuid ON kadu_chats (uuid)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_contact_pk ON kadu_contacts (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_contact_uuid ON kadu_contacts (uuid)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_dates_pk ON kadu_dates (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_dates_val ON kadu_dates (date)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_content_pk ON kadu_message_contents (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_chat ON kadu_messages (chat_id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_contact ON kadu_messages (contact_id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_date ON kadu_messages (date_id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_content ON kadu_messages (content_id)");
	query.exec();
}

void SqlImport::importVersion1Schema(QSqlDatabase &database)
{
	QSqlQuery query(database);
	database.transaction();

	QStringList queries;
	queries
			<< "PRAGMA foreign_keys = ON;"
			<< "DROP INDEX IF EXISTS kadu_messages_chat;"
			<< "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_rowid;"
			<< "DROP INDEX IF EXISTS kadu_messages_chat_receive_time;"
			<< "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_date;"
			<< "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_send_time;"
			<< "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_date_send_time;"
			<< "DROP INDEX IF EXISTS kadu_statuses_contact;"
			<< "DROP INDEX IF EXISTS kadu_statuses_contact_time;"
			<< "DROP INDEX IF EXISTS kadu_statuses_contact_time_date;"
			<< "DROP INDEX IF EXISTS kadu_sms_receipient;"
			<< "DROP INDEX IF EXISTS kadu_sms_receipient_time;"
			<< "DROP INDEX IF EXISTS kadu_sms_receipient_time_date;"

			<< "CREATE TABLE kadu_chats (id INTEGER PRIMARY KEY AUTOINCREMENT, uuid VARCHAR(16));"
			<< "CREATE TABLE kadu_contacts (id INTEGER PRIMARY KEY AUTOINCREMENT, uuid VARCHAR(16));"
			<< "CREATE TABLE kadu_message_contents (id INTEGER PRIMARY KEY AUTOINCREMENT, content TEXT, attributes VARCHAR(25));"
			<< "CREATE TABLE kadu_dates (id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER);"

			<< "ALTER TABLE kadu_messages RENAME TO kadu_messages_old;"

			<< "INSERT INTO kadu_chats (uuid) SELECT DISTINCT chat FROM kadu_messages_old;"
			<< "INSERT INTO kadu_contacts (uuid) SELECT DISTINCT sender FROM kadu_messages_old;"
			<< "INSERT INTO kadu_dates (date) SELECT DISTINCT REPLACE(substr(receive_time,0,11), '-', '') FROM kadu_messages_old;"
			<< "INSERT INTO kadu_message_contents (content) SELECT DISTINCT content FROM kadu_messages_old;"

			<< "CREATE TABLE kadu_messages ("
				"chat_id INTEGER,"
				"contact_id INTEGER,"
				"date_id INTEGER,"
				"send_time TIMESTAMP,"
				"receive_time TIMESTAMP,"
				"content_id INTEGER,"
				"is_outgoing BOOL, "
				"FOREIGN KEY (chat_id) REFERENCES kadu_chats(id), FOREIGN KEY (contact_id) REFERENCES kadu_contacts(id), "
				"FOREIGN KEY (date_id) REFERENCES kadu_dates(id), FOREIGN KEY (content_id) REFERENCES kadu_message_contents(id));"

			<< "INSERT INTO kadu_messages ("
				"chat_id, contact_id, date_id, send_time, receive_time, content_id, is_outgoing) "
				"SELECT (SELECT id FROM kadu_chats WHERE uuid=old.chat LIMIT 1), (SELECT id FROM kadu_contacts WHERE uuid=old.sender LIMIT 1), "
				"(SELECT id FROM kadu_dates WHERE date = REPLACE(substr(old.receive_time,0,11), '-', '')), send_time, receive_time, "
				"(SELECT id FROM kadu_message_contents WHERE content=old.content LIMIT 1), substr(attributes, 10, 1) FROM kadu_messages_old old;"

			<< "DROP TABLE kadu_messages_old;"

			<< "CREATE TABLE schema_version(version id INTEGER);"
			<< "DELETE FROM schema_version;"
			<< QString("INSERT INTO schema_version (version) VALUES (%1);").arg(CURRENT_SCHEMA_VERSION);

	foreach (const QString &queryString, queries)
	{
		query.prepare(queryString);
		query.setForwardOnly(true);
		query.exec();
	}

	initIndexes(database);

	database.commit();

	query.prepare("VACUUM;");
	query.exec();
}

void SqlImport::importVersion2Schema(QSqlDatabase &database)
{
	database.transaction();

	removeDuplicatesFromVersion2Schema(database, "kadu_chats", "uuid", "chat_id");
	removeDuplicatesFromVersion2Schema(database, "kadu_contacts", "uuid", "contact_id");
	removeDuplicatesFromVersion2Schema(database, "kadu_dates", "date", "date_id");
	initKaduSchemaTable(database);

	database.commit();

	QSqlQuery query(database);
	query.prepare("VACUUM;");
	query.exec();
}

void SqlImport::removeDuplicatesFromVersion2Schema(QSqlDatabase &database, const QString &idTableName, const QString &valueFieldName, const QString &idFieldName)
{
	QSqlQuery query(database);

	// typedef is needed for foreach
	typedef QPair<QString, QStringList> IdsPair;
	QHash<QString, IdsPair> chats;
	query.prepare(QString("SELECT id, %1 FROM %2;").arg(valueFieldName, idTableName));
	query.setForwardOnly(true);
	query.exec();
	while (query.next())
	{
		QString id = query.value(0).toString();
		QString value = query.value(1).toString();
		if (!chats.contains(value))
			chats.insert(value, qMakePair(id, QStringList()));
		else
			chats[value].second.append(id);
	}

	QStringList badIds;
	foreach (const IdsPair &pair, chats)
	{
		if (pair.second.isEmpty())
			continue;

		badIds << pair.second;

		// This should be prepared outside of loop and ids should be only binded here.
		// But I have no idea how to bind a list...
		query.prepare(QString("UPDATE kadu_messages SET %1 = %2 WHERE %1 IN (%3)").arg(idFieldName, pair.first, pair.second.join(", ")));
		query.setForwardOnly(true);
		query.exec();
	}

	query.prepare(QString("DELETE FROM %1 WHERE id IN (%2);").arg(idTableName, badIds.join(", ")));
	query.setForwardOnly(true);
	query.exec();
}

void SqlImport::performImport(QSqlDatabase &database)
{
	quint16 storedSchemaVersion = databaseSchemaVersion(database);

	switch (storedSchemaVersion)
	{
		case 0:
			database.transaction();
			initTables(database);
			initIndexes(database);
			database.commit();
			break;
		case 1:
			importVersion1Schema(database);
			// We do not need to call importVersion2Schema() here as the only thing it does
			// over importVersion1Schema() is cleaninig up after an already fixed bug which
			// polluted chat, contact, and date id's tables.
			break;
		case 2:
			importVersion2Schema(database);
			break;
		default:
			break; // no need to import
	}
}
