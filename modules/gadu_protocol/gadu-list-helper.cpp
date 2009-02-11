/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QStringList>

#include "contacts/contact-account-data.h"
#include "contacts/contact-list.h"
#include "contacts/contact-manager.h"
#include "contacts/group.h"

#include "debug.h"

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
