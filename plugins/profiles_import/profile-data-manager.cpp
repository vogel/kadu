/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QVector>

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "core/application.h"

#include "profile-data-manager.h"

QDomElement ProfileDataManager::getProfilesNode()
{
	QDomElement deprecated = Application::instance()->configuration()->api()->getNode("Deprecated", ConfigurationApi::ModeFind);
	if (deprecated.isNull())
		return QDomElement();

	QDomElement configFile;
	QVector<QDomElement> configFiles = Application::instance()->configuration()->api()->getNodes(deprecated, "ConfigFile");

	foreach (const QDomElement &cf, configFiles)
		if (cf.attribute("name").endsWith("kadu.conf"))
		{
			configFile = cf;
			break;
		}

	if (configFile.isNull())
		return QDomElement();

	return Application::instance()->configuration()->api()->getNamedNode(configFile, "Group", "Profiles");
}

QList<ProfileData> ProfileDataManager::readProfileData()
{
	QList<ProfileData> result;

	QDomElement groupProfiles = getProfilesNode();
	if (groupProfiles.isNull())
		return result;

	QVector<QDomElement> profiles = Application::instance()->configuration()->api()->getNodes(groupProfiles, "Profile");
	foreach (const QDomElement &profile, profiles)
	{
		if (profile.attribute("imported") != "yes")
		{
			ProfileData newProfile;
			newProfile.Name = profile.attribute("name");
			newProfile.Path = profile.attribute("directory");
			result.append(newProfile);
		}
	}

	return result;
}

void ProfileDataManager::markImported(const QString &name)
{
	QDomElement groupProfiles = getProfilesNode();
	if (groupProfiles.isNull())
		return;

	QDomElement profile = Application::instance()->configuration()->api()->getNamedNode(groupProfiles, "Profile", name);
	profile.setAttribute("imported", "yes");
}
