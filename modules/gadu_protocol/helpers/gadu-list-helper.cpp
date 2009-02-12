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

#include "contacts/contact-account-data.h"
#include "contacts/contact-list.h"
#include "contacts/contact-manager.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"

#include "protocols/protocol.h"

#include "debug.h"
#include "misc.h"

#include "../gadu-contact-account-data.h"

#include "gadu-list-helper.h"

QString GaduListHelper::contactListToString(Account *account, ContactList contacts)
{
	kdebugf();

	QStringList contactsStringList;

	foreach (Contact contact, contacts)
	{
		QStringList contactGroups;
		foreach (Group *group, contact.groups())
			contactGroups << group->name();

		ContactAccountData *cad = contact.accountData(account);

		contactsStringList << QString("%1;%2;%3;%4;%5;%6;%7;%8;%9;%10;%11;%12")
			.arg(contact.firstName())
			.arg(contact.lastName())
			.arg(contact.nickName())
			.arg(contact.mobile())
			.arg(contactGroups.join(";"))
			.arg(cad
				? cad->id()
				: "")
			// TODO: 0.6.6
			.arg("0")
			.arg("")
			.arg("0")
			.arg("")
			.arg(contact.isOfflineTo(account))
			.arg(contact.homePhone());
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

ContactList GaduListHelper::stringToContactList(Account* account, QString &content) {
	QTextStream stream(&content, QIODevice::ReadOnly);
	return streamToContactList(account, stream);
}

ContactList GaduListHelper::streamToContactList(Account *account, QTextStream &content)
{
	ContactList result;

	QStringList sections;
	QList<Group *> groups;
	QString line;
	unsigned int i, secCount;
	bool ok;

	content.setCodec(codec_latin2);

	while (!content.atEnd())
	{
		Contact contact;
		line = content.readLine();
//		kdebugm(KDEBUG_DUMP, ">>%s\n", qPrintable(line));
		sections = QStringList::split(";", line, true);
		secCount = sections.count();

		if (secCount < 7)
			continue;

		contact.setFirstName(sections[0]);
		contact.setLastName(sections[1]);
		contact.setNickName(sections[2]);
		contact.setDisplay(sections[3]);
		contact.setMobile(sections[4]);

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
		contact.setGroups(groups);
		--i;

		if (i < secCount)
		{
			UinType uin = sections[i++].toULong(&ok);
			if (!ok)
				uin = 0;
			if (uin)
				contact.addAccountData(new GaduContactAccountData(contact, account, QString::number(uin)));
		}

		if (i < secCount)
			contact.setEmail(sections[i++]);

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
			contact.setOfflineTo(account, bool(sections[i].toInt()));
			i++;
		}

		if (i < secCount)
			contact.setHomePhone(sections[i++]);

		result.append(contact);
	}

	return result;
}
