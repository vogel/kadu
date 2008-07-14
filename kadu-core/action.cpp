/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "icons_manager.h"
#include "kadu.h"
#include "kadu_main_window.h"

#include "action.h"

KaduAction::KaduAction(ActionDescription *description, QObject *parent)
	: QAction(parent), Description(description)
{
	setText(Description->Text);

	if (Description->Checkable)
	{
		OnText = Description->CheckedText;
		OffText = Description->Text;
	}

	if (!Description->IconName.isEmpty())
	{
		if (Description->Checkable)
		{
			OnIcon = icons_manager->loadIcon(Description->IconName);
			OffIcon = icons_manager->loadIcon(Description->IconName + "_off");
			setIcon(OffIcon);
		}
		else
			setIcon(icons_manager->loadIcon(Description->IconName));
	}

	setCheckable(Description->Checkable);

	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(hovered()), this, SLOT(hoveredSlot()));
	connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
	connect(this, SIGNAL(triggered(bool)), this, SLOT(triggeredSlot(bool)));
}

KaduAction::~KaduAction()
{
}

void KaduAction::changedSlot()
{
	emit changed(this);
}

void KaduAction::hoveredSlot()
{
	emit hovered(this);
}

void KaduAction::toggledSlot(bool checked)
{
	if (checked)
	{
		if (!OnText.isEmpty())
			setText(OnText);
		if (!OnIcon.isNull())
			setIcon(OnIcon);
	}
	else
	{
		if (!OffText.isEmpty())
			setText(OffText);
		if (!OffIcon.isNull())
			setIcon(OffIcon);
	}

	emit toggled(this, checked);
}

void KaduAction::triggeredSlot(bool checked)
{
	emit triggered(this, checked);
}

void KaduAction::userListChanged(const UserListElements &ules)
{
	if (Description->EnableCallback)
		setEnabled((*Description->EnableCallback)(ules));
}

ActionDescription::ActionDescription(ActionType Type, const QString &Name, QObject *Object, char *Slot,
	const QString &IconName, const QString &Text, bool Checkable, const QString &CheckedText, ActionBoolCallback enableCallback)
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

	KaduActions.insert(this);
}

ActionDescription::~ActionDescription()
{
	KaduActions.remove(this);
}

void ActionDescription::actionDestroyed(QObject *action)
{
	KaduAction *kaduAction = dynamic_cast<KaduAction *>(action);
	if (!kaduAction)
		return;

	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(action->parent());
	if (!kaduMainWindow)
		return;

	MappedActions.remove(kaduMainWindow, kaduAction);
}

KaduAction * ActionDescription::getAction(KaduMainWindow *kaduMainWindow)
{
	KaduAction *result = new KaduAction(this, kaduMainWindow);
	MappedActions.insert(kaduMainWindow, result);

	connect(result, SIGNAL(destroyed(QObject *)), this, SLOT(actionDestroyed(QObject *)));
	connect(result, SIGNAL(toggled(QAction *, bool)), Object, Slot);
	connect(result, SIGNAL(triggered(QAction *, bool)), Object, Slot);

	return result;
}

QList<KaduAction *> ActionDescription::getActions()
{
	return MappedActions.values();
}

QList<KaduAction *> ActionDescription::getActions(KaduMainWindow *kaduMainWindow)
{
	if (MappedActions.contains(kaduMainWindow))
		return MappedActions.values(kaduMainWindow);
	else
		return QList<KaduAction *>();
}

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

	if (!Kadu::closing())
		emit actionUnloaded(action->name());
}

QAction * Actions::getAction(const QString &name, KaduMainWindow *kaduMainWindow) const
{
	if (!contains(name))
		return 0;

	KaduAction *result = (*this)[name]->getAction(kaduMainWindow);
	kaduMainWindow->addAction(result);
	return result;
}

void Actions::refreshIcons()
{
// 	FOREACH(action, ActionsMap)
// 		(*action)->refreshIcons();
}

bool disableEmptyUles(const UserListElements &ules)
{
	return !ules.isEmpty();
}

Actions KaduActions;
