/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
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

bool ProfileImporter::import(const QString &name)
{
	QFile profileFile(ProfileFileName);
	if (!profileFile.open(QIODevice::ReadOnly))
	{
		ErrorMessage = tr("Unable to open profile file [%1].").arg(ProfileFileName);
		return false;
	}

	Account importedAccount = GaduImporter::import065Account(&profileFile);
	profileFile.close();

	if (importedAccount.id().isEmpty())
	{
		ErrorMessage = tr("Imported account has no ID");
		return false;
	}

	QString identity = name;
	if (identity.isEmpty())
		identity = tr("Imported: %1").arg(importedAccount.id());

	Identity accountIdentity = IdentityManager::instance()->byName(identity, true);
	importedAccount.setAccountIdentity(accountIdentity);

	AccountManager::instance()->addItem(importedAccount);
	return true;
}
