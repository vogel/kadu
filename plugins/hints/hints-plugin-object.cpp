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

#include "hints-plugin-object.h"

#include "hint-manager.h"

#include "gui/windows/main-configuration-window-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "notification/notifier-repository.h"

HintsPluginObject::HintsPluginObject(QObject *parent) :
		QObject{parent}
{
}

HintsPluginObject::~HintsPluginObject()
{
}

void HintsPluginObject::setHintManager(HintManager *hintManager)
{
	m_hintManager = hintManager;
}

void HintsPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void HintsPluginObject::setNotifierRepository(NotifierRepository *notifierRepository)
{
	m_notifierRepository = notifierRepository;
}

void HintsPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void HintsPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/hints.ui"));
	m_notifierRepository->registerNotifier(m_hintManager);
}

void HintsPluginObject::done()
{
	m_notifierRepository->unregisterNotifier(m_hintManager);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/hints.ui"));
}

#include "moc_hints-plugin-object.cpp"
