/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "config_file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "misc.h"

#include "gadu_account_data.h"
#include "gadu-contact-account-data.h"

#include "gadu-importer.h"

GaduImporter * GaduImporter::Instance;

GaduImporter * GaduImporter::instance()
{
	if (0 == Instance)
		Instance = new GaduImporter();

	return Instance;
}

void GaduImporter::importAccounts()
{
	GaduAccountData *gaduAccountData = new GaduAccountData(
			config_file.readNumEntry("General", "UIN"),
			unicode2cp(pwHash(config_file.readEntry("General", "Password"))));

	Account *defaultGaduGadu = AccountManager::instance()->createAccount(
			"Gadu-Gadu", "gadu", gaduAccountData);
	AccountManager::instance()->registerAccount(defaultGaduGadu);
}

void GaduImporter::importContacts()
{
	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact &)),
			this, SLOT(contactAdded(Contact &)));

	foreach (Contact contact, ContactManager::instance()->contacts())
		contactAdded(contact);
}

void GaduImporter::importContact(Contact &contact)
{
	QString id = contact.customData()["uin"];

	Account *account = AccountManager::instance()->defaultAccount();

	contact.customData().remove("uin");
	contact.addAccountData(new GaduContactAccountData(account, id));
}

void GaduImporter::contactAdded(Contact &contact)
{
	if (contact.customData().contains("uin"))
		importContact(contact);
}
