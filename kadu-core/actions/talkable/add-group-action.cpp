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

#include "add-group-action.h"

#include "actions/action-context.h"
#include "actions/action.h"
#include "actions/actions.h"
#include "buddies/group.h"
#include "core/injected-factory.h"
#include "windows/group-edit-window.h"

AddGroupAction::AddGroupAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"group-new"});
    setName(QStringLiteral("addGroupAction"));
    setText(tr("Add Group..."));
    setType(ActionDescription::TypeGlobal);
}

AddGroupAction::~AddGroupAction()
{
}

void AddGroupAction::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void AddGroupAction::actionTriggered(QAction *sender, bool)
{
    auto window = m_injectedFactory->makeInjected<GroupEditWindow>(Group::null, sender->parentWidget());
    window->show();
}

#include "moc_add-group-action.cpp"
