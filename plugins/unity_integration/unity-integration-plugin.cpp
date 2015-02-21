/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "unity-integration-plugin.h"

#include "unity-integration.h"

#include "core/core.h"

UnityIntegrationPlugin::~UnityIntegrationPlugin()
{
}

bool UnityIntegrationPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	m_unityIntegration.reset(new UnityIntegration{});
	m_unityIntegration->setFileTransferManager(Core::instance()->fileTransferManager());
	m_unityIntegration->setUnreadMessageRepository(Core::instance()->unreadMessageRepository());

	return true;
}

void UnityIntegrationPlugin::done()
{
	m_unityIntegration.reset();
}

Q_EXPORT_PLUGIN2(indicator_docking, UnityIntegrationPlugin)

#include "moc_unity-integration-plugin.cpp"
