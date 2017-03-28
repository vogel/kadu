/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "storage/uuid-storable-object.h"

#include <QtCore/QPointer>

class MobileNumberManager;

class MobileNumber : public UuidStorableObject
{
    QPointer<MobileNumberManager> m_mobileNumberManager;

    QString m_number;
    QString m_gatewayId;

protected:
    virtual void load();
    virtual void store();

public:
    MobileNumber(MobileNumberManager *mobileNumberManager);
    MobileNumber(MobileNumberManager *mobileNumberManager, QString number, QString gatewayId);
    virtual ~MobileNumber();

    virtual StorableObject *storageParent();
    virtual QString storageNodeName()
    {
        return QStringLiteral("MobileNumber");
    }

    const QString &number() const
    {
        return m_number;
    };
    void setNumber(const QString &number)
    {
        m_number = number;
    };

    const QString &gatewayId() const
    {
        return m_gatewayId;
    };
    void setGatewayId(const QString &gatewayId)
    {
        m_gatewayId = gatewayId;
    };
};
