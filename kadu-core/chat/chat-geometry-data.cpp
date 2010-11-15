/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "misc/misc.h"

#include "chat-geometry-data.h"

ChatGeometryData::ChatGeometryData(StorableObject *parent)
		: ModuleData(parent)
{
}

ChatGeometryData::~ChatGeometryData()
{
}

void ChatGeometryData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	WindowGeometry = stringToRect(loadValue<QString>("WindowGeometry"));
	WidgetHorizontalSizes = stringToIntList(loadValue<QString>("WidgetHorizontalSizes"));
	WidgetVerticalSizes = stringToIntList(loadValue<QString>("WidgetVerticalSizes"));
}

void ChatGeometryData::store()
{
	if (!isValidStorage())
		return;

	storeValue("WindowGeometry", rectToString(WindowGeometry));
	storeValue("WidgetHorizontalSizes", intListToString(WidgetHorizontalSizes));
	storeValue("WidgetVerticalSizes", intListToString(WidgetVerticalSizes));
}

QString ChatGeometryData::name() const
{
	return QLatin1String("chat-geometry");
}
