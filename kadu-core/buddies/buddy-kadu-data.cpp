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
#include "misc/misc.h"
#include "storage/storage-point.h"

#include "buddy-kadu-data.h"

BuddyKaduData::BuddyKaduData(StorableObject *parent)
		: ModuleData(parent)
{
}

BuddyKaduData::~BuddyKaduData()
{
}

void BuddyKaduData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	ChatGeometry = stringToRect(loadValue<QString>("ChatGeometry"));
	HideDescription = loadValue<bool>("HideDescription");

}

void BuddyKaduData::store()
{
	if (!isValidStorage())
		return;

	storeValue("ChatGeometry", rectToString(ChatGeometry));
	storeValue("HideDescription", HideDescription);
}

QString BuddyKaduData::name() const
{
	return QLatin1String("kadu");
}
