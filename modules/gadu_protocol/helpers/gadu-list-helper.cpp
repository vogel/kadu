/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
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

QString GaduListHelper::contactToLine70(Contact contact)
{
	QStringList list;
	Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);

	list.append(buddy.firstName());
	list.append(buddy.lastName());
	list.append(buddy.nickName());
	list.append(buddy.display());
	list.append(buddy.mobile());

	QStringList groups;
	foreach (Group group, buddy.groups())
		groups.append(group.name());

	list.append(groups.join(","));
	list.append(contact.id());
	list.append(buddy.email());
	list.append(""); // alive sound
	list.append(""); // alive sound
	list.append(""); // message sound
	list.append(""); // message sound
	list.append(""); // offlineTo 
	list.append(buddy.homePhone());

	return list.join(";");
}

QByteArray GaduListHelper::buddyListToByteArray(Account account, BuddyList buddies)
{
	kdebugf();

	QStringList result;
	result.append("GG70ExportString");

	foreach (Buddy buddy, buddies)
		foreach (Contact contact, buddy.contacts(account))
			result.append(contactToLine70(contact));

	return codec_cp1250->fromUnicode(result.join("\n"));
}

BuddyList GaduListHelper::byteArrayToBuddyList(Account account, QByteArray &content) {
	QTextStream stream(&content, QIODevice::ReadOnly);
	return streamToBuddyList(account, stream);
}

BuddyList GaduListHelper::streamToBuddyList(Account account, QTextStream &content)
{
	BuddyList result;
	QStringList sections;
	QString line;
	bool gg70 = false;

	content.setCodec(codec_cp1250);

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

		if (!gg70 && (sections.size() > 6))
		{
			bool ok = false;
			sections[6].toULong(&ok);
			if (ok)
				gg70 = true;
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
	bool ok = false;

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
			Contact contact = Contact::create();
			contact.setContactAccount(account);
			GaduContactDetails *details = new GaduContactDetails(contact.data());
			details->setState(StorableObject::StateNew);
			contact.setDetails(details);
			contact.setId(QString::number(uin));
			contact.data()->setState(StorableObject::StateNew);
			contact.setOwnerBuddy(buddy);
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

	Buddy buddy = Buddy::create();

	secCount = sections.count();

	buddy.setFirstName(sections[0]);
	buddy.setLastName(sections[1]);
	buddy.setNickName(sections[2]);
	buddy.setDisplay(sections[3]);
	buddy.setMobile(sections[4]);

	if (!sections[5].isEmpty())
	{
		foreach (const QString &group, sections[5].split(",", QString::SkipEmptyParts))
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
			Contact contact = Contact::create();
			contact.setContactAccount(account);
			GaduContactDetails *details = new GaduContactDetails(contact.data());
			details->setState(StorableObject::StateNew);
			contact.setDetails(details);
			contact.setId(QString::number(uin));
			contact.data()->setState(StorableObject::StateNew);
			contact.setOwnerBuddy(buddy);
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
