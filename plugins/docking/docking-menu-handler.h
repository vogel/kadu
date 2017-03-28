/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking-exports.h"

#include "injeqt-type-roles.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class DockingMenuActionRepository;
class IconsManager;
class PluginInjectedFactory;
class KaduWindowService;
class SilentModeService;
class StatusContainer;
class StatusContainerManager;
class StatusNotifierItem;

class QAction;
class QMenu;

class DOCKINGAPI DockingMenuHandler final : public QObject
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(SERVICE)

public:
    Q_INVOKABLE explicit DockingMenuHandler(QObject *parent = nullptr);
    virtual ~DockingMenuHandler();

private:
    QPointer<Configuration> m_configuration;
    QPointer<DockingMenuActionRepository> m_dockingMenuActionRepository;
    QPointer<IconsManager> m_iconsManager;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<KaduWindowService> m_kaduWindowService;
    QPointer<SilentModeService> m_silentModeService;
    QPointer<StatusContainerManager> m_statusContainerManager;

    QPointer<QMenu> m_menu;
#if defined(Q_OS_UNIX)
    QAction *m_showKaduAction;
    QAction *m_hideKaduAction;
#endif
    QAction *m_silentModeAction;
    QAction *m_closeKaduAction;

    bool m_needsUpdate;
    bool m_mainWindowLastVisible;

    void doUpdate();
    void addStatusContainerMenus();
    void addStatusContainerMenu(StatusContainer *statusContainer);
    void addActionRepositoryMenus();

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setDockingMenuActionRepository(DockingMenuActionRepository *dockingMenuActionRepository);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
    INJEQT_SET void setSilentModeService(SilentModeService *silentModeService);
    INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
    INJEQT_SET void setStatusNotifierItem(StatusNotifierItem *statusNotifierItem);
    INJEQT_INIT void init();

    void aboutToShow();
    void update();

    void statusContainerRegistered(StatusContainer *statusContainer);
    void statusContainerUnregistered(StatusContainer *statusContainer);

    void showKaduWindow();
    void hideKaduWindow();
    void silentModeToggled(bool enabled);
};
