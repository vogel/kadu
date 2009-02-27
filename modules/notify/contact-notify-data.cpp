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

#include "misc.h"
#include "xml_config_file.h"

#include "contact-notify-data.h"

ContactNotifyData::ContactNotifyData(StoragePoint *storage)
	: ContactModuleData(storage)
{

}

void ContactNotifyData::loadFromStorage()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	Notify = QVariant(configurationStorage->getTextNode(parent, "Notify")).toBool();
}

void ContactNotifyData::storeConfiguration() const
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	configurationStorage->createTextNode(parent, "Notify", QVariant(Notify).toString());
}
