/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"
#include "exports.h"
#include "status/status-container-aware-object.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class StatusChangerManager;
class StatusContainerManager;
class StatusTypeManager;
class Status;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StatusSetter
 * @short Class responsible for setting modified status to StatusContainers.
 * @see StatusChangerManager
 *
 * This singleton class is used to set status on status containers. Current implementation uses StatusChangerManager
 * to modify all statuses before sending them to status containers.
 *
 * StatusSetter also allows receiving originally statused (not modified) by manuallySetStatus method.
 */
class KADUAPI StatusSetter : public QObject, private StatusContainerAwareObject, private ConfigurationAwareObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit StatusSetter(QObject *parent = nullptr);
    virtual ~StatusSetter();

    /**
     * @short Sets status on given status container with modifications.
     * @param statusContainer StatusContainer to set status on.
     * @param status status to be modified and then set on statusContainer
     *
     * This methods stores given status as manually set status (to receive by manuallySetStatus) and then modifies
     * it using StatusChangersManager singleton. After all changes are done, new status is set on statusContainer
     * using StatusContainer::setStatus method.
     */
    void setStatusManually(StatusContainer *statusContainer, Status status);

    /**
     * @short Returns manually (unchanged) status from given StatusContainer.
     * @param statusContainer StatusContainer to get status from.
     *
     * This methods returns manually set status from given StatusContainer. This is the last status that was
     * set using StatusSetter::setStatus method.
     */
    Status manuallySetStatus(StatusContainer *statusContainer);

protected:
    void configurationUpdated();

    /**
     * @short Called on registration of new container, sets default status.
     * @param statusContainer new status container
     *
     * If core is initialized this method calls setDefaultStatus to set initial status on given status container.
     * If core is not initialized, this method does nothing. On core initialization this method will be called
     * for each status container.
     */
    void statusContainerRegistered(StatusContainer *statusContainer);
    void statusContainerUnregistered(StatusContainer *statusContainer);

private:
    friend class Core;

    QPointer<Configuration> m_configuration;
    QPointer<StatusChangerManager> m_statusChangerManager;
    QPointer<StatusContainerManager> m_statusContainerManager;
    QPointer<StatusTypeManager> m_statusTypeManager;

    QString StartupStatus;
    QString StartupDescription;
    bool StartupLastDescription;
    bool OfflineToInvisible;

    /**
     * @short Private method called by Core.
     *
     * This method is called by Core when all internals have been initialized. It allows StatusSetter
     * to set initial status on all registered status containers.
     */
    void coreInitialized();

    /**
     * @short Sets default status on given container.
     * @param statusContainer container to set default status on
     *
     * This method is internally called after new status container is registered. Stored status
     * is retreived from this container, then configuration is applied on this status (like:
     * default description), then this status is send to StatusChangerManager to get final version
     * to set on container.
     */
    void setDefaultStatus(StatusContainer *statusContainer);

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setStatusChangerManager(StatusChangerManager *statusChangerManager);
    INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
    INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
    INJEQT_INIT void init();
};

/**
 * @addtogroup Status
 * @}
 */
