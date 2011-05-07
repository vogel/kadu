/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QLatin1String>

#include "gui/hot-key.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/windows/main-window.h"

#include "action-description.h"

ActionDescription::ActionDescription(QObject *parent, ActionType type, const QString &name, QObject *object, const char *slot,
		const KaduIcon &icon, const QString &text, bool checkable, ActionBoolCallback enableCallback) :
		QObject(parent), Type(type), Name(name), Object(object), Slot(slot), Icon(icon), Text(text),
		Checkable(checkable), EnableCallback(enableCallback), ShortcutContext(Qt::WidgetShortcut)
{
	deleted = 0;

	Actions::instance()->insert(this);
}

ActionDescription::~ActionDescription()
{
	deleted = 1;
	qDeleteAll(MappedActions);
	MappedActions.clear();
	Actions::instance()->remove(this);
}

void ActionDescription::actionAboutToBeDestroyed(Action *action)
{
	if (deleted)
		return;

	if (action && MappedActions.contains(action->dataSource()))
		MappedActions.remove(action->dataSource());
}

void ActionDescription::setShortcut(QString configItem, Qt::ShortcutContext context)
{
	ShortcutItem = configItem;
	ShortcutContext = context;

	configurationUpdated();
}

Action * ActionDescription::createAction(ActionDataSource *dataSource, QObject *parent)
{
	if (MappedActions.contains(dataSource))
		return MappedActions.value(dataSource);

	Action *result = new Action(this, dataSource, parent);
	MappedActions.insert(dataSource, result);

	connect(result, SIGNAL(aboutToBeDestroyed(Action *)), this, SLOT(actionAboutToBeDestroyed(Action *)));
	if (Object && Slot)
		connect(result, SIGNAL(triggered(QAction *, bool)), Object, Slot);

	emit actionCreated(result);

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		result->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		result->setShortcutContext(ShortcutContext);
	}
	else if (MappedActions.count() == 1)
	{
		result->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		result->setShortcutContext(ShortcutContext);
	}
	return result;
}

QList<Action *> ActionDescription::actions()
{
	return MappedActions.values();
}

Action * ActionDescription::action(ActionDataSource *dataSource)
{
	if (MappedActions.contains(dataSource))
		return MappedActions.value(dataSource);
	else
		return 0;
}

void ActionDescription::configurationUpdated()
{
	if (ShortcutItem.isEmpty())
		return;

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		foreach (Action *action, MappedActions)
		{
			action->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
			action->setShortcutContext(ShortcutContext);
		}
	}
	else if (MappedActions.count() > 0)
	{
		MappedActions.values().at(0)->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		MappedActions.values().at(0)->setShortcutContext(ShortcutContext);
	}
}

// TODO: a hack
void ActionDescription::connectNotify(const char *signal)
{
	QObject::connectNotify(signal);

	if (QLatin1String(signal) == SIGNAL(actionCreated(Action*))) // do not insert space here
		foreach (Action *action, MappedActions)
			emit actionCreated(action);
}
