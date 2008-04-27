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

KaduAction::KaduAction(QObject *parent)
	: QAction(parent)
{
	connectSignalsAndSlots();
}

KaduAction::KaduAction(const QString &text, QObject *parent)
	: QAction(text, parent)
{
	connectSignalsAndSlots();
}

KaduAction::KaduAction(const QString &onText, const QString &offText, QObject *parent)
	: QAction(offText, parent), OnText(onText), OffText(offText)
{
	connectSignalsAndSlots();
}

KaduAction::KaduAction(const QIcon &icon, const QString &text, QObject *parent)
	: QAction(icon, text, parent)
{
	connectSignalsAndSlots();
}

KaduAction::KaduAction(const QIcon &onIcon, const QIcon offIcon, const QString &onText, const QString &offText, QObject *parent)
	: QAction(offIcon, offText, parent), OnText(onText), OffText(offText), OnIcon(onIcon), OffIcon(offIcon)
{
	connectSignalsAndSlots();
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

void KaduAction::connectSignalsAndSlots()
{
	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(hovered()), this, SLOT(hoveredSlot()));
	connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
	connect(this, SIGNAL(triggered(bool)), this, SLOT(triggeredSlot(bool)));
}


ActionDescription::ActionDescription(ActionType Type, const QString &Name, QObject *Object, char *Slot,
	const QString &IconName, const QString &Text, bool Checkable, const QString &CheckedText)
{
	this->Type = Type;
	this->Name = Name;
	this->Object = Object;
	this->Slot = Slot;
	this->IconName = IconName;
	this->Text = Text;
	this->Checkable = Checkable;
	this->CheckedText = CheckedText;

	KaduActions.insert(this);
}

ActionDescription::~ActionDescription()
{
	KaduActions.remove(this);
}

KaduAction * ActionDescription::getAction(KaduMainWindow *kaduMainWindow)
{
	KaduAction *result;

	if (Checkable)
	{
		if (!IconName.isEmpty())
			result = new KaduAction(icons_manager->loadIcon(IconName + "_off"), icons_manager->loadIcon(IconName), CheckedText, Text, kaduMainWindow);
		else
			result = new KaduAction(CheckedText, Text, kaduMainWindow);
	}
	else
	{
		if (!IconName.isEmpty())
			result = new KaduAction(icons_manager->loadIcon(IconName), Text, kaduMainWindow);
		else
			result = new KaduAction(Text, kaduMainWindow);
	}

	result->setCheckable(Checkable);

	connect(result, SIGNAL(toggled(QAction *, bool)), Object, Slot);
	connect(result, SIGNAL(triggered(QAction *, bool)), Object, Slot);

	return result;
}

Actions::Actions()
{
}

void Actions::insert(ActionDescription *action)
{
	ActionDescriptions.insert(action->name(), action);
	emit actionLoaded(action->name());
}

void Actions::remove(ActionDescription *action)
{
	ActionDescriptions.remove(action->name());

	if (!Kadu::closing())
		emit actionUnloaded(action->name());
}

ActionDescription * Actions::operator [] (const QString &name)
{
	return ActionDescriptions[name];
}

ActionDescription * Actions::operator [] (int index)
{
	return ActionDescriptions[ActionDescriptions.keys()[index]];
}

QAction * Actions::getAction(const QString &name, KaduMainWindow *kaduMainWindow) const
{
	if (!contains(name))
		return 0;

	KaduAction *result =  ActionDescriptions[name]->getAction(kaduMainWindow);
	kaduMainWindow->addAction(result);
	return result;
}

bool Actions::contains(const QString &name) const
{
	return ActionDescriptions.contains(name);
}

void Actions::refreshIcons()
{
// 	FOREACH(action, ActionsMap)
// 		(*action)->refreshIcons();
}

Actions KaduActions;
