/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpopupmenu.h>
#include <qcursor.h>
#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "toolbar.h"

DockArea::DockArea(Orientation o, HandlePosition h,
			QWidget * parent, const char * name)
	: QDockArea(o, h, parent, name)
{
	kdebugf();
	kdebugf2();
}

DockArea::~DockArea()
{
	kdebugf();
	kdebugf2();
}

void DockArea::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	p->insertItem(tr("Create new toolbar"), this, SLOT(createNewToolbar()));
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void DockArea::createNewToolbar()
{
	kdebugf();
	kdebug("Creating new toolbar\n");
	ToolBar* tb = new ToolBar(NULL, "New toolbar");
	tb->show();
	moveDockWindow(tb);
	setAcceptDockWindow(tb, true);
	writeToConfig();
	kdebugf2();
}

void DockArea::writeToConfig()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement toolbars_elem = xml_config_file->accessElement(root_elem, "Toolbars");
	QDomElement dockarea_elem = xml_config_file->accessElementByProperty(
		toolbars_elem, "DockArea", "name", name());
	xml_config_file->removeChildren(dockarea_elem);
	QPtrList<QDockWindow> dock_windows = dockWindowList();
	for (QPtrList<QDockWindow>::iterator i = dock_windows.begin();
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
		QDomElement dockarea_elem = xml_config_file->findElementByProperty(
			toolbars_elem, "DockArea", "name", name());
		if (!dockarea_elem.isNull())
		{
			QDomNodeList toolbars = dockarea_elem.elementsByTagName("ToolBar");
			for (unsigned int i = 0; i < toolbars.count(); i++)
			{
				ToolBar* toolbar = new ToolBar(toolbars_parent, QString());
				moveDockWindow(toolbar);
				toolbar->loadFromConfig(toolbars.item(i).toElement());
				toolbar->show();
				setAcceptDockWindow(toolbar, true);
			}
			return true;
		}
	}
	return false;
	kdebugf2();
}

const UserGroup* DockArea::selectedUsers()
{
	kdebugf();
	const UserGroup* users = NULL;
	emit selectedUsersNeeded(users);
	kdebugf2();
	return users;
}
