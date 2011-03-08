/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXmlPatterns/QXmlQuery>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "identities/identity-manager.h"

#include "modules/gadu_protocol/helpers/gadu-importer.h"

#include "profile-importer.h"

ProfileImporter::ProfileImporter(const QString &profileFileName) :
		ProfileFileName(profileFileName)
{
}

QString ProfileImporter::errorMessage()
{
	return ErrorMessage;
}

bool ProfileImporter::import(const Identity &identity)
{
	QFile profileFile(ProfileFileName);
	if (!profileFile.open(QIODevice::ReadOnly))
	{
		ErrorMessage = tr("Unable to open profile file [%1].").arg(ProfileFileName);
		return false;
	}

	QXmlQuery xmlQuery;
	xmlQuery.setFocus(&profileFile);

	Account importedAccount = GaduImporter::import065Account(xmlQuery);

	Account existingAccount = AccountManager::instance()->byId(importedAccount.protocolName(), importedAccount.id());
	if (existingAccount)
	{
		ErrorMessage = tr("Account already exists.");
		profileFile.close();
		return false;
	}

	if (importedAccount.id().isEmpty())
	{
		ErrorMessage = tr("Imported account has no ID");
		profileFile.close();
		return false;
	}

	importedAccount.setAccountIdentity(identity);

	AccountManager::instance()->addItem(importedAccount);
	importedAccount.accountContact().setOwnerBuddy(Core::instance()->myself());

	QList<Buddy> buddies = GaduImporter::import065Buddies(importedAccount, xmlQuery);
	foreach (const Buddy &buddy, buddies)
	{
		foreach (const Contact &contact, buddy.contacts())
			ContactManager::instance()->addItem(contact);

		Buddy existingBuddy = BuddyManager::instance()->byDisplay(buddy.display(), ActionReturnNull);
		if (existingBuddy)
		{
			foreach (const Contact &contact, buddy.contacts())
				contact.setOwnerBuddy(existingBuddy);
		}
		else
		{
			buddy.setAnonymous(false);
			BuddyManager::instance()->addItem(buddy);
		}
	}

	profileFile.close();

	ResultAccount = importedAccount;
	return true;
}
