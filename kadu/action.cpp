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
#include "kadu.h"

Action::Action(const QIconSet& icon, const QString& text, const char* name, QKeySequence accel)
	: QAction(icon, text, accel, kadu, name)
{
	kdebugf();
	UsesTextLabel = false;
	kdebugf2();
}

void Action::setUsesTextLabel(bool uses)
{
	kdebugf();
	UsesTextLabel = uses;
	kdebugf2();
}

void Action::toolButtonClicked()
{
	kdebugf();
	const ToolButton* button = dynamic_cast<const ToolButton*>(sender());
	ToolBar* toolbar = dynamic_cast<ToolBar*>(button->parentWidget());
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

void Action::setOnShape(const QIconSet& icon, const QString& text)
{
	OnIcon = icon;
	OnText = text;
}

ToolButton* Action::addToToolbar(ToolBar* toolbar)
{
	kdebugf();
	ToolButton* btn = new ToolButton(toolbar, name());
	btn->setIconSet(iconSet());
	btn->setTextLabel(menuText());
	btn->setOnShape(OnIcon, OnText);
	btn->setToggleButton(isToggleAction());
	btn->setUsesTextLabel(UsesTextLabel);
	btn->setTextPosition(ToolButton::BesideIcon);
	connect(btn, SIGNAL(clicked()), this, SLOT(toolButtonClicked()));
	connect(btn, SIGNAL(destroyed(QObject*)), this, SLOT(toolButtonDestroyed(QObject*)));
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
	int id = menu->insertItem(iconSet(), menuText());
	if (connect_signal)
		menu->connectItem(id, this, SIGNAL(activated()));
	kdebugf2();
	return id;
}

QValueList<ToolButton*> Action::toolButtonsForUserListElements(const UserListElements& users)
{
	kdebugf();
	QValueList<ToolButton*> buttons;
	for (QValueList<ToolButton*>::iterator i = ToolButtons.begin(); i != ToolButtons.end(); i++)
	{
		ToolBar* toolbar = dynamic_cast<ToolBar*>((*i)->parentWidget());
		if (toolbar->selectedUsers()->toUserListElements().equals(users))
			buttons.append(*i);
	}
	kdebugf2();
	return buttons;
}

bool Action::isOn(const UserListElements& users)
{
	kdebugf();
	for (QValueList<ToggleStateStruct>::iterator i = ToggleState.begin(); i != ToggleState.end(); i++)
		if ((*i).elems == users)
		{
			kdebug("state: %i\n", (*i).state);
			kdebugf2();
			return (*i).state;
		}
	return false;
	kdebugf2();
}

void Action::setOn(const UserListElements& users, bool on)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
		(*i)->setOn(on);
	for (QValueList<ToggleStateStruct>::iterator i = ToggleState.begin(); i != ToggleState.end(); i++)
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
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
		(*i)->setPixmap(pixmap);
	kdebugf2();
}

void Action::setTexts(const UserListElements& users, const QString& text)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
		(*i)->setTextLabel(text);
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

Actions KaduActions;
