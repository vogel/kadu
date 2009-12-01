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
#include "status/description-model.h"

#include "description-manager.h"

DescriptionManager * DescriptionManager::Instance = 0;

KADUAPI DescriptionManager * DescriptionManager::instance()
{
	if (0 == Instance)
		Instance = new DescriptionManager();
	return Instance;
}

DescriptionManager::DescriptionManager()
{
	ConfigurationManager::instance()->registerStorableObject(this);

	if (xml_config_file->getNode("Description", XmlConfigFile::ModeFind).isNull())
		import();
}

DescriptionManager::~DescriptionManager()
{
}

StorableObject * DescriptionManager::storageParent()
{
	return 0;
}

QString DescriptionManager::storageNodeName()
{
	return QLatin1String("Descriptions");
}

QString DescriptionManager::storageItemNodeName()
{
	return QLatin1String("Description");
}

DescriptionModel * DescriptionManager::model()
{
	ensureLoaded();

	return new DescriptionModel(this);
}

void DescriptionManager::import()
{
	clear();
	append(config_file.readEntry("General", "DefaultDescription").split("<-->"));
	removeDuplicates();

	store();
}

void DescriptionManager::addDescription(const QString &description)
{
	if (description.isEmpty())
		return;

	if (contains(description))
		removeDescription(description);

	emit descriptionAboutToBeAdded(description);
	prepend(description);
	emit descriptionAdded(description);
}

void DescriptionManager::removeDescription(const QString& description)
{
	if (!contains(description))
		return;

	emit descriptionAboutToBeRemoved(description);
	removeAll(description);
	emit descriptionRemoved(description);
}
