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

#include "configuration/xml-configuration-file.h"

#include "profile-data-reader.h"

QList<ProfileData> ProfileDataReader::readProfileData()
{
	QList<ProfileData> result;

	QDomElement deprecated = xml_config_file->getNode("Deprecated", XmlConfigFile::ModeFind);
	if (deprecated.isNull())
		return result;

	QDomElement configFile;
	QList<QDomElement> configFiles = xml_config_file->getNodes(deprecated, "ConfigFile");

	foreach (const QDomElement &cf, configFiles)
		if (cf.attribute("name").endsWith("kadu.conf"))
		{
			configFile = cf;
			break;
		}

	if (configFile.isNull())
		return result;

	QDomElement groupProfiles = xml_config_file->getNamedNode(configFile, "Group", "Profiles");
	if (groupProfiles.isNull())
		return result;

	QList<QDomElement> profiles = xml_config_file->getNodes(groupProfiles, "Profile");
	foreach (const QDomElement &profile, profiles)
	{
		ProfileData newProfile;
		newProfile.Name = profile.attribute("name");
		newProfile.Path = profile.attribute("directory");
		result.append(newProfile);
	}

	return result;
}
