/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QIntValidator>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"

#include "debug.h"

#include "gadu-contact-account-data.h"
#include "gadu-open-chat-with-runner.h"

GaduOpenChatWithRunner::GaduOpenChatWithRunner(Account *account) : ParentAccount(account)
{
}

ContactList GaduOpenChatWithRunner::matchingContacts(const QString &query)
{
	kdebugf();

	ContactList matchedContacts;
	if (!validateUserID(query))
		return matchedContacts;

	Contact c;

	GaduContactAccountData *gcad = new GaduContactAccountData(c, ParentAccount, query);
	c.addAccountData(gcad);
	c.setDisplay(ParentAccount->name() + ": " + query);
	matchedContacts.append(c);

	return matchedContacts;
}

bool GaduOpenChatWithRunner::validateUserID(const QString &uid)
{
	QIntValidator v(1, 99999999, 0);
	int pos = 0;
	QString text = uid;
	return v.validate(text, pos) == QValidator::Acceptable;
}
