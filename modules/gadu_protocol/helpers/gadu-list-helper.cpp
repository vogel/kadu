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
#include "contacts/contact-shared.h"

#include "protocols/protocol.h"

#include "debug.h"
#include "misc/misc.h"

#include "../gadu-contact-details.h"

#include "gadu-list-helper.h"

QString GaduListHelper::contactListToString(Account account, BuddyList buddies)
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

BuddyList GaduListHelper::stringToContactList(Account account, QString &content) {
	QTextStream stream(&content, QIODevice::ReadOnly);
	return streamToContactList(account, stream);
}

BuddyList GaduListHelper::streamToContactList(Account account, QTextStream &content)
{
	BuddyList result;

	QStringList sections;
	QList<Group> groups;
	QString line;
	unsigned int i, secCount;
	bool ok;

	content.setCodec(codec_latin2);

	while (!content.atEnd())
	{
		Buddy buddy;
		line = content.readLine();
//		kdebugm(KDEBUG_DUMP, ">>%s\n", qPrintable(line));
		sections = line.split(";", QString::KeepEmptyParts);
		secCount = sections.count();

		if (secCount < 7)
			continue;

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
				Contact contact;
				contact.setContactAccount(account);
				contact.setOwnerBuddy(buddy);
				contact.setId(QString::number(uin));
				contact.data()->setState(StorableObject::StateNew);
				contact.setDetails(new GaduContactDetails(contact));
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
		result.append(buddy);
	}

	return result;
}
