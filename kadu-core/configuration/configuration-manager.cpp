/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>

#include "configuration/toolbar-configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "storage/storable-object.h"

#include "configuration-manager.h"

ConfigurationManager * ConfigurationManager::Instance = 0;

ConfigurationManager * ConfigurationManager::instance()
{
	if (!Instance)
	{
		Instance = new ConfigurationManager(qApp);
		Instance->load();
	}

	return Instance;
}

ConfigurationManager::ConfigurationManager(QObject *parent) :
		QObject(parent)
{
	ToolbarConfiguration = new ToolbarConfigurationManager();

	connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)),
			this, SLOT(flush()));
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

void ConfigurationManager::flush()
{
	foreach (StorableObject *object, RegisteredStorableObjects)
		object->ensureStored();

	xml_config_file->rootElement().setAttribute("uuid", Uuid.toString());
	xml_config_file->sync();
}

void ConfigurationManager::registerStorableObject(StorableObject *object)
{
	if (RegisteredStorableObjects.contains(object))
	{
		qWarning("Someone tried to register already registered storable object.");
		return;
	}

	// Prepend so that store() method calls ensureStored() on objects in reverse order (LIFO).
	// This way if object A is registered and then object B which depends on A and can
	// change A's properties is registered, we first call ensureStored() on B, which can
	// safely change A's properties and they will be stored.
	RegisteredStorableObjects.prepend(object);
}

void ConfigurationManager::unregisterStorableObject(StorableObject *object)
{
	object->ensureStored();

	if (RegisteredStorableObjects.removeAll(object) <= 0)
		qWarning("Someone tried to unregister unregistered storable object.");
}

void ConfigurationManager::importConfiguration()
{
	QDomElement root = xml_config_file->rootElement();
	QDomElement general = xml_config_file->findElementByProperty(root.firstChild().firstChild().toElement(), "Group", "name", "General");
	QDomElement mainConfiguration = xml_config_file->findElementByProperty(general, "Entry", "name", "ConfigGeometry");

	if (!mainConfiguration.isNull())
		  mainConfiguration.setAttribute("name", "MainConfiguration_Geometry");
}
