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

#include "show-multilogons-action.h"

#include "accounts/account-manager.h"
#include "actions/action.h"
#include "windows/multilogon-window-service.h"

ShowMultilogonsAction::ShowMultilogonsAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"kadu_icons/multilogon"});
    setName(QStringLiteral("showMultilogonsAction"));
    setText(tr("Multilogons"));
    setType(ActionDescription::TypeMainMenu);
}

ShowMultilogonsAction::~ShowMultilogonsAction()
{
}

void ShowMultilogonsAction::setAccountManager(AccountManager *accountManager)
{
    m_accountManager = accountManager;
}

void ShowMultilogonsAction::setMultilogonWindowService(MultilogonWindowService *multilogonWindowService)
{
    m_multilogonWindowService = multilogonWindowService;
}

void ShowMultilogonsAction::actionInstanceCreated(Action *action)
{
    connect(m_accountManager, SIGNAL(accountLoadedStateChanged(Account)), action, SLOT(checkState()));
}

void ShowMultilogonsAction::actionTriggered(QAction *, bool)
{
    m_multilogonWindowService->show();
}

void ShowMultilogonsAction::updateActionState(Action *action)
{
    auto hasMultilogonAccount = std::any_of(
        std::begin(m_accountManager->items()), std::end(m_accountManager->items()), [](auto const &account) {
            return account.protocolHandler() && account.protocolHandler()->multilogonService();
        });

    action->setVisible(hasMultilogonAccount);
}

#include "moc_show-multilogons-action.cpp"
