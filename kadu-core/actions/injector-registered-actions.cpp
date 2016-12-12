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

#include "injector-registered-actions.h"

#include "actions/action-description.h"
#include "actions/actions.h"
#include "injeqt-type-roles.h"

#include <injeqt/injector.h>

InjectorRegisteredActions::InjectorRegisteredActions(Actions &actions, injeqt::injector &injector) :
		m_actions{actions},
		m_injector{injector}
{
	for (const auto &o : m_injector.get_all_with_type_role(ACTION))
	{
		auto action = qobject_cast<ActionDescription *>(o);
		if (action && m_actions.insert(action))
			m_registeredActions.push_back(action);
	}
}

InjectorRegisteredActions::~InjectorRegisteredActions()
{
	for (const auto &a : m_registeredActions)
		m_actions.remove(a);
}
