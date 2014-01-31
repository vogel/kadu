/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtSql/QSqlQuery>

#include "accounts/account.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact-set.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-room.h"
#include "contacts/contact-set.h"

#include "storage/sql-accounts-mapping.h"
#include "storage/sql-contacts-mapping.h"

#include "sql-chats-mapping.h"

SqlChatsMapping::SqlChatsMapping(const QSqlDatabase &database, SqlAccountsMapping *accountsMapping, SqlContactsMapping *contactsMapping, QObject *parent) :
		QObject(parent), Database(database), Mutex(QMutex::Recursive), AccountsMapping(accountsMapping), ContactsMapping(contactsMapping)
{
	Q_ASSERT(AccountsMapping);
	Q_ASSERT(ContactsMapping);

	loadMappingsFromDatabase();

	connect(ChatManager::instance(), SIGNAL(chatUpdated(Chat)), this, SLOT(chatUpdated(Chat)));
}

SqlChatsMapping::~SqlChatsMapping()
{
}

void SqlChatsMapping::chatUpdated(const Chat &chat)
{
	QMutexLocker locker(&Mutex);

	if (idByChat(chat, false) <= 0)
		return;

	QSqlQuery query(Database);
	query.prepare("UPDATE kadu_chats SET account_id = :account_id, chat = :chat WHERE id = :id");
	query.bindValue(":account_id", AccountsMapping->idByAccount(chat.chatAccount()));
	query.bindValue(":chat", chatToString(chat));
	query.bindValue(":id", idByChat(chat, false));
	query.exec();
}

void SqlChatsMapping::addMapping(int id, const Chat &chat)
{
	QMutexLocker locker(&Mutex);

	chat.addProperty("sql_history:id", id, CustomProperties::NonStorable);
	ChatMapping.insert(id, chat);
}

void SqlChatsMapping::loadMappingsFromDatabase()
{
	QMutexLocker locker(&Mutex);

	QSqlQuery query(Database);
	query.prepare("SELECT id, account_id, chat FROM kadu_chats");

	query.setForwardOnly(true);
	query.exec();

	while (query.next())
	{
		int id = query.value(0).toInt();
		Account account = AccountsMapping->accountById(query.value(1).toInt());
		QString chatString = query.value(2).toString();

		if (id <= 0)
			continue;

		Chat chat = stringToChat(account, chatString);
		if (chat)
			addMapping(id, chat);
	}
}

Chat SqlChatsMapping::chatById(int sqlId) const
{
	QMutexLocker locker(&Mutex);

	if (ChatMapping.contains(sqlId))
		return ChatMapping.value(sqlId);
	else
		return Chat::null;
}

int SqlChatsMapping::idByChat(const Chat &chat, bool create)
{
	QMutexLocker locker(&Mutex);

	int id = chat.property("sql_history:id", 0).toInt();
	if (!create || id > 0)
		return id;

	QString chatString = chatToString(chat);
	if (chatString.isEmpty())
		return 0;

	QSqlQuery query(Database);
	query.prepare("INSERT INTO kadu_chats (account_id, chat) VALUES (:account_id, :chat)");
	query.bindValue(":account_id", SqlAccountsMapping::idByAccount(chat.chatAccount()));
	query.bindValue(":chat", chatToString(chat));
	query.exec();

	id = query.lastInsertId().toInt();
	addMapping(id, chat);

	return id;
}

void SqlChatsMapping::removeChat(const Chat &chat)
{
	QMutexLocker locker(&Mutex);

	int id = idByChat(chat, false);
	chat.removeProperty("sql_history:id");

	if (ChatMapping.contains(id))
		ChatMapping.remove(id);
}

const QMap<int, Chat> & SqlChatsMapping::mapping() const
{
	QMutexLocker locker(&Mutex);

	return ChatMapping;
}

QString SqlChatsMapping::chatToString(const Chat &chat)
{
	if ("Room" == chat.type())
	{
		ChatDetailsRoom *details = qobject_cast<ChatDetailsRoom *>(chat.details());
		if (!details || details->room().isEmpty())
			return QString();

		return QString("Room;") + details->room();
	}

	if ("Contact" == chat.type())
		return QString("Contact;") + QString::number(ContactsMapping->idByContact(chat.contacts().toContact(), true));

	if ("ContactSet" == chat.type())
	{
		QStringList result;
		result << "ContactSet";

		foreach (const Contact &contact, chat.contacts())
			result << QString::number(ContactsMapping->idByContact(contact, true));

		return result.join(";");
	}

	return QString();
}

Chat SqlChatsMapping::stringToChat(const Account &account, const QString &string)
{
	if (!account)
		return Chat::create();

	QStringList items = string.split(";", QString::SkipEmptyParts);
	int len = items.length();

	if (len < 2)
		return Chat::create();

	QString chatType = items.at(0);

	if ("Room" == chatType)
		return ChatTypeRoom::findChat(account, items.at(1), ActionCreateAndAdd);

	if ("Contact" == chatType)
	{
		Contact contact = ContactsMapping->contactById(items.at(1).toInt());
		if (!contact)
			return Chat::null;

		return ChatTypeContact::findChat(contact, ActionCreateAndAdd);
	}

	if ("ContactSet" == chatType)
	{
		ContactSet contacts;
		for (int i = 1; i < len; i++)
		{
			Contact contact = ContactsMapping->contactById(items.at(i).toInt());
			if (!contact)
				return Chat::null;

			contacts.insert(contact);
		}

		return ChatTypeContactSet::findChat(contacts, ActionCreateAndAdd);
	}

	return Chat::create();
}

#include "moc_sql-chats-mapping.cpp"
