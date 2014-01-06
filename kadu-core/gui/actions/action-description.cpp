/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QMetaMethod>

#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/hot-key.h"
#include "gui/windows/main-window.h"

#include "action-description.h"

ActionDescription::ActionDescription(QObject *parent, ActionType type, const QString &name, QObject *object, const char *slot,
		const KaduIcon &icon, const QString &text, bool checkable, ActionBoolCallback enableCallback) :
		QObject(parent), Type(type), Name(name), Object(object), Slot(slot), Icon(icon), Text(text),
		Checkable(checkable), EnableCallback(enableCallback), ShortcutContext(Qt::WidgetShortcut)
{
	Deleting = false;

	registerAction();
}

ActionDescription::ActionDescription(QObject *parent) :
		QObject(parent), Type(TypeAll), Object(0), Slot(0),
		Checkable(false), EnableCallback(0), ShortcutContext(Qt::WidgetShortcut)
{
	Deleting = false;
}

ActionDescription::~ActionDescription()
{
	Deleting = true;

	qDeleteAll(MappedActions);
	MappedActions.clear();

	unregisterAction();
}

void ActionDescription::registerAction()
{
	Actions::instance()->insert(this);
}

void ActionDescription::unregisterAction()
{
	Actions::instance()->remove(this);
}

void ActionDescription::actionAboutToBeDestroyed(Action *action)
{
	if (Deleting)
		return;

	if (action && MappedActions.contains(action->context()))
		MappedActions.remove(action->context());
}

void ActionDescription::setType(ActionType type)
{
	Type = type;
}

void ActionDescription::setName(const QString &name)
{
	Name = name;
}

void ActionDescription::setConnection(QObject *object, const char *slot)
{
	Object = object;
	Slot = slot;
}

void ActionDescription::setIcon(const KaduIcon &icon)
{
	Icon = icon;
}

void ActionDescription::setText(const QString &text)
{
	Text = text;
}

void ActionDescription::setCheckable(bool checkable)
{
	Checkable = checkable;
}

void ActionDescription::setActionCallback(ActionBoolCallback enableCallback)
{
	EnableCallback = enableCallback;
}

void ActionDescription::setShortcut(QString configItem, Qt::ShortcutContext context)
{
	ShortcutItem = configItem;
	ShortcutContext = context;

	configurationUpdated();
}

void ActionDescription::actionTriggeredSlot(QAction *sender, bool toggled)
{
	actionTriggered(sender, toggled);

	Action *action = qobject_cast<Action *>(sender);
	if (action)
		triggered(sender->parentWidget(), action->context(), toggled);
}

QMenu * ActionDescription::menuForAction(Action *action)
{
	Q_UNUSED(action)

	return 0;
}

void ActionDescription::actionInstanceCreated(Action *action)
{
	QMenu *menu = menuForAction(action);
	if (menu)
		action->setMenu(menu);
}

void ActionDescription::updateActionState(Action *action)
{
	if (EnableCallback)
		(*EnableCallback)(action);
}

Action * ActionDescription::createAction(ActionContext *context, QObject *parent)
{
	Action *result = MappedActions.value(context);
	if (result)
	{
		if (result->parent() != parent)
			qWarning("ActionDescription::createAction(): requested action for already known context but with different parent\n");

		return result;
	}

	result = new Action(this, context, parent);
	MappedActions.insert(context, result);

	actionInstanceCreated(result);
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

	connect(result, SIGNAL(triggered(QAction *, bool)), this, SLOT(actionTriggeredSlot(QAction *, bool)));
	connect(result, SIGNAL(aboutToBeDestroyed(Action *)), this, SLOT(actionAboutToBeDestroyed(Action *)));
	if (Object && Slot)
		connect(result, SIGNAL(triggered(QAction *, bool)), Object, Slot);

	return result;
}

QList<Action *> ActionDescription::actions()
{
	return MappedActions.values();
}

Action * ActionDescription::action(ActionContext *context)
{
	if (MappedActions.contains(context))
		return MappedActions.value(context);
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
	else if (!MappedActions.isEmpty())
	{
		MappedActions.values().at(0)->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		MappedActions.values().at(0)->setShortcutContext(ShortcutContext);
	}
}

// TODO: a hack
#if QT_VERSION >= 0x050000
void ActionDescription::connectNotify(const QMetaMethod &signal)
#else
void ActionDescription::connectNotify(const char *signal)
#endif
{
	QObject::connectNotify(signal);

#if QT_VERSION >= 0x050000
	if (signal == QMetaMethod::fromSignal(&ActionDescription::actionCreated))
#else
	if (QLatin1String(signal) == SIGNAL(actionCreated(Action*))) // do not insert space here
#endif
		foreach (Action *action, MappedActions)
			emit actionCreated(action);
}

#include "moc_action-description.cpp"
