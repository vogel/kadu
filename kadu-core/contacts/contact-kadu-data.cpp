/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/storage-point.h"

#include "misc.h"
#include "xml_config_file.h"

#include "contact-kadu-data.h"

ContactKaduData::ContactKaduData(StoragePoint *storage)
	: ContactModuleData(storage)
{
}

void ContactKaduData::loadFromStorage()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	ChatGeometry = stringToRect(configurationStorage->getTextNode(parent, "ChatGeometry"));

}

void ContactKaduData::storeConfiguration() const
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	configurationStorage->createTextNode(parent, "ChatGeometry", rectToString(ChatGeometry));
}
