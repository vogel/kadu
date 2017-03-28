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

#include "silent-mode-service.h"

#include "actions/action-description.h"
#include "actions/action.h"
#include "actions/main/toggle-silent-mode-action.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "menu/menu-inventory.h"
#include "notification/full-screen-service-handler.h"
#include "notification/full-screen-service.h"
#include "status/status-container-manager.h"

SilentModeService::SilentModeService(QObject *parent)
        : QObject{parent}, m_silentModeWhenDnD{true}, m_silentModeWhenFullscreen{true}, m_silentMode{false}
{
}

SilentModeService::~SilentModeService()
{
}

void SilentModeService::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void SilentModeService::setFullScreenService(FullScreenService *fullScreenService)
{
    m_fullScreenService = fullScreenService;
}

void SilentModeService::setMenuInventory(MenuInventory *menuInventory)
{
    m_menuInventory = menuInventory;
}

void SilentModeService::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
    m_statusContainerManager = statusContainerManager;
}

void SilentModeService::setToggleSilentModeAction(ToggleSilentModeAction *toggleSilentModeAction)
{
    m_toggleSilentModeAction = toggleSilentModeAction;
}

void SilentModeService::init()
{
    createActionDescriptions();
    createDefaultConfiguration();
    configurationUpdated();
}

void SilentModeService::done()
{
    destroyActionDescriptions();
}

void SilentModeService::createActionDescriptions()
{
    m_menuInventory->menu("main")->addAction(m_toggleSilentModeAction, KaduMenu::SectionMiscTools, 5);
}

void SilentModeService::destroyActionDescriptions()
{
    m_menuInventory->menu("main")->removeAction(m_toggleSilentModeAction);
}

bool SilentModeService::isSilentOrAutoSilent() const
{
    if (isSilent())
        return true;
    if (m_silentModeWhenFullscreen && m_fullScreenService->hasFullscreenApplication())
        return true;
    if (m_silentModeWhenDnD && m_statusContainerManager->status().type() == StatusType::DoNotDisturb)
        return true;

    return false;
}

bool SilentModeService::isSilent() const
{
    return m_silentMode;
}

void SilentModeService::setSilent(bool silent)
{
    if (m_silentMode == silent)
        return;

    m_silentMode = silent;
    m_configuration->deprecatedApi()->writeEntry("Notify", "SilentMode", m_silentMode);

    for (auto action : m_toggleSilentModeAction->actions())
        action->setChecked(m_silentMode);

    emit silentModeToggled(isSilentOrAutoSilent());
}

void SilentModeService::createDefaultConfiguration()
{
    m_configuration->deprecatedApi()->addVariable("Notify", "AwaySilentMode", false);
    m_configuration->deprecatedApi()->addVariable("Notify", "FullscreenSilentMode", false);
    m_configuration->deprecatedApi()->addVariable("Notify", "SilentMode", false);
}

void SilentModeService::configurationUpdated()
{
    m_silentModeWhenDnD = m_configuration->deprecatedApi()->readBoolEntry("Notify", "AwaySilentMode", false);
    m_silentModeWhenFullscreen =
        m_configuration->deprecatedApi()->readBoolEntry("Notify", "FullscreenSilentMode", false);
    m_silentMode = m_configuration->deprecatedApi()->readBoolEntry("Notify", "SilentMode", false);

    if (m_silentModeWhenFullscreen)
        m_fullScreenServiceHandler = std::make_unique<FullScreenServiceHandler>(m_fullScreenService);
    else
        m_fullScreenServiceHandler.reset();
}
