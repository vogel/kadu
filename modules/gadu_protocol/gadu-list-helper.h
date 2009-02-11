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

class Account;
class ContactList;

class GaduListHelper
{

public:
	static QString contactListToString(Account *account, ContactList contacts);

};

#endif // GADU_LIST_HELPER_H
