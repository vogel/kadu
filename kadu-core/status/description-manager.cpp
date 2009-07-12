/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "core/core.h"

#include "description-manager.h"

DescriptionManager * DescriptionManager::Instance = 0;

KADUAPI DescriptionManager * DescriptionManager::instance()
{
	if (0 == Instance)
		Instance = new DescriptionManager();
	return Instance;
}

DescriptionManager::DescriptionManager() :
		StorableStringList("Descriptions", "Description", 0)
{
	Core::instance()->configuration()->registerStorableObject(this);

	if (xml_config_file->getNode("Description", XmlConfigFile::ModeFind).isNull())
		import();
}

DescriptionManager::~DescriptionManager()
{
}

void DescriptionManager::import()
{
	content().clear();
	content().append(config_file.readEntry("General", "DefaultDescription").split("<-->"));

	store();
}

void DescriptionManager::addDescription(const QString& description)
{
	if (content().contains(description))
		removeDescription(description);

	emit descriptionAboutToBeAdded(description);
	content().append(description);
	emit descriptionAdded(description);
}

void DescriptionManager::removeDescription(const QString& description)
{
	emit descriptionAboutToBeRemoved(description);
	content().removeAll(description);
	emit descriptionRemoved(description);
}
