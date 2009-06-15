/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QVariant>

#include "configuration/storage-point.h"
#include "configuration/xml-configuration-file.h"
#include "misc/misc.h"

#include "contact-kadu-data.h"

ContactKaduData::ContactKaduData(StoragePoint *storage)
	: ModuleData(storage)
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
	HideDescription = QVariant(configurationStorage->getTextNode(parent, "HideDescription")).toBool();

}

void ContactKaduData::storeConfiguration() const
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	configurationStorage->createTextNode(parent, "ChatGeometry", rectToString(ChatGeometry));
	configurationStorage->createTextNode(parent, "HideDescription", QVariant(HideDescription).toString());
}
