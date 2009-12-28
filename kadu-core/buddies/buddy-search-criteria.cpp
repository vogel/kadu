/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-shared.h"

#include "buddy-search-criteria.h"

BuddySearchCriteria::BuddySearchCriteria() :
		SearchBuddy(Buddy()), BirthYearTo(), Active(false), IgnoreResults(false)
{
}

BuddySearchCriteria::~BuddySearchCriteria()
{
}

void BuddySearchCriteria::reqUin(Account account, const QString &uin)
{
	// TODO ???
	QList<Contact> contactslist = SearchBuddy.contacts(account);
	Contact contact = contactslist.isEmpty() ? Contact::null : contactslist[0];
	if (contact.isNull())
	{
		contact = Contact::create;
		contact.setContactAccount(account);
		contact.setOwnerBuddy(SearchBuddy);
	}
	contact.setId(uin);
}

void BuddySearchCriteria::reqFirstName(const QString &firstName)
{
	SearchBuddy.setFirstName(firstName);
}

void BuddySearchCriteria::reqLastName(const QString &lastName)
{
	SearchBuddy.setLastName(lastName);
}

void BuddySearchCriteria::reqNickName(const QString &nickName)
{
	SearchBuddy.setNickName(nickName);
}

void BuddySearchCriteria::reqCity(const QString &city)
{
	SearchBuddy.setCity(city);
}

void BuddySearchCriteria::reqBirthYear(const QString &birthYearFrom, const QString &birthYearTo)
{
	BirthYearFrom = birthYearFrom;
	BirthYearTo = birthYearTo;
}

void BuddySearchCriteria::reqGender(bool female)
{
	SearchBuddy.setGender(female ? GenderFemale : GenderMale);
}

void BuddySearchCriteria::reqActive()
{
	Active = true;
}

void BuddySearchCriteria::clearData()
{
	SearchBuddy = Buddy();
	BirthYearFrom.truncate(0);
	BirthYearTo.truncate(0);
	Active = false;
	IgnoreResults = false;
}
