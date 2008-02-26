/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qobject.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QChildEvent>
#include <Q3ValueList>
#include <Q3PtrList>
#include <Q3PopupMenu>

#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "misc.h"
#include "toolbar.h"

DockArea::DockArea(Qt::Orientation o, HandlePosition h,
			QWidget* parent, const char* name,
			int supportedActions)
	: Q3DockArea(o, h, parent, name), SupportedActions(supportedActions)
{
	kdebugf();
	AllDockAreas.append(this);
	kdebugf2();
}

DockArea::~DockArea()
{
	kdebugf();
	AllDockAreas.remove(this);
	kdebugf2();
}

void DockArea::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	Q3PopupMenu* p = createContextMenu(this);
	showPopupMenu(p);
	delete p;
	e->accept();
	kdebugf2();
}

void DockArea::childEvent(QChildEvent* e)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "child:%p inserted:%d removed:%d\n", e->child(), e->inserted(), e->removed());
	Q3DockArea::childEvent(e);
	if (e->inserted())
	{
		ToolBar* toolbar = dynamic_cast<ToolBar*>(e->child());
		if (toolbar)
		{
			connect(toolbar, SIGNAL(destroyed()), this, SLOT(writeToConfig()));
			connect(toolbar, SIGNAL(placeChanged(Q3DockWindow::Place)),
				this, SLOT(writeToConfig()));
			emit toolbarAttached();
		}
	}
	kdebugf2();
}

void DockArea::toolbarPlaceChanged()
{
	kdebugf();
	const ToolBar* toolbar = dynamic_cast<const ToolBar*>(sender());
	if (toolbar != NULL && toolbar->area() != this)
	{
		disconnect(toolbar, SIGNAL(destroyed()), this, SLOT(writeToConfig()));
		disconnect(toolbar, SIGNAL(placeChanged(Q3DockWindow::Place)),
				this, SLOT(writeToConfig()));
		writeToConfig();
		((DockArea*)toolbar->area())->writeToConfig();
	}
	kdebugf2();
}

void DockArea::createNewToolbar()
{
	kdebugf();
	ToolBar* tb = new ToolBar(NULL, "New toolbar");
	tb->show();
	moveDockWindow(tb);
	tb->setOrientation(orientation());
	setAcceptDockWindow(tb, true);
	writeToConfig();
	kdebugf2();
}

void DockArea::setBlockToolbars(bool b)
{
	kdebugf();
	Blocked = b;
	QList<Q3DockWindow *> l = dockWindowList();
	uint wcount = l.count();
	for (uint i = 0; i < wcount; ++i)
		l.at(i)->setMovingEnabled(!Blocked);
	kdebugf2();
}

void DockArea::blockToolbars()
{
	kdebugf();
	Blocked = !Blocked;
	CONST_FOREACH(DockArea, AllDockAreas)
	{
		(*DockArea)->setBlockToolbars(Blocked);
		(*DockArea)->writeToConfig();
	}
	kdebugf2();
}

void DockArea::writeToConfig()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement toolbars_elem = xml_config_file->accessElement(root_elem, "Toolbars");
	toolbars_elem.setAttribute("blocked", Blocked);
	QDomElement dockarea_elem = xml_config_file->accessElementByProperty(
		toolbars_elem, "DockArea", "name", name());
	xml_config_file->removeChildren(dockarea_elem);
	QList<Q3DockWindow *> dock_windows = dockWindowList();
	for (QList<Q3DockWindow *>::iterator i = dock_windows.begin();
		i != dock_windows.end(); i++)
	{
		ToolBar* toolbar = dynamic_cast<ToolBar*>(*i);
		if (toolbar != NULL)
			toolbar->writeToConfig(dockarea_elem);
	}
	kdebugf2();
}

bool DockArea::loadFromConfig(QWidget* toolbars_parent)
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement toolbars_elem = xml_config_file->findElement(root_elem, "Toolbars");
	if (!toolbars_elem.isNull())
	{
		setBlockToolbars(toolbars_elem.attribute("blocked").toInt());
		QDomElement dockarea_elem = xml_config_file->findElementByProperty(
			toolbars_elem, "DockArea", "name", name());
		if (!dockarea_elem.isNull())
		{
			for (QDomNode n = dockarea_elem.firstChild(); !n.isNull(); n = n.nextSibling())
			{
				const QDomElement &toolbar_elem = n.toElement();
				if (toolbar_elem.isNull())
					continue;
				if (toolbar_elem.tagName() != "ToolBar")
					continue;

				ToolBar* toolbar = new ToolBar(toolbars_parent, "");
				moveDockWindow(toolbar);
				toolbar->setOrientation(orientation());
				toolbar->loadFromConfig(toolbar_elem);
				toolbar->show();
				setAcceptDockWindow(toolbar, true);
			}
			return true;
		}
	}
	kdebugf2();
	return false;
}

void DockArea::usersChangedSlot()
{
	const QObjectList childList = children();

	FOREACH(child, childList)
	{
		ToolBar *toolbar = dynamic_cast<ToolBar *>(*child);
		if (toolbar)
			toolbar->usersChanged();
	}
}

const UserGroup* DockArea::selectedUsers()
{
	kdebugf();
	const UserGroup *users = NULL;
	emit selectedUsersNeeded(users);
	kdebugf2();
	return users;
}

bool DockArea::blocked()
{
	return Blocked;
}

Q3PopupMenu* DockArea::createContextMenu(QWidget* parent)
{
	kdebugf();
	Q3PopupMenu* p = new Q3PopupMenu(parent);
	if (!blocked())
		p->insertItem(tr("Create new toolbar"), this, SLOT(createNewToolbar()));
	int block_toolbars_id =
		p->insertItem(tr("Block toolbars"), this, SLOT(blockToolbars()));
	p->setItemChecked(block_toolbars_id, Blocked);
	kdebugf2();
	return p;
}

bool DockArea::supportsAction(int actionType)
{
	return (SupportedActions & actionType) != 0;
}

bool DockArea::Blocked = true;
Q3ValueList<DockArea *> DockArea::AllDockAreas;
