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

#include "hints-configuration.h"
#include "hints-configuration.moc"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"

#include <QtWidgets/QApplication>

HintsConfiguration::HintsConfiguration(QObject *parent) : QObject{parent}
{
}

HintsConfiguration::~HintsConfiguration()
{
}

void HintsConfiguration::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void HintsConfiguration::init()
{
    createDefaultConfiguration();
    configurationUpdated();
}

void HintsConfiguration::createDefaultConfiguration()
{
#if !defined(Q_OS_UNIX)
    m_configuration->deprecatedApi()->addVariable("Notify", "ConnectionError_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "NewChat_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "NewMessage_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToFreeForChat_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOnline_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToAway_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToNotAvailable_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToDoNotDisturb_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOffline_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/IncomingFile_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/Finished_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "multilogon_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "multilogon/sessionConnected_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "multilogon/sessionDisconnected_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_UseCustomSettings", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_Hints", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_UseCustomSettings", true);
    m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_Hints", true);
#endif

    m_configuration->deprecatedApi()->addVariable(
        "Hints", "IconSize", m_configuration->deprecatedApi()->readNumEntry("Hints", "AllEvents_iconSize", 32));
    m_configuration->deprecatedApi()->addVariable("Hints", "ScreenCorner", static_cast<int>(Corner::BottomRight));
    m_configuration->deprecatedApi()->addVariable(
        "Hints", "ShowAllNotificationActions",
        m_configuration->deprecatedApi()->readBoolEntry(
            "Hints", "ShowNotificationActions",
            !m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowOnlyNecessaryButtons", true)));
    m_configuration->deprecatedApi()->addVariable("Hints", "ShowContentMessage", true);
}

void HintsConfiguration::configurationUpdated()
{
    m_corner = static_cast<Corner>(
        m_configuration->deprecatedApi()->readNumEntry("Hints", "ScreenCorner", static_cast<int>(Corner::BottomRight)));
    if (m_corner < Corner::TopLeft || m_corner > Corner::BottomRight)
        m_corner = Corner::BottomRight;

    m_iconSize = m_configuration->deprecatedApi()->readNumEntry("Hints", "IconSize", 32);
    m_showAllNotificationActions =
        m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowAllNotificationActions", false);
    m_showContentMessage = m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowContentMessage", true);

    emit updated();
}

HintsConfiguration::Corner HintsConfiguration::corner() const
{
    return m_corner;
}

int HintsConfiguration::iconSize() const
{
    return m_iconSize;
}

bool HintsConfiguration::showAllNotificationActions() const
{
    return m_showAllNotificationActions;
}

bool HintsConfiguration::showContentMessage() const
{
    return m_showContentMessage;
}
