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

#include "show-blocked-buddies-action.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "actions/action.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "talkable/filter/blocked-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

ShowBlockedBuddiesAction::ShowBlockedBuddiesAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setIcon(KaduIcon{"kadu_icons/show-blocked-buddies"});
	setName(QStringLiteral("showIgnoredAction"));
	setText(tr("Show Blocked Buddies"));
	setType(ActionDescription::TypeMainMenu);
}

ShowBlockedBuddiesAction::~ShowBlockedBuddiesAction()
{
}

void ShowBlockedBuddiesAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ShowBlockedBuddiesAction::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void ShowBlockedBuddiesAction::actionInstanceCreated(Action *action)
{
	auto window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	auto enabled = m_configuration->deprecatedApi()->readBoolEntry("General", "ShowBlocked");
	auto blockedTalkableFilter = new BlockedTalkableFilter{action};
	blockedTalkableFilter->setEnabled(!enabled);

	action->setData(QVariant::fromValue(blockedTalkableFilter));
	action->setChecked(enabled);

	window->talkableProxyModel()->addFilter(blockedTalkableFilter);
}

void ShowBlockedBuddiesAction::actionTriggered(QAction *action, bool toggled)
{
	auto v = action->data();
	if (v.canConvert<BlockedTalkableFilter *>())
	{
		auto blockedTalkableFilter = v.value<BlockedTalkableFilter *>();
		blockedTalkableFilter->setEnabled(!toggled);
		m_configuration->deprecatedApi()->writeEntry("General", "ShowBlocked", toggled);
	}
}

void ShowBlockedBuddiesAction::configurationUpdated()
{
	if (!m_kaduWindowService || !m_kaduWindowService->kaduWindow())
		return;

	ActionDescription::configurationUpdated();

	auto context = m_kaduWindowService->kaduWindow()->actionContext();
	if (action(context) &&action(context)->isChecked() != m_configuration->deprecatedApi()->readBoolEntry("General", "ShowBlocked"))
		action(context)->trigger();
}

#include "moc_show-blocked-buddies-action.cpp"
