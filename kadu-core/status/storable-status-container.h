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

#pragma once

#include "exports.h"
#include "status/status-container.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationManager;
class StatusTypeManager;
class StorableObject;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StorableStatusContainer
 * @author Rafał 'Vogel' Malinowski
 * @short Status container that can store and load status from storable object.
 * @see StorableObject
 * @see StatusSetter
 *
 * This class is status container that can store and load status from storable object.
 */
class KADUAPI StorableStatusContainer : public StatusContainer
{
    Q_OBJECT

public:
    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Creates new StorableStatusContainer instance.
     * @param storableObject storable object to store status in
     *
     * This constructor creates new StorableStatusContainer instance associated with given storableObject.
     * Each status loading and storing will be performed using this storable object.
     */
    explicit StorableStatusContainer(StorableObject *storableObject, QObject *parent = nullptr);
    virtual ~StorableStatusContainer();

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Return status stored in storable object.
     * @return stored status.
     *
     * Return status stored in storable object.
     */
    virtual Status loadStatus();

    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Stores given status in storable object.
     * @param status status to store
     *
     * Stores given status in storable object.
     */
    virtual void storeStatus(Status status);

protected:
    StatusTypeManager *statusTypeManager() const;

private:
    QPointer<ConfigurationManager> m_configurationManager;
    QPointer<StatusTypeManager> m_statusTypeManager;

    StorableObject *m_storableObject;

private slots:
    INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
    INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
};

/**
 * @}
 */
