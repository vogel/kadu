/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "mobile-number-manager.h"

#include "mobile-number.h"

MobileNumber::MobileNumber()
{
	setUuid(QUuid::createUuid());
}

MobileNumber::MobileNumber(QString number, QString gatewayId) :
		Number(number), GatewayId(gatewayId)
{
	setUuid(QUuid::createUuid());
}

MobileNumber::~MobileNumber()
{
}

void MobileNumber::load()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::load();

	setUuid(loadAttribute<QString>("uuid"));
	Number = loadValue<QString>("Number");
	GatewayId = loadValue<QString>("Gateway");
}

void MobileNumber::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	UuidStorableObject::store();

	storeAttribute("uuid", uuid().toString());
	storeValue("Number", Number);
	storeValue("Gateway", GatewayId);
}

StorableObject * MobileNumber::storageParent()
{
	return MobileNumberManager::instance();
}
