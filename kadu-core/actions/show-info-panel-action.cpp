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

#include "show-info-panel-action.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "gui/actions/action.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"

ShowInfoPanelAction::ShowInfoPanelAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setIcon(KaduIcon{"kadu_icons/show-information-panel"});
	setName(QStringLiteral("showInfoPanelAction"));
	setText(tr("Show Information Panel"));
	setType(ActionDescription::TypeMainMenu);
}

ShowInfoPanelAction::~ShowInfoPanelAction()
{
}

void ShowInfoPanelAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ShowInfoPanelAction::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void ShowInfoPanelAction::init()
{
	registerAction(actionsRegistry());
}

void ShowInfoPanelAction::actionInstanceCreated(Action *action)
{
	action->setChecked(m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowInfoPanel"));
}

void ShowInfoPanelAction::actionTriggered(QAction *, bool toggled)
{
	m_kaduWindowService->kaduWindow()->infoPanel()->setVisible(toggled);
	m_configuration->deprecatedApi()->writeEntry("Look", "ShowInfoPanel", toggled);
}

void ShowInfoPanelAction::configurationUpdated()
{
	if (!m_kaduWindowService || !m_kaduWindowService->kaduWindow())
		return;

	ActionDescription::configurationUpdated();

	auto context = m_kaduWindowService->kaduWindow()->actionContext();
	if (action(context) &&action(context)->isChecked() != m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowInfoPanel"))
		action(context)->trigger();
}

#include "moc_show-info-panel-action.cpp"
