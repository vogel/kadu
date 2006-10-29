/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcursor.h>
#include <qobjectlist.h>
#include <qtooltip.h>

#include "action.h"
#include "debug.h"
#include "misc.h"
#include "toolbar.h"
#include "toolbutton.h"

Action::Action(const QIconSet& icon, const QString& text, const char* name, ActionType Type, QKeySequence accel)
	: QObject(NULL, name), Icon(icon), Text(text), Accel(accel), ToggleAction(false),
		OnIcon(), OnText(), Slot(0), ToolButtons(), ToggleState(false), Type(Type)
{
	kdebugf();
	kdebugf2();
}

void Action::toolButtonClicked()
{
	kdebugf();
	const ToolButton* button = dynamic_cast<const ToolButton*>(sender());
	const ToolBar* toolbar = dynamic_cast<ToolBar*>(button->parentWidget());
	const UserGroup* users = toolbar->selectedUsers();

	/**
		Don't check if users == NULL, now it is not neccesary.
		Either action dont need users field (with ActionType == TypeGlobal) or it cannot be activated when users == NULL
		(ActionType != TypeGlobal) - it is possible only when action's toolbar is floating - action's button is disabled.
	**/

	activated(users, button, button->isOn());
	kdebugf2();
}

void Action::toolButtonDestroyed(QObject* obj)
{
	kdebugf();
	ToolButton* btn = static_cast<ToolButton*>(obj);
	QValueList<ToolButton*>::iterator it = ToolButtons.find(btn);
	ToolButtons.remove(it);
	kdebugf2();
}

void Action::setToggleAction(bool toggle)
{
	kdebugf();
	ToggleAction = toggle;
	kdebugf2();
}

void Action::setOnShape(const QIconSet& icon, const QString& text)
{
	kdebugf();
	OnIcon = icon;
	OnText = text;
	kdebugf2();
}

ToolButton* Action::addToToolbar(ToolBar* toolbar, bool uses_text_label)
{
	kdebugf();

	ToolButton* btn = new ToolButton(toolbar, name(), Type);
	btn->setIconSet(Icon);
	btn->setTextLabel(Text);
	btn->setOnShape(OnIcon, OnText);
	btn->setToggleButton(ToggleAction);
	btn->setUsesTextLabel(uses_text_label);
	btn->setTextPosition(ToolButton::BesideIcon);
	btn->setAccel(Accel);
	if(ToggleAction || !OnIcon.isNull())
		btn->setOn(ToggleState);
	connect(btn, SIGNAL(clicked()), this, SLOT(toolButtonClicked()));
	connect(btn, SIGNAL(destroyed(QObject*)), this, SLOT(toolButtonDestroyed(QObject*)));
	if (Slot)
		connect(btn, SIGNAL(clicked()), toolbar->area()->parent(), Slot);

	ToolButtons.append(btn);
	const UserGroup* user_group = toolbar->selectedUsers();
	if (user_group != NULL)
		emit addedToToolbar(user_group, btn, toolbar);
	kdebugf2();
	return btn;
}

int Action::addToPopupMenu(QPopupMenu* menu, bool connect_signal)
{
	kdebugf();
	int id = menu->insertItem(Icon, Text);
	if (connect_signal)
		menu->connectItem(id, this, SIGNAL(activated()));
	kdebugf2();
	return id;
}

QValueList<ToolButton*> Action::toolButtonsForUserListElements(const UserListElements& users)
{
	kdebugf();
	QValueList<ToolButton*> buttons;
	CONST_FOREACH(i, ToolButtons)
	{
		const ToolBar* toolbar = dynamic_cast<ToolBar*>((*i)->parentWidget());
		if (toolbar->selectedUsers()->equals(users))
			buttons.append(*i);
	}
	kdebugf2();
	return buttons;
}

bool Action::isOn(const UserListElements& users)
{
//	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	if(!buttons.empty())
		return (*buttons.begin())->isOn();
	return false;
//	kdebugf2();
}

void Action::setOn(const UserListElements& users, bool on)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setOn(on);
	kdebugf2();
}

void Action::setAllOn(bool on)
{
	kdebugf();
	CONST_FOREACH(i, ToolButtons)
		(*i)->setOn(on);
	ToggleState = on;
	kdebugf2();
}

void Action::setPixmaps(const UserListElements& users, const QPixmap& pixmap)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setPixmap(pixmap);
	kdebugf2();
}

void Action::setTexts(const UserListElements& users, const QString& text)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setTextLabel(text);
	kdebugf2();
}

void Action::setEnabled(QWidget* parent, bool enabled)
{
	kdebugf();
	CONST_FOREACH(i, ToolButtons)
	{
		if (((ToolBar*)(*i)->parent())->area()->parent() == parent)
			(*i)->setEnabled(enabled);
	}
	kdebugf2();
}

void Action::setSlot(const char *slot)
{
	kdebugf();
	Slot = slot;
	kdebugf2();
}

void Action::activate(const UserGroup* users)
{
	kdebugf();
	emit activated(users, NULL, false);
	kdebugf2();
}

Action::ActionType Action::actionType()
{
	kdebugf();
	return Type;
}

Actions::Actions() : QMap<QString, Action *>(), DefaultToolbarActions()
{
}

void Actions::refreshIcons()
{
	//TODO: refresh icons
}

void Actions::addDefaultToolbarAction(
	const QString& toolbar, const QString& action, int index, bool uses_text_label)
{
	kdebugf();
	QValueList<Default>& actions = DefaultToolbarActions[toolbar];
	Default def(action, uses_text_label);
	if (index < 0)
		actions.push_back(def);
	else
		actions.insert(actions.at(index), def);
	kdebugf2();
}

void Actions::addDefaultActionsToToolbar(ToolBar* toolbar)
{
	kdebugf();
	if (DefaultToolbarActions.contains(toolbar->name()))
	{
		const QValueList<Default>& actions = DefaultToolbarActions[toolbar->name()];
		CONST_FOREACH(i, actions)
		{
			if (contains((*i).action_name))
				(*this)[(*i).action_name]->addToToolbar(toolbar,
					(*i).uses_text_label);
		}
	}
	kdebugf2();
}

Actions::Default::Default(QString action_name_, bool uses_text_label_) : action_name(action_name_), uses_text_label(uses_text_label_)
{
}

Actions::Default::Default() : action_name(), uses_text_label(false)
{
}

Actions KaduActions;
