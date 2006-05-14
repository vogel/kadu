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

Action::Action(const QIconSet& icon, const QString& text, const char* name, QKeySequence accel)
	: QObject(NULL, name), Slot(0)
{
	kdebugf();
	Icon = icon;
	Text = text;
	Accel = accel;
	ToggleAction = false;
	kdebugf2();
}

void Action::toolButtonClicked()
{
	kdebugf();
	const ToolButton* button = dynamic_cast<const ToolButton*>(sender());
	const ToolBar* toolbar = dynamic_cast<ToolBar*>(button->parentWidget());
	const UserGroup* users = toolbar->selectedUsers();
	if (users != NULL)
		emit activated(users, button, button->isOn());
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
	ToolButton* btn = new ToolButton(toolbar, name());
	btn->setIconSet(Icon);
	btn->setTextLabel(Text);
	btn->setOnShape(OnIcon, OnText);
	btn->setToggleButton(ToggleAction);
	btn->setUsesTextLabel(uses_text_label);
	btn->setTextPosition(ToolButton::BesideIcon);
	btn->setAccel(Accel);
	connect(btn, SIGNAL(clicked()), this, SLOT(toolButtonClicked()));
	connect(btn, SIGNAL(destroyed(QObject*)), this, SLOT(toolButtonDestroyed(QObject*)));
	if (Slot)
		connect(btn, SIGNAL(clicked()), toolbar->area()->parent(), Slot);
	ToolButtons.append(btn);
	const UserGroup* user_group = toolbar->selectedUsers();
	if (user_group != NULL)
	{
		UserListElements user_list_elems = user_group->toUserListElements();
		btn->setOn(isOn(user_list_elems));
		emit addedToToolbar(btn, toolbar, user_list_elems);
	}
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
	CONST_FOREACH(i, ToggleState)
		if ((*i).elems == users)
		{
			kdebugm(KDEBUG_INFO, "state: %i\n", (*i).state);
//			kdebugf2();
			return (*i).state;
		}
//	kdebugf2();
	return false;
}

void Action::setOn(const UserListElements& users, bool on)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setOn(on);
	for (QValueList<ToggleStateStruct>::iterator i = ToggleState.begin(), end = ToggleState.end(); i != end; ++i)
		if ((*i).elems == users)
		{
			(*i).state = on;
			return;
		}
	ToggleStateStruct s;
	s.elems = users;
	s.state = on;
	ToggleState.push_back(s);
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

void Action::setDockAreaGroupRestriction(const QString& dockarea_group)
{
	kdebugf();
	DockAreaGroupRestriction = dockarea_group;
	kdebugf2();
}

QString Action::dockAreaGroupRestriction()
{
	return DockAreaGroupRestriction;
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

Actions::Actions()
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
	Default def;
	def.action_name = action;
	def.uses_text_label = uses_text_label;
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

Actions KaduActions;
