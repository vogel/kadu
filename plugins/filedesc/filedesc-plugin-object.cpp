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

#include "filedesc-plugin-object.h"

#include "filedesc-status-changer.h"

#include "misc/paths-provider.h"
#include "status/status-changer-manager.h"
#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"

FiledescPluginObject::FiledescPluginObject(QObject *parent) :
		QObject{parent}
{
}

FiledescPluginObject::~FiledescPluginObject()
{
}

void FiledescPluginObject::setFileDescStatusChanger(FileDescStatusChanger *fileDescStatusChanger)
{
	m_fileDescStatusChanger = fileDescStatusChanger;
}

void FiledescPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void FiledescPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void FiledescPluginObject::setStatusChangerManager(StatusChangerManager *statusChangerManager)
{
	m_statusChangerManager = statusChangerManager;
}

void FiledescPluginObject::init()
{
	m_statusChangerManager->registerStatusChanger(m_fileDescStatusChanger);
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/filedesc.ui"));
}

void FiledescPluginObject::done()
{
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/filedesc.ui"));
	m_statusChangerManager->unregisterStatusChanger(m_fileDescStatusChanger);
}

#include "moc_filedesc-plugin-object.cpp"
