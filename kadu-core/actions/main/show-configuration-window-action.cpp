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

#include "show-configuration-window-action.h"

#include "windows/main-configuration-window-service.h"

ShowConfigurationWindowAction::ShowConfigurationWindowAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"preferences-other"});
	setName(QStringLiteral("configurationAction"));
	setShortcut("kadu_configure", Qt::ApplicationShortcut);
	setText(tr("Preferences"));
	setType(ActionDescription::TypeGlobal);
}

ShowConfigurationWindowAction::~ShowConfigurationWindowAction()
{
}

void ShowConfigurationWindowAction::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void ShowConfigurationWindowAction::actionTriggered(QAction *, bool)
{
	m_mainConfigurationWindowService->show();
}

#include "moc_show-configuration-window-action.cpp"
