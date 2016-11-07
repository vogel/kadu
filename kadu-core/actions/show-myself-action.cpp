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

#include "show-myself-action.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "gui/actions/action.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "talkable/model/talkable-model.h"
#include "talkable/model/talkable-proxy-model.h"

ShowMyselfAction::ShowMyselfAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setName(QStringLiteral("showMyselfAction"));
	setText(tr("Show Myself Buddy"));
	setType(ActionDescription::TypeMainMenu);
}

ShowMyselfAction::~ShowMyselfAction()
{
}

void ShowMyselfAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ShowMyselfAction::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void ShowMyselfAction::init()
{
	registerAction(actionsRegistry());
}

void ShowMyselfAction::actionInstanceCreated(Action *action)
{
	auto window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	auto enabled = m_configuration->deprecatedApi()->readBoolEntry("General", "ShowMyself", false);
	auto model = qobject_cast<TalkableModel *>(window->talkableProxyModel()->sourceModel());
	if (model)
	{
		model->setIncludeMyself(enabled);
		action->setChecked(enabled);
	}
}

void ShowMyselfAction::actionTriggered(QAction *action, bool toggled)
{
	auto window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	auto model = qobject_cast<TalkableModel *>(window->talkableProxyModel()->sourceModel());
	if (model)
	{
		model->setIncludeMyself(toggled);
		m_configuration->deprecatedApi()->writeEntry("General", "ShowMyself", toggled);
	}
}

void ShowMyselfAction::configurationUpdated()
{
	auto context = m_kaduWindowService->kaduWindow()->actionContext();
	if (action(context)->isChecked() != m_configuration->deprecatedApi()->readBoolEntry("General", "ShowMyself"))
		action(context)->trigger();
}

#include "moc_show-myself-action.cpp"
