/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "accounts/account.h"
#include "contacts/contact-shared.h"

#include "buddy-search-criteria.h"

BuddySearchCriteria::BuddySearchCriteria() :
		SearchBuddy(Buddy::create()), BirthYearTo(), Active(false), IgnoreResults(false)
{
}

BuddySearchCriteria::~BuddySearchCriteria()
{
}

void BuddySearchCriteria::reqUin(Account account, const QString &uin)
{
	// TODO ???
	QList<Contact> contactslist = SearchBuddy.contacts(account);
	Contact contact = contactslist.isEmpty() ? Contact::null : contactslist.at(0);
	if (!contact)
	{
		contact = Contact::create();
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
	SearchBuddy = Buddy::create();
	BirthYearFrom.clear();
	BirthYearTo.clear();
	Active = false;
	IgnoreResults = false;
}
