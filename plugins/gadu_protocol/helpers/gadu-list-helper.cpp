/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"

#include "protocols/protocol.h"

#include "misc/misc.h"
#include "debug.h"

#include "../gadu-contact-details.h"

#include "gadu-list-helper.h"

void GaduListHelper::setSupportedBuddyInformation(const Buddy &destination, const Buddy &source)
{
	destination.setFirstName(source.firstName());
	destination.setLastName(source.lastName());
	destination.setNickName(source.nickName());
	destination.setDisplay(source.display());
	destination.setMobile(source.mobile());
	destination.setGroups(source.groups());
	destination.setEmail(source.email());
	destination.setOfflineTo(source.isOfflineTo());
	destination.setHomePhone(source.homePhone());
}

QString GaduListHelper::contactToLine70(Contact contact)
{
	QStringList list;
	Buddy buddy = contact.ownerBuddy();

	list.append(buddy.firstName());
	list.append(buddy.lastName());
	list.append(buddy.nickName());
	list.append(buddy.display());
	list.append(buddy.mobile());

	QStringList groups;
	foreach (const Group &group, buddy.groups())
		groups.append(group.name());

	list.append(groups.join(","));
	list.append(contact.id());
	list.append(buddy.email());
	list.append(QString()); // alive sound
	list.append(QString()); // alive sound
	list.append(QString()); // message sound
	list.append(QString()); // message sound
	list.append(QString::number((int)buddy.isOfflineTo()));
	list.append(buddy.homePhone());

	return list.join(";");
}

QByteArray GaduListHelper::buddyListToByteArray(Account account, const BuddyList &buddies)
{
	kdebugf();

	QStringList result;
	result.append("GG70ExportString");

	foreach (const Buddy &buddy, buddies)
		foreach (const Contact &contact, buddy.contacts(account))
			result.append(contactToLine70(contact));

	return result.join("\n").toUtf8();
}

BuddyList GaduListHelper::byteArrayToBuddyList(Account account, QByteArray &content)
{
	QTextStream stream(&content, QIODevice::ReadOnly);

	return streamToBuddyList(account, stream);
}

BuddyList GaduListHelper::streamToBuddyList(Account account, QTextStream &content)
{
	BuddyList result;

	content.setCodec("UTF-8");

	QString line = content.readLine(70);

	if (line.startsWith(QLatin1String("<ContactBook>")))
		result = streamPost70ToBuddyList(line, account, content);
	else if (line.startsWith(QLatin1String("GG70ExportString")))
		result = stream70ToBuddyList(account, content);
	else
		result = streamPre70ToBuddyList(line, account, content);

	return result;
}

BuddyList GaduListHelper::streamPre70ToBuddyList(const QString &firstLine, Account account, QTextStream &content)
{
	BuddyList result;

	content.setCodec(codec_cp1250);

	if (firstLine.isEmpty())
		return result;

	QString line = firstLine;
	QStringList sections = line.split(';', QString::KeepEmptyParts);

	if (sections.count() > 6)
	{
		bool ok = false;
		sections[6].toULong(&ok);
		if (ok)
		{
			Buddy buddy = line70ToBuddy(account, sections);
			if (buddy)
				result.append(buddy);
			result.append(stream70ToBuddyList(account, content));
			return result;
		}
		else
		{
			Buddy buddy = linePre70ToBuddy(account, sections);
			if (buddy)
				result.append(buddy);
		}
	}

	while (!content.atEnd())
	{
		line = content.readLine();
		sections = line.split(';', QString::KeepEmptyParts);

		if (sections.count() < 7)
			continue;

		Buddy buddy = linePre70ToBuddy(account, sections);
		if (buddy)
			result.append(buddy);
	}

	return result;
}

BuddyList GaduListHelper::stream70ToBuddyList(Account account, QTextStream &content)
{
	BuddyList result;
	QString line;
	QStringList sections;

	while (!content.atEnd())
	{
		line = content.readLine();

		sections = line.split(';', QString::KeepEmptyParts);

		Buddy buddy = line70ToBuddy(account, sections);
		if (buddy)
			result.append(buddy);
	}

	return result;
}

BuddyList GaduListHelper::streamPost70ToBuddyList(const QString &line, Account account, QTextStream &content)
{
	BuddyList result;

	QString documentString = line + content.readAll();

	QDomDocument document;
	document.setContent(documentString);

	QDomElement docElement = document.documentElement();
	QMap<QString, Group> importedGroups;

	QDomNode groupsNode = docElement.firstChildElement("Groups");
	if (!groupsNode.isNull())
	{
		QDomElement groupElement = groupsNode.firstChildElement("Group");
		for (; !groupElement.isNull(); groupElement = groupElement.nextSiblingElement("Group"))
		{
			QDomElement idElement = groupElement.firstChildElement("Id");
			if (idElement.text().startsWith(QLatin1String("00000000-0000-0000-0000-")))
				continue;

			QDomElement nameElement = groupElement.firstChildElement("Name");
			if (nameElement.text().isEmpty())
				continue;

			importedGroups.insert(idElement.text(), GroupManager::instance()->byName(nameElement.text()));
		}
	}

	QDomNode contactsNode = docElement.firstChildElement("Contacts");
	if (!contactsNode.isNull())
	{
		QDomElement contactElement = contactsNode.firstChildElement("Contact");
		for (; !contactElement.isNull(); contactElement = contactElement.nextSiblingElement("Contact"))
		{
			Buddy buddy = Buddy::create();

			buddy.setFirstName(contactElement.firstChildElement("FirstName").text());
			buddy.setLastName(contactElement.firstChildElement("LastName").text());
			buddy.setDisplay(contactElement.firstChildElement("ShowName").text());
			buddy.setMobile(contactElement.firstChildElement("MobilePhone").text());
			buddy.setHomePhone(contactElement.firstChildElement("HomePhone").text());
			buddy.setEmail(contactElement.firstChildElement("Email").text());
			buddy.setCity(contactElement.firstChildElement("City").text());
			buddy.setWebsite(contactElement.firstChildElement("WwwAddress").text());
			buddy.setGender((BuddyGender)contactElement.firstChildElement("Gender").text().toInt());

			QList<Group> groups;
			QDomElement groupsElement = contactsNode.firstChildElement("Groups");
			QDomElement groupElement = groupsElement.firstChildElement("GroupId");
			for (; !groupElement.isNull(); groupElement = groupElement.nextSiblingElement("GroupId"))
				if (importedGroups.contains(groupElement.text()))
					groups.append(importedGroups.value(groupElement.text()));

			if (!groups.isEmpty())
				buddy.setGroups(groups);

			QDomElement numberElement = contactElement.firstChildElement("GGNumber");
			if (!numberElement.text().isEmpty() && numberElement.text() != account.id())
			{
				Contact contact = Contact::create();
				contact.setContactAccount(account);
				GaduContactDetails *details = dynamic_cast<GaduContactDetails *>(contact.details());
				details->setState(StorableObject::StateNew);
				contact.setId(numberElement.text());
				contact.data()->setState(StorableObject::StateNew);
				contact.setOwnerBuddy(buddy);
			}

			buddy.setAnonymous(false);
			result.append(buddy);
		}
	}

	return result;
}

Buddy GaduListHelper::linePre70ToBuddy(Account account, QStringList &sections)
{
	QList<Group> groups;
	unsigned int i, secCount;
	bool ok = false;

	secCount = sections.count();

	if (secCount < 5)
		return Buddy::null;

	Buddy buddy = Buddy::create();

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
		if (uin && QString::number(uin) != account.id())
		{
			Contact contact = Contact::create();
			contact.setContactAccount(account);
			contact.setId(QString::number(uin));
			GaduContactDetails *details = dynamic_cast<GaduContactDetails *>(contact.details());
			details->setState(StorableObject::StateNew);
			contact.data()->setState(StorableObject::StateNew);
			contact.setOwnerBuddy(buddy);
		}
	}

	if (i < secCount)
		buddy.setEmail(sections[i++]);

	if (i+1 < secCount)
	{
		i+=2;
	}
	if (i+1 < secCount)
	{
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

	secCount = sections.count();

	if (secCount < 6)
		return Buddy::null;

	Buddy buddy = Buddy::create();

	buddy.setFirstName(sections[0]);
	buddy.setLastName(sections[1]);
	buddy.setNickName(sections[2]);
	buddy.setDisplay(sections[3]);
	buddy.setMobile(sections[4]);

	if (!sections[5].isEmpty())
	{
		foreach (const QString &group, sections[5].split(',', QString::SkipEmptyParts))
			groups.append(GroupManager::instance()->byName(group));

		buddy.setGroups(groups);
	}

	i = 6;
	if (i < secCount)
	{
		UinType uin = sections[i++].toULong(&ok);
		if (!ok)
			uin = 0;
		if (uin && QString::number(uin) != account.id())
		{
			Contact contact = Contact::create();
			contact.setContactAccount(account);
			contact.setId(QString::number(uin));
			GaduContactDetails *details = dynamic_cast<GaduContactDetails *>(contact.details());
			details->setState(StorableObject::StateNew);
			contact.data()->setState(StorableObject::StateNew);
			contact.setOwnerBuddy(buddy);
		}
	}

	if (i < secCount)
		buddy.setEmail(sections[i++]);

	if (i+1 < secCount)
	{
		i+=2;
	}
	if (i+1 < secCount)
	{
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
