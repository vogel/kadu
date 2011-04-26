/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_LIST_HELPER_H
#define GADU_LIST_HELPER_H

class QByteArray;
class QString;
class QTextStream;

class Account;
class Buddy;
class BuddyList;
class Contact;

class GaduListHelper
{

public:
	static QByteArray buddyListToByteArray(Account account, const BuddyList &buddies);
	static QString contactToLine70(Contact contact);
	static BuddyList byteArrayToBuddyList(Account account, QByteArray &content);
	static BuddyList streamToBuddyList(Account account, QTextStream &content);
	static Buddy linePre70ToBuddy(Account account, QStringList &sections);
	static Buddy line70ToBuddy(Account account, QStringList &sections);
	static BuddyList streamPost70ToBuddyList(const QString &line, Account account, QTextStream &content);
	static BuddyList stream70ToBuddyList(Account account, QTextStream &content);
	static BuddyList streamPre70ToBuddyList(const QString &firstLine, Account account, QTextStream &content);
};

#endif // GADU_LIST_HELPER_H
