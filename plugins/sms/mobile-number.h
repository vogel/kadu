/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MOBILE_NUMBER_H
#define MOBILE_NUMBER_H

#include "storage/uuid-storable-object.h"

class MobileNumber : public UuidStorableObject
{
	QString Number;
	QString GatewayId;

protected:
	virtual void load();

public:
	MobileNumber();
	MobileNumber(QString number, QString gatewayId);
	virtual ~MobileNumber();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName() { return QLatin1String("MobileNumber"); }

	virtual void store();

	const QString & number() const { return Number; };
	void setNumber(const QString &number) { Number = number; };

	const QString & gatewayId() const { return GatewayId; };
	void setGatewayId(const QString &gatewayId) { GatewayId = gatewayId; };

};

#endif // MOBILE_NUMBER_H
