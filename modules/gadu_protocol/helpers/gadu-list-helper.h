/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_LIST_HELPER_H
#define GADU_LIST_HELPER_H

class QString;
class QTextStream;

class Account;
class BuddyList;

class GaduListHelper
{

public:
	static QString buddyListToString(Account account, BuddyList buddies);
	static BuddyList stringToBuddyList(Account account, QString &content);
	static BuddyList streamToBuddyList(Account account, QTextStream &content);
	static Buddy linePre70ToBuddy(Account account, QStringList &sections);
	static Buddy line70ToBuddy(Account account, QStringList &sections);

};

#endif // GADU_LIST_HELPER_H
