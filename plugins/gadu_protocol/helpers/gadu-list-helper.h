/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GADU_LIST_HELPER_H
#define GADU_LIST_HELPER_H

class QByteArray;
class QString;
class QTextStream;

class Account;
class Buddy;
class BuddyList;
class Contact;

namespace GaduListHelper
{
	void setSupportedBuddyInformation(const Buddy &destination, const Buddy &source);
	QByteArray buddyListToByteArray(Account account, const BuddyList &buddies);
	QByteArray contactListToByteArray(const QVector<Contact> &contacts);
	QString contactToLine70(Contact contact);
	BuddyList byteArrayToBuddyList(Account account, QByteArray &content);
	BuddyList streamToBuddyList(Account account, QTextStream &content);
	Buddy linePre70ToBuddy(Account account, QStringList &sections);
	Buddy line70ToBuddy(Account account, QStringList &sections);
	BuddyList streamPost70ToBuddyList(const QString &line, Account account, QTextStream &content);
	BuddyList stream70ToBuddyList(Account account, QTextStream &content);
	BuddyList streamPre70ToBuddyList(const QString &firstLine, Account account, QTextStream &content);
}

#endif // GADU_LIST_HELPER_H
