/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/windows/main-window.h"
#include "hot_key.h"

#include "action-description.h"

ActionDescription::ActionDescription(QObject *parent, ActionType Type, const QString &Name, QObject *Object, const char *Slot,
	const QString &IconName, const QString &Text, bool Checkable, const QString &CheckedText, ActionBoolCallback enableCallback)
	: QObject(parent), ShortcutItem(""), ShortcutContext(Qt::WidgetShortcut)
{
	this->Type = Type;
	this->Name = Name;
	this->Object = Object;
	this->Slot = Slot;
	this->IconName = IconName;
	this->Text = Text;
	this->Checkable = Checkable;
	this->CheckedText = CheckedText;
	this->EnableCallback = enableCallback;
	this->deleted = 0;

	KaduActions.insert(this);
}

ActionDescription::~ActionDescription()
{
	deleted = 1;
	qDeleteAll(MappedActions);
	MappedActions.clear();
	KaduActions.remove(this);
}

void ActionDescription::actionDestroyed(QObject *action)
{
	Action *kaduAction = static_cast<Action *>(action);
	if (!kaduAction)
		return;

	MainWindow *kaduMainWindow = static_cast<MainWindow *>(action->parent());
	if (!kaduMainWindow)
		return;

	if (!deleted && MappedActions.contains(kaduMainWindow))
		MappedActions.remove(kaduMainWindow);
}

void ActionDescription::setShortcut(QString configItem, Qt::ShortcutContext context)
{
	ShortcutItem = configItem;
	ShortcutContext = context;

	configurationUpdated();
}

Action * ActionDescription::createAction(MainWindow *kaduMainWindow)
{
	if (MappedActions.contains(kaduMainWindow))
		return MappedActions[kaduMainWindow];

	Action *result = new Action(this, kaduMainWindow);
	MappedActions[kaduMainWindow] = result;

	connect(result, SIGNAL(destroyed(QObject *)), this, SLOT(actionDestroyed(QObject *)));
	connect(result, SIGNAL(triggered(QAction *, bool)), Object, Slot);

	emit actionCreated(result);

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		result->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		result->setShortcutContext(ShortcutContext);
	}
	else if (MappedActions.values().count() == 1)
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

Action * ActionDescription::action(MainWindow *kaduMainWindow)
{
	if (MappedActions.contains(kaduMainWindow))
		return MappedActions[kaduMainWindow];
	else
		return 0;
}

void ActionDescription::configurationUpdated()
{
	if (ShortcutItem.isEmpty())
		return;

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		foreach (Action *action, MappedActions.values())
		{
			action->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
			action->setShortcutContext(ShortcutContext);
		}
	}
	else if (MappedActions.values().count() > 0)
	{
		MappedActions.values()[0]->setShortcut(HotKey::shortCutFromFile("ShortCuts", ShortcutItem));
		MappedActions.values()[0]->setShortcutContext(ShortcutContext);
	}
}
