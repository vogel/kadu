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

#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>

#include "profile-importer.h"

const QString ProfileImporter::UinQuery("/Kadu/Deprecated/ConfigFile[@name='kadu.conf']/Group[@name='General']/Entry[@name='UIN']/@value/string()");
const QString ProfileImporter::PasswordQuery("/Kadu/Deprecated/ConfigFile[@name='kadu.conf']/Group[@name='General']/Entry[@name='Password']/@value/string()");
const QString ProfileImporter::ContactsQuery("/Kadu/Contacts/Contact");

ProfileImporter::ProfileImporter(const QString &profileFileName) :
		ProfileFileName(profileFileName)
{
}

QString ProfileImporter::errorMessage()
{
	return ErrorMessage;
}

bool ProfileImporter::import()
{
	QFile profileFile(ProfileFileName);
	if (!profileFile.open(QIODevice::ReadOnly))
	{
		ErrorMessage = tr("Unable to open profile file [%1].").arg(ProfileFileName);
		return false;
	}

	QXmlQuery xmlQuery;
	xmlQuery.setFocus(&profileFile);

	xmlQuery.setQuery(UinQuery);
	QString uin;
	if (!xmlQuery.evaluateTo(&uin) || uin.isEmpty())
	{
		ErrorMessage = tr("Invalid profile. UIN data not found.");
		profileFile.close();
		return false;
	}

	xmlQuery.setQuery(PasswordQuery);
	QString password;
	if (!xmlQuery.evaluateTo(&password) || password.isEmpty())
	{
		ErrorMessage = tr("Invalid profile. Password data not found.");
		profileFile.close();
		return false;
	}

	xmlQuery.setQuery(ContactsQuery);
	QXmlResultItems contacts;
	xmlQuery.evaluateTo(&contacts);

	int count = 0;
	while (!contacts.next().isNull())
		count++;

	ErrorMessage = tr("Found data:\nUin: %1\n Password: %2\nContact count: %3").arg(uin).arg(password).arg(count);
	profileFile.close();
	return false;
}
