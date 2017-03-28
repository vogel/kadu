/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <memory>

class Configuration;
class FullScreenServiceHandler;
class FullScreenService;
class MenuInventory;
class StatusContainerManager;
class ToggleSilentModeAction;

class QAction;

class KADUAPI SilentModeService : public QObject, ConfigurationAwareObject
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(SERVICE);

public:
    Q_INVOKABLE explicit SilentModeService(QObject *parent = nullptr);
    virtual ~SilentModeService();

    bool isSilentOrAutoSilent() const;
    bool isSilent() const;
    void setSilent(bool silent);

signals:
    void silentModeToggled(bool);

protected:
    virtual void configurationUpdated() override;

private:
    QPointer<Configuration> m_configuration;
    QPointer<FullScreenService> m_fullScreenService;
    QPointer<MenuInventory> m_menuInventory;
    QPointer<StatusContainerManager> m_statusContainerManager;
    QPointer<ToggleSilentModeAction> m_toggleSilentModeAction;

    std::unique_ptr<FullScreenServiceHandler> m_fullScreenServiceHandler;

    bool m_silentModeWhenDnD;
    bool m_silentModeWhenFullscreen;
    bool m_silentMode;

    void createActionDescriptions();
    void destroyActionDescriptions();
    void createDefaultConfiguration();

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setFullScreenService(FullScreenService *fullScreenService);
    INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
    INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
    INJEQT_SET void setToggleSilentModeAction(ToggleSilentModeAction *toggleSilentModeAction);
    INJEQT_INIT void init();
    INJEQT_DONE void done();
};
