/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/windows/main-window.h"

#include "actions.h"

Actions::Actions()
{
}

void Actions::insert(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::insert(action->name(), action);
	emit actionLoaded(action->name());
}

void Actions::remove(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::remove(action->name());

	emit actionUnloaded(action->name());
}

QAction * Actions::createAction(const QString &name, MainWindow *kaduMainWindow)
{
	if (!contains(name))
		return 0;

	Action *result = (*this)[name]->createAction(kaduMainWindow);
	kaduMainWindow->actionAdded(result);

	emit actionCreated(result);

	return result;
}

Actions KaduActions;
