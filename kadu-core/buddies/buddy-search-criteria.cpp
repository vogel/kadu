/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddy-search-criteria.h"

ContactSearchCriteria::ContactSearchCriteria() :
	SearchContact(Contact()), BirthYearTo(), Active(false), IgnoreResults(false)
{
}

ContactSearchCriteria::~ContactSearchCriteria()
{
}

void ContactSearchCriteria::reqUin(Account account, const QString &uin)
{
	ContactAccountData *cad = SearchContact.accountData(account);
	if (!cad)
	{
		cad = new ContactAccountData(account, SearchContact, uin);
		SearchContact.addAccountData(cad);
	}
	cad->setId(uin);
}

void ContactSearchCriteria::reqFirstName(const QString &firstName)
{
	SearchContact.setFirstName(firstName);
}

void ContactSearchCriteria::reqLastName(const QString &lastName)
{
	SearchContact.setLastName(lastName);
}

void ContactSearchCriteria::reqNickName(const QString &nickName)
{
	SearchContact.setNickName(nickName);
}

void ContactSearchCriteria::reqCity(const QString &city)
{
	SearchContact.setCity(city);
}

void ContactSearchCriteria::reqBirthYear(const QString &birthYearFrom, const QString &birthYearTo)
{
	BirthYearFrom = birthYearFrom;
	BirthYearTo = birthYearTo;
}

void ContactSearchCriteria::reqGender(bool female)
{
	SearchContact.setGender(female ? ContactData::GenderFemale : ContactData::GenderMale);
}

void ContactSearchCriteria::reqActive()
{
	Active = true;
}

void ContactSearchCriteria::clearData()
{
	SearchContact = Contact();
	BirthYearFrom.truncate(0);
	BirthYearTo.truncate(0);
	Active = false;
	IgnoreResults = false;
}