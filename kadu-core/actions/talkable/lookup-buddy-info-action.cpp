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

#include "lookup-buddy-info-action.h"
#include "lookup-buddy-info-action.moc"

#include "actions/action-context.h"
#include "actions/action.h"
#include "buddies/buddy-set.h"
#include "core/injected-factory.h"
#include "protocols/protocol.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"
#include "windows/search-window.h"

LookupBuddyInfoAction::LookupBuddyInfoAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"edit-find"});
    setName(QStringLiteral("lookupUserInfoAction"));
    setText(tr("Search in Directory"));
    setType(ActionDescription::TypeUser);
}

LookupBuddyInfoAction::~LookupBuddyInfoAction()
{
}

void LookupBuddyInfoAction::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void LookupBuddyInfoAction::setKaduWindowService(KaduWindowService *kaduWindowService)
{
    m_kaduWindowService = kaduWindowService;
}

void LookupBuddyInfoAction::actionTriggered(QAction *sender, bool)
{
    auto action = qobject_cast<Action *>(sender);
    if (!action)
        return;

    auto const &buddy = action->context()->buddies().toBuddy();
    if (!buddy)
    {
        (m_injectedFactory->makeInjected<SearchWindow>(m_kaduWindowService->kaduWindow()))->show();
        return;
    }

    auto sd = m_injectedFactory->makeInjected<SearchWindow>(m_kaduWindowService->kaduWindow(), buddy);
    sd->show();
    sd->firstSearch();
}

void LookupBuddyInfoAction::updateActionState(Action *action)
{
    auto const &contact = action->context()->contacts().toContact();
    action->setEnabled(
        contact && contact.contactAccount().protocolHandler() &&
        contact.contactAccount().protocolHandler()->searchService());
}
