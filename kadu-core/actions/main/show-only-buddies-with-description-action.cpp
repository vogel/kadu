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

#include "show-only-buddies-with-description-action.h"
#include "show-only-buddies-with-description-action.moc"

#include "actions/action.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "talkable/filter/hide-without-description-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"

ShowOnlyBuddiesWithDescriptionAction::ShowOnlyBuddiesWithDescriptionAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setCheckable(true);
    setIcon(KaduIcon{"kadu_icons/only-show-with-description"});
    setName(QStringLiteral("descriptionUsersAction"));
    setShortcut("kadu_showonlydesc");
    setText(tr("Only Show Buddies with Description"));
    setType(ActionDescription::TypeUserList);
}

ShowOnlyBuddiesWithDescriptionAction::~ShowOnlyBuddiesWithDescriptionAction()
{
}

void ShowOnlyBuddiesWithDescriptionAction::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ShowOnlyBuddiesWithDescriptionAction::setKaduWindowService(KaduWindowService *kaduWindowService)
{
    m_kaduWindowService = kaduWindowService;
}

void ShowOnlyBuddiesWithDescriptionAction::actionInstanceCreated(Action *action)
{
    auto window = qobject_cast<MainWindow *>(action->parentWidget());
    if (!window)
        return;
    if (!window->talkableProxyModel())
        return;

    auto enabled = !m_configuration->deprecatedApi()->readBoolEntry("General", "ShowWithoutDescription");
    auto filter = injectedFactory()->makeInjected<HideWithoutDescriptionTalkableFilter>(action);
    filter->setEnabled(enabled);

    action->setData(QVariant::fromValue(filter));
    action->setChecked(enabled);

    window->talkableProxyModel()->addFilter(filter);
}

void ShowOnlyBuddiesWithDescriptionAction::actionTriggered(QAction *action, bool toggled)
{
    auto v = action->data();
    if (v.canConvert<HideWithoutDescriptionTalkableFilter *>())
    {
        auto filter = v.value<HideWithoutDescriptionTalkableFilter *>();
        filter->setEnabled(toggled);
        m_configuration->deprecatedApi()->writeEntry("General", "ShowWithoutDescription", !toggled);
    }
}

void ShowOnlyBuddiesWithDescriptionAction::configurationUpdated()
{
    if (!m_kaduWindowService || !m_kaduWindowService->kaduWindow())
        return;

    ActionDescription::configurationUpdated();

    auto context = m_kaduWindowService->kaduWindow()->actionContext();
    if (action(context) &&
        action(context)->isChecked() ==
            m_configuration->deprecatedApi()->readBoolEntry("General", "ShowWithoutDescription"))
        action(context)->trigger();
}
