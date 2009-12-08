/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-shared.h"

#include "protocols/protocol.h"

#include "debug.h"
#include "misc/misc.h"

#include "../gadu-contact-details.h"

#include "gadu-list-helper.h"

QString GaduListHelper::buddyListToString(Account account, BuddyList buddies)
{
	kdebugf();

	QStringList contactsStringList;

	foreach (Buddy buddy, buddies)
	{
		QStringList buddyGroups;
		foreach (Group group, buddy.groups())
			buddyGroups << group.name();

		Contact contact = buddy.contacts(account)[0]; // NULL!!!!

		contactsStringList << QString("%1;%2;%3;%4;%5;%6;%7;%8;%9;%10;%11;%12;%13")
			.arg(buddy.firstName())
			.arg(buddy.lastName())
			.arg(buddy.nickName())
			.arg(buddy.display())
			.arg(buddy.mobile())
			.arg(buddyGroups.join(";"))
			.arg(contact.id())
			// TODO: 0.6.6
			.arg("0")
			.arg("")
			.arg("0")
			.arg("")
			.arg(buddy.isOfflineTo())
			.arg(buddy.homePhone());
	}

// 			file = user.aliveSound(type);
// 			contacts += QString::number(type);				contacts += ';';
// 			contacts += file;								contacts += ';';
// 			file = user.messageSound(type);
// 			contacts += QString::number(type);				contacts += ';';
// 			contacts += file;								contacts += ';';

	QString contactsString(contactsStringList.join("\r\n"));
	contactsString.remove("(null)");

	return contactsString;
}

BuddyList GaduListHelper::stringToBuddyList(Account account, QString &content) {
	QTextStream stream(&content, QIODevice::ReadOnly);
	return streamToBuddyList(account, stream);
}

BuddyList GaduListHelper::streamToBuddyList(Account account, QTextStream &content)
{
	BuddyList result;
	QStringList sections;
	QString line;
	bool gg70 = false;

	content.setCodec(codec_latin2);

	while (!content.atEnd())
	{
		line = content.readLine();
		sections = line.split(";", QString::KeepEmptyParts);
		//kdebugm(KDEBUG_DUMP, ">>%s\n", qPrintable(line));
		if (line.startsWith("GG70ExportString"))
		{
			gg70 = true;
			continue;
		}
		
		if (sections.count() < 7)
			continue;

		result.append(gg70 ? line70ToBuddy(account, sections) : linePre70ToBuddy(account, sections));
	}

	return result;
}

Buddy GaduListHelper::linePre70ToBuddy(Account account, QStringList &sections)
{
	QList<Group> groups;
	unsigned int i, secCount;
	bool ok;

	Buddy buddy = Buddy::create();

	secCount = sections.count();

	buddy.setFirstName(sections[0]);
	buddy.setLastName(sections[1]);
	buddy.setNickName(sections[2]);
	buddy.setDisplay(sections[3]);
	buddy.setMobile(sections[4]);

	groups.clear();
	if (!sections[5].isEmpty())
		groups.append(GroupManager::instance()->byName(sections[5]));

	i = 6;
	ok = false;
	while (!ok && i < secCount)
	{
		sections[i].toULong(&ok);
		ok = ok || sections[i].isEmpty();
		if (!ok)
			groups.append(GroupManager::instance()->byName(sections[i]));
		++i;
	}
	buddy.setGroups(groups);
	--i;

	if (i < secCount)
	{
		UinType uin = sections[i++].toULong(&ok);
		if (!ok)
			uin = 0;
		if (uin)
		{
			Contact contact = ContactManager::instance()->byId(account, QString::number(uin));
			if (contact.isNull())
			{
				contact = Contact::create();
				contact.setContactAccount(account);
				GaduContactDetails *details = new GaduContactDetails(contact);
				details->setState(StorableObject::StateNew);
				contact.setDetails(details);
				contact.setId(QString::number(uin));
				contact.data()->setState(StorableObject::StateNew);
			}

			contact.setOwnerBuddy(buddy);
			buddy.addContact(contact);
		}
	}

	if (i < secCount)
		buddy.setEmail(sections[i++]);

// TODO: 0.6.6
	if (i+1 < secCount)
	{
// 			contact.setAliveSound((NotifyType)sections[i].toInt(), sections[i+1]);
		i+=2;
	}
	if (i+1 < secCount)
	{
// 			e.setMessageSound((NotifyType)sections[i].toInt(), sections[i+1]);
		i+=2;
	}

	if (i < secCount)
	{
		buddy.setOfflineTo(bool(sections[i].toInt()));
		i++;
	}

	if (i < secCount)
		buddy.setHomePhone(sections[i++]);

	buddy.setAnonymous(false);
	return buddy;
}

Buddy GaduListHelper::line70ToBuddy(Account account, QStringList &sections)
{
	QList<Group> groups;
	unsigned int i, secCount;
	bool ok = false;

	kdebugm(KDEBUG_DUMP, "\n>==>%s", qPrintable(sections.join("|")));

	Buddy buddy = Buddy::create();

	secCount = sections.count();

	buddy.setFirstName(sections[0]);
	buddy.setLastName(sections[1]);
	buddy.setNickName(sections[2]);
	buddy.setDisplay(sections[3]);
	buddy.setMobile(sections[4]);

	if (!sections[5].isEmpty())
	{
		foreach (const QString group, sections[5].split(",", QString::SkipEmptyParts))
			groups.append(GroupManager::instance()->byName(group));

		buddy.setGroups(groups);
	}

	i = 6;
	if (i < secCount)
	{
		UinType uin = sections[i++].toULong(&ok);
		if (!ok)
			uin = 0;
		if (uin)
		{
			Contact contact = ContactManager::instance()->byId(account, QString::number(uin));
			if (contact.isNull())
			{
				contact = Contact::create();
				contact.setContactAccount(account);
				GaduContactDetails *details = new GaduContactDetails(contact);
				details->setState(StorableObject::StateNew);
				contact.setDetails(details);
				contact.setId(QString::number(uin));
				contact.data()->setState(StorableObject::StateNew);
			}

			contact.setOwnerBuddy(buddy);
			buddy.addContact(contact);
		}
	}

	if (i < secCount)
		buddy.setEmail(sections[i++]);

// TODO: 0.6.6
	if (i+1 < secCount)
	{
// 			contact.setAliveSound((NotifyType)sections[i].toInt(), sections[i+1]);
		i+=2;
	}
	if (i+1 < secCount)
	{
// 			e.setMessageSound((NotifyType)sections[i].toInt(), sections[i+1]);
		i+=2;
	}

	if (i < secCount)
	{
		buddy.setOfflineTo(bool(sections[i].toInt()));
		i++;
	}

	if (i < secCount)
		buddy.setHomePhone(sections[i++]);

	buddy.setAnonymous(false);
	return buddy;
}
