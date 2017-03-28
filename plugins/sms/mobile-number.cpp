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

MobileNumber::MobileNumber(MobileNumberManager *mobileNumberManager) : m_mobileNumberManager{mobileNumberManager}
{
    setUuid(QUuid::createUuid());
}

MobileNumber::MobileNumber(MobileNumberManager *mobileNumberManager, QString number, QString gatewayId)
        : m_mobileNumberManager{mobileNumberManager}, m_number{number}, m_gatewayId{gatewayId}
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
    m_number = loadValue<QString>("Number");
    m_gatewayId = loadValue<QString>("Gateway");
}

void MobileNumber::store()
{
    if (!isValidStorage())
        return;

    ensureLoaded();

    UuidStorableObject::store();

    storeAttribute("uuid", uuid().toString());
    storeValue("Number", m_number);
    storeValue("Gateway", m_gatewayId);
}

StorableObject *MobileNumber::storageParent()
{
    return m_mobileNumberManager;
}
