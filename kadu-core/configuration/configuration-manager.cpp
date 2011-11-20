/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/toolbar-configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "storage/storable-object.h"

#include "configuration-manager.h"

ConfigurationManager * ConfigurationManager::Instance = 0;

ConfigurationManager * ConfigurationManager::instance()
{
	if (!Instance)
	{
		Instance = new ConfigurationManager();
		Instance->load();
	}

	return Instance;
}

ConfigurationManager::ConfigurationManager()
{
	ToolbarConfiguration = new ToolbarConfigurationManager();
}

ConfigurationManager::~ConfigurationManager()
{
	delete ToolbarConfiguration;
	ToolbarConfiguration = 0;
}

void ConfigurationManager::load()
{
	xml_config_file->makeBackup();

	importConfiguration();

	Uuid = xml_config_file->rootElement().attribute("uuid");
	if (Uuid.isNull())
		Uuid = QUuid::createUuid();
}

void ConfigurationManager::store()
{
	foreach (StorableObject *object, RegisteredStorableObjects)
		object->ensureStored();

	xml_config_file->rootElement().setAttribute("uuid", Uuid.toString());
}

void ConfigurationManager::flush()
{
	store();
	xml_config_file->sync();
}

void ConfigurationManager::registerStorableObject(StorableObject *object)
{
	RegisteredStorableObjects.append(object);
}

void ConfigurationManager::unregisterStorableObject(StorableObject *object)
{
	object->ensureStored();

	RegisteredStorableObjects.removeAll(object);
}

void ConfigurationManager::importConfiguration()
{
	QDomElement root = xml_config_file->rootElement();
	QDomElement general = xml_config_file->findElementByProperty(root.firstChild().firstChild().toElement(), "Group", "name", "General");
	QDomElement mainConfiguration = xml_config_file->findElementByProperty(general, "Entry", "name", "ConfigGeometry");

	if (!mainConfiguration.isNull())
		  mainConfiguration.setAttribute("name", "MainConfiguration_Geometry");
}
