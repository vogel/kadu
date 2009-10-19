/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NON_ACCOUNT_CONTACT_FILTER_H
#define NON_ACCOUNT_CONTACT_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-contact-filter.h"

class Account;

class NonAccountContactFilter : public AbstractContactFilter
{
	Q_OBJECT

	Account *CurrentAccount;
	bool Enabled;

public:
	explicit NonAccountContactFilter(QObject *parent = 0);
	virtual ~NonAccountContactFilter();

	void setAccount(Account *account);
	virtual bool acceptContact(Contact contact);

};

Q_DECLARE_METATYPE(NonAccountContactFilter *)

#endif // NON_ACCOUNT_CONTACT_FILTER_H
