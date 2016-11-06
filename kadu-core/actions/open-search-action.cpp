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

#include "open-search-action.h"

#include "accounts/account-manager.h"
#include "core/injected-factory.h"
#include "gui/actions/actions.h"
#include "gui/actions/action.h"
#include "gui/windows/search-window.h"

OpenSearchAction::OpenSearchAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"edit-find"});
	setName(QStringLiteral("openSearchAction"));
	setText(tr("Search for Buddy..."));
	setType(ActionDescription::TypeGlobal);
}

OpenSearchAction::~OpenSearchAction()
{
}

void OpenSearchAction::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void OpenSearchAction::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void OpenSearchAction::init()
{
	registerAction(actionsRegistry());
}

void OpenSearchAction::actionInstanceCreated(Action *action)
{
	connect(m_accountManager, SIGNAL(accountRegistered(Account)), action, SLOT(checkState()));
	connect(m_accountManager, SIGNAL(accountUnregistered(Account)), action, SLOT(checkState()));
}

void OpenSearchAction::actionTriggered(QAction *sender, bool)
{
	auto window = m_injectedFactory->makeInjected<SearchWindow>(sender->parentWidget());
	window->show();
}

void OpenSearchAction::updateActionState(Action *action)
{
	auto hasSearchServiceAccount = std::any_of(std::begin(m_accountManager->items()), std::end(m_accountManager->items()), [](auto const &account) {
		return account.protocolHandler() && account.protocolHandler()->searchService();
	});
	action->setVisible(hasSearchServiceAccount);
}

#include "moc_open-search-action.cpp"
