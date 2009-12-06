/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "misc/misc.h"

#include "contact-notify-data.h"

ContactNotifyData::ContactNotifyData(StorableObject *parent)
		: ModuleData(parent)
{
}

ContactNotifyData::~ContactNotifyData()
{
}

void ContactNotifyData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	Notify = loadValue<bool>("Notify", true);
}

void ContactNotifyData::store()
{
	if (!isValidStorage())
		return;

	storeValue("Notify", Notify);
}

QString ContactNotifyData::name() const
{
	return QLatin1String("notify");
}
