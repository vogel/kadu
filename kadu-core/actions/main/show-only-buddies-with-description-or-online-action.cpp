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

#include "show-only-buddies-with-description-or-online-action.h"
#include "show-only-buddies-with-description-or-online-action.moc"

#include "actions/action.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "talkable/filter/hide-offline-without-description-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"

ShowOnlyBuddiesWithDescriptionOrOnlineAction::ShowOnlyBuddiesWithDescriptionOrOnlineAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setCheckable(true);
    setIcon(KaduIcon{"kadu_icons/only-show-online-and-with-description"});
    setName(QStringLiteral("onlineAndDescriptionUsersAction"));
    setText(tr("Only Show Online Buddies and Buddies with Description"));
    setType(ActionDescription::TypeUserList);
}

ShowOnlyBuddiesWithDescriptionOrOnlineAction::~ShowOnlyBuddiesWithDescriptionOrOnlineAction()
{
}

void ShowOnlyBuddiesWithDescriptionOrOnlineAction::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ShowOnlyBuddiesWithDescriptionOrOnlineAction::setKaduWindowService(KaduWindowService *kaduWindowService)
{
    m_kaduWindowService = kaduWindowService;
}

void ShowOnlyBuddiesWithDescriptionOrOnlineAction::actionInstanceCreated(Action *action)
{
    auto window = qobject_cast<MainWindow *>(action->parentWidget());
    if (!window)
        return;
    if (!window->talkableProxyModel())
        return;

    auto enabled = m_configuration->deprecatedApi()->readBoolEntry("General", "ShowOnlineAndDescription");
    auto filter = injectedFactory()->makeInjected<HideOfflineWithoutDescriptionTalkableFilter>(action);
    filter->setEnabled(enabled);

    action->setData(QVariant::fromValue(filter));
    action->setChecked(enabled);

    window->talkableProxyModel()->addFilter(filter);
}

void ShowOnlyBuddiesWithDescriptionOrOnlineAction::actionTriggered(QAction *action, bool toggled)
{
    auto v = action->data();
    if (v.canConvert<HideOfflineWithoutDescriptionTalkableFilter *>())
    {
        auto filter = v.value<HideOfflineWithoutDescriptionTalkableFilter *>();
        filter->setEnabled(toggled);
        m_configuration->deprecatedApi()->writeEntry("General", "ShowOnlineAndDescription", toggled);
    }
}

void ShowOnlyBuddiesWithDescriptionOrOnlineAction::configurationUpdated()
{
    if (!m_kaduWindowService || !m_kaduWindowService->kaduWindow())
        return;

    ActionDescription::configurationUpdated();

    auto context = m_kaduWindowService->kaduWindow()->actionContext();
    if (action(context) &&
        action(context)->isChecked() !=
            m_configuration->deprecatedApi()->readBoolEntry("General", "ShowOnlineAndDescription"))
        action(context)->trigger();
}
