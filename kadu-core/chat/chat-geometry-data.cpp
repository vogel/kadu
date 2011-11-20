/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "misc/misc.h"

#include "chat-geometry-data.h"

ChatGeometryData::ChatGeometryData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent)
		: ModuleData(moduleName, parent, qobjectParent)
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
}

void ChatGeometryData::store()
{
	if (!isValidStorage())
		return;

	if (WindowGeometry.isValid())
		storeValue("WindowGeometry", rectToString(WindowGeometry));
	else
		removeValue("WindowGeometry");

	if (!WidgetHorizontalSizes.isEmpty())
		storeValue("WidgetHorizontalSizes", intListToString(WidgetHorizontalSizes));
	else
		removeValue("WidgetHorizontalSizes");

	removeValue("WidgetVerticalSizes");
}

bool ChatGeometryData::shouldStore()
{
	ensureLoaded();

	return ModuleData::shouldStore() && (WindowGeometry.isValid() || !WidgetHorizontalSizes.isEmpty());
}

QString ChatGeometryData::name() const
{
	return QLatin1String("chat-geometry");
}
