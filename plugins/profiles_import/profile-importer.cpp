/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "roster/roster.h"

#include "plugins/gadu_protocol/helpers/gadu-importer.h"

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

	if (importedAccount.id().isEmpty())
	{
		ErrorMessage = tr("Imported account has no ID");
		profileFile.close();
		return false;
	}

	if (AccountManager::instance()->byId(importedAccount.protocolName(), importedAccount.id()))
	{
		ErrorMessage = tr("Account already exists.");
		profileFile.close();
		return false;
	}

	importedAccount.setAccountIdentity(identity);

	AccountManager::instance()->addItem(importedAccount);
	importedAccount.accountContact().setOwnerBuddy(Core::instance()->myself());

	QList<Buddy> buddies = GaduImporter::import065Buddies(importedAccount, xmlQuery);
	foreach (const Buddy &buddy, buddies)
	{
		// GaduImporter returns contacts already added to the ContactManager.

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

			foreach (const Contact &contact, buddy.contacts())
				Roster::instance()->addContact(contact);
		}
	}

	profileFile.close();

	ResultAccount = importedAccount;
	return true;
}

#include "moc_profile-importer.cpp"
