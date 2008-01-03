/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qaccel.h>
#include <qcursor.h>
#include <qobjectlist.h>

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "toolbar.h"
#include "kadu.h"

QMap< QString, QValueList<ToolBar::ToolBarAction> > ToolBar::DefaultActions;

ToolBar::ToolBar(QWidget* /*parent*/, const char *name)
	: QToolBar(NULL, name), dragButton(NULL)
{
	kdebugf();

	setAcceptDrops(true);
	setMovingEnabled(!DockArea::blocked());

	connect(&KaduActions, SIGNAL(actionLoaded(const QString &)), this, SLOT(actionLoaded(const QString &)));
	connect(&KaduActions, SIGNAL(actionUnloaded(const QString &)), this, SLOT(actionUnloaded(const QString &)));

	kdebugf2();
}


ToolBar::~ToolBar()
{
	kdebugf();

	disconnect(&KaduActions, SIGNAL(actionLoaded(const QString &)), this, SLOT(actionLoaded(const QString &)));
	disconnect(&KaduActions, SIGNAL(actionUnloaded(const QString &)), this, SLOT(actionUnloaded(const QString &)));

	kdebugf2();
}

void ToolBar::addAction(const QString &actionName, bool showLabel, ToolButton *button, bool before)
{
	if (hasAction(actionName))
		return;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.button = 0;
	newAction.showLabel = showLabel;

	bool inserted = false;
	if (button)
	{
		FOREACH(i, ToolBarActions)
			if ((*i).button == button)
			{
				if (!before)
					i++;

				inserted = true;
				ToolBarActions.insert(i, newAction);
				break;
			}
	}

	if (!inserted)
		ToolBarActions.append(newAction);
}

void ToolBar::removeAction(const QString &actionName)
{
	FOREACH(actionIterator, ToolBarActions)
		if ((*actionIterator).actionName == actionName)
		{
			delete (*actionIterator).button;
			ToolBarActions.remove(actionIterator);
			return;
		}
}

void ToolBar::usersChanged()
{
	FOREACH(actionIterator, ToolBarActions)
		if ((*actionIterator).button)
			(*actionIterator).button->usersChanged();
}

// TODO: optimize
void ToolBar::moveAction(const QString &actionName, ToolButton *button)
{
	bool actionFirst;
	bool showLabel;

	FOREACH(actionIterator, ToolBarActions)
		if ((*actionIterator).actionName == actionName)
		{
			if ((*actionIterator).button == button)
				return;
			actionFirst = true;
			break;
		}
		else if ((*actionIterator).button == button)
		{
			if ((*actionIterator).actionName == actionName)
				return;
			actionFirst = false;
			break;
		}

	FOREACH(actionIterator, ToolBarActions)
		if ((*actionIterator).actionName == actionName)
		{
			showLabel = (*actionIterator).showLabel;
			delete (*actionIterator).button;
			ToolBarActions.remove(actionIterator);
			break;
		}

	addAction(actionName, showLabel, button, !actionFirst);
	updateButtons();
}

ToolButton * ToolBar::addButton(Action *action, bool showLabel, ToolButton *after)
{
	kdebugf();

	ToolButton* button = new ToolButton(this, action->name(), action->type());

	// need, becouse without it positioning just doesn't work
	button->show();

	connect(button, SIGNAL(removedFromToolbar(ToolButton *)), this, SLOT(removeButtonClicked(ToolButton *)));

	QBoxLayout* layout = boxLayout();

	if (after)
	{
		layout->remove(button);
		layout->insertWidget(layout->findWidget(after) + 1, button);
	}
	else
	{
		layout->remove(button);
		layout->insertWidget(0, button);
	}

	button->setIconSet(icons_manager->loadIconSet(action->iconName()));

	QString textWithoutAccel = action->text();
	textWithoutAccel.remove('&');
	button->setTextLabel(textWithoutAccel);

	button->setOnShape(icons_manager->loadIconSet(action->onIcon()), action->onText());
	button->setToggleButton(action->toggleAction());
	button->setUsesTextLabel(showLabel);
	button->setTextPosition(ToolButton::BesideIcon);

	QAccel* accel = new QAccel(button);
	accel->connectItem(accel->insertItem(action->keySeq0()), button, SIGNAL(clicked()));
	accel->connectItem(accel->insertItem(action->keySeq1()), button, SIGNAL(clicked()));

	if (action->toggleAction() || !action->onIcon().isEmpty())
		button->setOn(action->toggleState());

	action->buttonAddedToToolbar(this, button);

	return button;
}

void ToolBar::removeButtonClicked(ToolButton *button)
{
	FOREACH(actionIterator, ToolBarActions)
		if ((*actionIterator).button == button)
		{
			ToolBarActions.remove(actionIterator);
			return;
		}
}

void ToolBar::addButtonClicked(int action_index)
{
	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "action_index = %d\n", action_index);

	addAction(KaduActions[action_index]->name(), false);
	updateButtons();

	kdebugf2();
}

void ToolBar::moveEvent(QMoveEvent *e)
{
	if (offset() != 10000)
	{
		QWidget *parent = parentWidget();
		if (parent && e->pos().x() != 0 && parent->width() == e->pos().x() + width() + 1)
			setOffset(10000);
	}

	QToolBar::moveEvent(e);
}

void ToolBar::dragEnterEvent(QDragEnterEvent* event)
{
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());
	if (source)
	{
		QString actionName;
		bool showLabel;

		if (ActionDrag::decode(event, actionName, showLabel))
			event->accept((event->source() == this) || (!hasAction(actionName) && dockArea()->supportsAction(KaduActions[actionName]->actionType())));
		else
			event->accept(false);
	}
	else
		event->accept(false);
	kdebugf2();
}

void ToolBar::dropEvent(QDropEvent* event)
{
	// TODO: update ToolbarActions

	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());

	if (!source)
	{
		event->accept(false);
		return;
	}


	QString actionName;
	bool showLabel;
	if (!ActionDrag::decode(event, actionName, showLabel))
	{
		event->accept(false);
		return;
	}

	ToolBar* source_toolbar = (ToolBar*)event->source();
	ToolButton *child = dynamic_cast<ToolButton *>(childAt(event->pos()));

	if (source_toolbar != this)
	{
		source_toolbar->removeAction(actionName);

		if (child)
			addAction(actionName, showLabel, child);
		else
			addAction(actionName, showLabel);
	}
	else
		moveAction(actionName, child);

	updateButtons();

	event->accept(true);

	kdebugf2();
}

void ToolBar::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();

	if (DockArea::blocked())
	{
		e->ignore();
		return;
	}

	//NOTE: parent MUST be dockArea(), NOT this, because when user is choosing "remove toolbar",
	//      it calls deleteLater(), which is invoked _before_ exec returns! so QPopupMenu would
	//      be deleted when exec returns!
	QPopupMenu* p = createContextMenu(dockArea());
	showPopupMenu(p);
	delete p;
	e->accept();

	kdebugf2();
}

void ToolBar::show()
{
	 // very lame, but i don't have better idea
	FOREACH(toolBarAction, ToolBarActions)
		if ((*toolBarAction).button)
		{
			delete (*toolBarAction).button;
			(*toolBarAction).button = 0;
		}

	QToolBar::show();
	QTimer::singleShot(0, this, SLOT(updateButtons()));
}

void ToolBar::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = xml_config_file->createElement(parent_element, "ToolBar");
	toolbar_elem.setAttribute("offset", offset());

	FOREACH(toolBarAction, ToolBarActions)
	{
		if ((*toolBarAction).button)
			(*toolBarAction).showLabel = (*toolBarAction).button->usesTextLabel();

		QDomElement button_elem = xml_config_file->createElement(toolbar_elem, "ToolButton");
		button_elem.setAttribute("action_name", (*toolBarAction).actionName);
		button_elem.setAttribute("uses_text_label", (*toolBarAction).showLabel);
	}

	kdebugf2();
}

bool ToolBar::hasAction(const QString &action_name)
{
	kdebugf();

	CONST_FOREACH(toolBarAction, ToolBarActions)
		if ((*toolBarAction).actionName == action_name)
			return true;
	return false;

	kdebugf2();
}

DockArea* ToolBar::dockArea()
{
	return (DockArea*)area();
}

void ToolBar::actionLoaded(const QString &actionName)
{
	if (!hasAction(actionName))
		return;

	QTimer::singleShot(0, this, SLOT(updateButtons()));
}

void ToolBar::actionUnloaded(const QString &actionName)
{
	if (!hasAction(actionName))
		return;

	updateButtons();
}

void ToolBar::updateButtons()
{
	ToolButton *lastButton = 0;
	DockArea *dockarea = (DockArea *)area();

	FOREACH(toolBarAction, ToolBarActions)
	{
		const QString &actionName = (*toolBarAction).actionName;

		if ((*toolBarAction).button)
		{
			if (KaduActions.contains(actionName))
			{
				lastButton = (*toolBarAction).button;
				continue;
			}
			else
			{
				if ((*toolBarAction).button)
				{
					delete (*toolBarAction).button;
					(*toolBarAction).button = 0;
				}
			}
		}

		if (KaduActions.contains(actionName))
		{
			if (!dockarea || dockarea->supportsAction(KaduActions[actionName]->actionType()))
			{
				(*toolBarAction).button = addButton(KaduActions[actionName], (*toolBarAction).showLabel, lastButton);
				lastButton = (*toolBarAction).button;
			}

#if 0
			// TODO: enable it? i don't think so
			else
				ToolBarActions.remove(toolBarAction);
#endif

		}
	}

// 	adjustSize();
	updateGeometry();
// 	adjustSize();

	if (dockarea)
		dockarea->writeToConfig();
}

void ToolBar::loadFromConfig(QDomElement toolbar_element)
{
	kdebugf();

	setOffset(toolbar_element.attribute("offset").toInt());

	ToolBarActions.clear();

	for (QDomNode n = toolbar_element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &button_elem = n.toElement();
		if (button_elem.isNull())
			continue;
		if (button_elem.tagName() != "ToolButton")
			continue;

		ToolBarAction action;

		action.actionName = button_elem.attribute("action_name");
		if (button_elem.attribute("uses_text_label").isNull())
			action.showLabel = false;
		else
			action.showLabel = button_elem.attribute("uses_text_label") == "1";
		action.button = 0;

		ToolBarActions.append(action);
	}

	// TODO: why calling updateButtons does not work?
	QTimer::singleShot(0, this, SLOT(updateButtons()));

	kdebugf2();
}

void ToolBar::addDefaultAction(const QString &toolbar, const QString &actionName, int index, bool showLabel)
{
	kdebugf();

	QValueList<ToolBarAction> &actions = DefaultActions[toolbar];

	ToolBarAction action;
	action.actionName = actionName;
	action.showLabel = showLabel;
	action.button = 0;

	if (index >= (int)actions.size())
	{
		kdebugm(KDEBUG_ERROR, "requested action index (%d) >= actions size (%u)!\n", index, actions.size());
		printBacktrace("requested action index >= actions size!");
		index = -1;
	}

	if (index < 0)
		actions.push_back(action);
	else
		actions.insert(actions.at(index), action);

	kdebugf2();
}

void ToolBar::loadDefault()
{
	kdebugf();

	if (DefaultActions.contains(name()))
	{
		const QValueList<ToolBarAction>& actions = DefaultActions[name()];
		CONST_FOREACH(i, actions)
			addAction((*i).actionName, (*i).showLabel, 0);
	}

	// TODO: why calling updateButtons does not work?
	QTimer::singleShot(0, this, SLOT(updateButtons()));

	kdebugf2();
}

const UserGroup* ToolBar::selectedUsers() const
{
	kdebugf();
	DockArea *dockArea = dynamic_cast<DockArea*>(area());
	const UserGroup *users = NULL;

	// dont segfault on floating toolbars
	if (dockArea)
		users = dockArea->selectedUsers();

	kdebugf2();
	return users;
}

QPopupMenu* ToolBar::createContextMenu(QWidget* parent)
{
	QPopupMenu* p = new QPopupMenu(parent);
	p->insertItem(tr("Delete toolbar"), this, SLOT(deleteToolbar()));

	QPopupMenu* p2 = new QPopupMenu(p); // popup z akcjami mozliwymi do dodania w toolbarze
	unsigned int param = 0; // parametr przekazywany slotowi addButtonClicked()
	CONST_FOREACH(a, KaduActions)
	{
		if (!hasAction((*a)->name()) &&
			((dockArea() != NULL && dockArea()->supportsAction((*a)->actionType())) ||
			(dockArea() == NULL && ((*a)->actionType() & Action::TypeGlobal) != 0)))
		{
			unsigned int index = (*a)->addToPopupMenu(p2, false);
			p2->setItemParameter(index, param);
			p2->connectItem(index, this, SLOT(addButtonClicked(int)));
		}
		param++;
	}
	if (!p2->count()) // jezeli nie zostaly zadne akcje do dodania, dodajemy wpis informacyjny
	{
		p2->insertItem("No items to add found", 0);
		p2->setItemEnabled(0, false);
	}

	p->insertItem(tr("Add new button"), p2);
	p->insertSeparator();
	if (dockArea())
	{
		QPopupMenu* panel_menu = dockArea()->createContextMenu(p);
		p->insertItem(tr("Panel menu"), panel_menu);
	}
	return p;
}

void ToolBar::deleteToolbar()
{
	kdebugf();
	if (MessageBox::ask(tr("Remove toolbar?"), "Warning", this))
		deleteLater();
	kdebugf2();
}
