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

#include "add-user-action.h"

#include "buddies/buddy-set.h"
#include "core/injected-factory.h"
#include "actions/action-context.h"
#include "actions/action.h"
#include "windows/add-buddy-window.h"

AddUserAction::AddUserAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"contact-new"});
	setName(QStringLiteral("addUserAction"));
	setShortcut("kadu_adduser", Qt::ApplicationShortcut);
	setText(tr("Add Buddy..."));
	setType(ActionDescription::TypeGlobal);
}

AddUserAction::~AddUserAction()
{
}

void AddUserAction::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void AddUserAction::actionTriggered(QAction *sender, bool)
{
	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	auto const &buddy = action->context()->buddies().toBuddy();

	if (buddy.isAnonymous())
		(m_injectedFactory->makeInjected<AddBuddyWindow>(action->parentWidget(), buddy, true))->show();
	else
		(m_injectedFactory->makeInjected<AddBuddyWindow>(action->parentWidget()))->show();
}

#include "moc_add-user-action.cpp"
