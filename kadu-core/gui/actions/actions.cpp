/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/windows/main-window.h"

#include "actions.h"

Actions * Actions::Instance = 0;

Actions * Actions::instance()
{
	if (!Instance)
		Instance = new Actions();

	return Instance;
}

Actions::Actions() :
	BlockSignals(false)
{
}

void Actions::insert(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::insert(action->name(), action);

	if (!BlockSignals)
		emit actionLoaded(action->name());
}

void Actions::remove(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::remove(action->name());

	if (!Core::instance()->isClosing())
		emit actionUnloaded(action->name());
}

QAction * Actions::createAction(const QString &name, MainWindow *kaduMainWindow)
{
	if (!contains(name))
		return 0;

	Action *result = value(name)->createAction(kaduMainWindow, kaduMainWindow);
	kaduMainWindow->actionAdded(result);

	emit actionCreated(result);

	return result;
}
