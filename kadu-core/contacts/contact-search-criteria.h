#ifndef CONTACT_SEARCH_CRITERIA_H
#define CONTACT_SEARCH_CRITERIA_H

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"

struct KADUAPI ContactSearchCriteria
{
	Contact SearchContact;
	QString BirthYearFrom;
	QString BirthYearTo;
	bool Active;
	bool IgnoreResults;

	ContactSearchCriteria();
	virtual ~ContactSearchCriteria();

	void reqUin(Account *account, const QString& uin);
	void reqFirstName(const QString& firstName);
	void reqLastName(const QString& lastName);
	void reqNickName(const QString& nickName);
	void reqCity(const QString& city);
	void reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo);
	void reqGender(bool female);
	void reqActive();

	void clearData();
};

#endif
