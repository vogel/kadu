/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddy-search-criteria.h"

BuddySearchCriteria::BuddySearchCriteria() :
	SearchContact(Buddy()), BirthYearTo(), Active(false), IgnoreResults(false)
{
}

BuddySearchCriteria::~BuddySearchCriteria()
{
}

void BuddySearchCriteria::reqUin(Account account, const QString &uin)
{
	ContactAccountData *cad = SearchContact.accountData(account);
	if (!cad)
	{
		cad = new ContactAccountData(account, SearchContact, uin);
		SearchContact.addAccountData(cad);
	}
	cad->setId(uin);
}

void BuddySearchCriteria::reqFirstName(const QString &firstName)
{
	SearchContact.setFirstName(firstName);
}

void BuddySearchCriteria::reqLastName(const QString &lastName)
{
	SearchContact.setLastName(lastName);
}

void BuddySearchCriteria::reqNickName(const QString &nickName)
{
	SearchContact.setNickName(nickName);
}

void BuddySearchCriteria::reqCity(const QString &city)
{
	SearchContact.setCity(city);
}

void BuddySearchCriteria::reqBirthYear(const QString &birthYearFrom, const QString &birthYearTo)
{
	BirthYearFrom = birthYearFrom;
	BirthYearTo = birthYearTo;
}

void BuddySearchCriteria::reqGender(bool female)
{
	SearchContact.setGender(female ? BuddyShared::GenderFemale : BuddyShared::GenderMale);
}

void BuddySearchCriteria::reqActive()
{
	Active = true;
}

void BuddySearchCriteria::clearData()
{
	SearchContact = Buddy();
	BirthYearFrom.truncate(0);
	BirthYearTo.truncate(0);
	Active = false;
	IgnoreResults = false;
}