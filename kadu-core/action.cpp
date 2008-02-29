/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qglobal.h>

#include <qapplication.h>
#include <qcursor.h>
#include <qobject.h>
#include <qtooltip.h>
#include <q3accel.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>
#include <Q3PopupMenu>

#include "action.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "toolbar.h"
#include "toolbutton.h"

Action::Action(const QString& icon, const QString& text, const char* name, ActionType Type,
		QKeySequence Seq0, QKeySequence Seq1)
	: QObject(NULL, name), IconName(icon), Text(text), KeySeq0(Seq0), KeySeq1(Seq1),
		ToggleAction(false), OnIcon(), OnText(), Slot(0), ToolButtons(),
		ToggleState(false), Type(Type)
{
	kdebugf();
	KaduActions.insert(name, this);
	kdebugf2();
}

Action::~Action()
{
	kdebugf();
	KaduActions.remove(name());
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
	**/

	activated(users, button, button->isOn());
	kdebugf2();
}

void Action::toolButtonDestroyed(QObject* obj)
{
	kdebugf();
	ToolButton* btn = static_cast<ToolButton*>(obj);
	Q3ValueList<ToolButton*>::iterator it = ToolButtons.find(btn);
	ToolButtons.remove(it);
	kdebugf2();
}

void Action::setToggleAction(bool toggle)
{
	kdebugf();
	ToggleAction = toggle;
	kdebugf2();
}

void Action::setOnShape(const QString& icon, const QString& text)
{
	kdebugf();

	OnIcon = icon;
	OnText = text;

	kdebugf2();
}

void Action::buttonAddedToToolbar(ToolBar *toolBar, ToolButton *button)
{
	kdebugf();

	connect(button, SIGNAL(clicked()), this, SLOT(toolButtonClicked()));
	connect(button, SIGNAL(destroyed(QObject*)), this, SLOT(toolButtonDestroyed(QObject*)));

	// it should now be automatic
	// connect(this, SIGNAL(destroyed()), btn, SLOT(deleteLater()));

	if (Slot && toolBar->area())
		connect(button, SIGNAL(clicked()), toolBar->area()->parent(), Slot);

	ToolButtons.append(button);
	const UserGroup* user_group = toolBar->selectedUsers();
	if (user_group != NULL)
		emit addedToToolbar(user_group, button, toolBar);
	emit addedToToolbar(button, toolBar);

	kdebugf2();
}

int Action::addToPopupMenu(Q3PopupMenu* menu, bool connect_signal)
{
	kdebugf();
	int id = menu->insertItem(icons_manager->loadIcon(IconName), Text);
	if (connect_signal)
		menu->connectItem(id, this, SIGNAL(activated()));
	kdebugf2();
	return id;
}

Q3ValueList<ToolButton*> Action::toolButtonsForUserListElements(const UserListElements& users)
{
	kdebugf();
	Q3ValueList<ToolButton*> buttons;
	CONST_FOREACH(i, ToolButtons)
	{
		const ToolBar* toolbar = dynamic_cast<ToolBar*>((*i)->parentWidget());
		if (toolbar->selectedUsers() && toolbar->selectedUsers()->equals(users))
			buttons.append(*i);
	}
	kdebugf2();
	return buttons;
}

bool Action::isOn(const UserListElements& users)
{
//	kdebugf();
	Q3ValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	if(!buttons.empty())
		return (*buttons.begin())->isOn();
	return false;
//	kdebugf2();
}

void Action::setOn(const UserListElements& users, bool on)
{
	kdebugf();
	Q3ValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
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
	Q3ValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setPixmap(pixmap);
	kdebugf2();
}

void Action::setIcons(const UserListElements& users, const QIcon& icon)
{
	kdebugf();
	Q3ValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setIcon(icon);
	kdebugf();
}

void Action::refreshIcons()
{
	kdebugf();

	FOREACH(button, ToolButtons)
	{
		if (!IconName.isEmpty())
		{
			(*button)->setIcon(icons_manager->loadIcon(IconName));
			(*button)->setText(Text);
		}

		if (!OnIcon.isEmpty())
			(*button)->setOnShape(icons_manager->loadIcon(OnIcon), OnText);
	}

	emit iconsRefreshed();

	kdebugf2();
}

void Action::setTexts(const UserListElements& users, const QString& text)
{
	kdebugf();
	Q3ValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setText(text);
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

Actions::Actions()
{
}

void Actions::insert(const QString &name, Action *action)
{
	ActionsMap.insert(name, action);
	emit actionLoaded(name);
}

void Actions::remove(const QString &name)
{
	ActionsMap.remove(name);

	if (!Kadu::closing())
		emit actionUnloaded(name);
}

Action * Actions::operator [] (const QString &name)
{
	return ActionsMap[name];
}

Action * Actions::operator [] (int index)
{
	return ActionsMap[ActionsMap.keys()[index]];
}

bool Actions::contains(const QString &name)
{
	return ActionsMap.contains(name);
}

void Actions::refreshIcons()
{
	FOREACH(action, ActionsMap)
		(*action)->refreshIcons();
}

Actions KaduActions;
