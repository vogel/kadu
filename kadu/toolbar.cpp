/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qobjectlist.h>
#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "misc.h"
#include "toolbar.h"

ToolBar::ToolBar(QWidget* parent, const QString& label)
	: QToolBar(NULL, label), dragButton(NULL)
{
	kdebugf();
	setAcceptDrops(true);
	kdebugf2();
}


ToolBar::~ToolBar()
{
	kdebugf();
	kdebugf2();
}

void ToolBar::addButtonClicked(int action_index)
{
	kdebugf();
	kdebug("action_index = %d\n", action_index);
	KaduActions[KaduActions.keys()[action_index]]->addToToolbar(this);
	DockArea* dockarea = (DockArea*)area();
	dockarea->writeToConfig();
	kdebugf2();
}

void ToolBar::dragEnterEvent(QDragEnterEvent* event)
{
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());
	if (source)
	{
		QString text;
		// during dragLeave event we cannot check source, so we are remembering it here
		if (QTextDrag::decode(event, text))
		{
			dragButton = (ToolButton*)source->find(text.toULong());
			QString action_name = dragButton->actionName();
			QString dockarea_group_restr =
				KaduActions[action_name]->dockAreaGroupRestriction();
			if (dockarea_group_restr.isNull() ||
					dockarea_group_restr == dockAreaGroup())
				event->accept(true);
			else
				event->accept(false);
		}
	}
	else
		event->accept(false);
	kdebugf2();
}

void ToolBar::dragLeaveEvent(QDragLeaveEvent *event)
{
	kdebugf();
	if (dragButton)
	{
		dragButton->setDown(false);
		dragButton = NULL;
	}
	kdebugf2();
}

void ToolBar::dropEvent(QDropEvent* event)
{
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());
	if (source != NULL)
	{
		QString text;
		if (QTextDrag::decode(event, text))
		{
			ToolBar* source_toolbar = (ToolBar*)event->source();
			// TODO: using WId might be not portable ;), any ideas?
			ToolButton* button = (ToolButton*)source_toolbar->find(text.toULong());
			QWidget* widget = childAt(event->pos());
			button->reparent(this, QPoint(0,0), true);
			if (widget != NULL && widget != button)
			{
				button->stackUnder(widget);
				QBoxLayout* layout = boxLayout();
				layout->remove(button);
				layout->insertWidget(layout->findWidget(widget), button);
			}
			button->setDown(false);

			// if we dropped button on itself, then we simulate normal click
			if (button == widget)
				button->animateClick();
			source->dragButton = NULL;

			// saving dockarea
			DockArea* source_dockarea = (DockArea*)source_toolbar->area();
			source_dockarea->writeToConfig();
			DockArea* dockarea = (DockArea*)area();
			dockarea->writeToConfig();
		}
	}
	kdebugf2();
}

void ToolBar::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = createContextMenu(this);
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void ToolBar::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = xml_config_file->createElement(
		parent_element, "ToolBar");
	toolbar_elem.setAttribute("offset", offset());
	QObjectList* l = queryList("ToolButton");
	CONST_FOREACH(i, *l)
		((ToolButton*)(*i))->writeToConfig(toolbar_elem);
	kdebugf2();
}

QString ToolBar::dockAreaGroup()
{
	kdebugf();
	DockArea* dockarea = (DockArea*)area();
	return dockarea->dockAreaGroup();
	kdebugf2();
}

DockArea* ToolBar::dockArea()
{
	return (DockArea*)area();
}

void ToolBar::loadFromConfig(QDomElement toolbar_element)
{
	kdebugf();
	setOffset(toolbar_element.attribute("offset").toInt());

	for (QDomNode n = toolbar_element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &button_elem = n.toElement();
		if (button_elem.isNull())
			continue;
		if (button_elem.tagName() != "ToolButton")
			continue;

		QString action_name = button_elem.attribute("action_name");
		bool uses_text_label = !button_elem.attribute("uses_text_label").isNull();
		if (KaduActions.contains(action_name))
		{
			QString dockarea_group_restr =
				KaduActions[action_name]->dockAreaGroupRestriction();
			if (dockarea_group_restr.isNull() ||
					dockarea_group_restr == dockAreaGroup())
				KaduActions[action_name]->addToToolbar(this, uses_text_label);
		}
	}
	kdebugf2();
}

const UserGroup* ToolBar::selectedUsers() const
{
	kdebugf();
	const UserGroup* users = dynamic_cast<DockArea*>(area())->selectedUsers();
	kdebugf2();
	return users;
}

QPopupMenu* ToolBar::createContextMenu(QWidget* parent)
{
	QPopupMenu* p = new QPopupMenu(parent);
	p->insertItem(tr("Delete toolbar"), this, SLOT(deleteLater()));
	QPopupMenu* p2 = new QPopupMenu(p);
	int param = 0;
	CONST_FOREACH(a, KaduActions)
	{
		QString dockarea_group_restr = (*a)->dockAreaGroupRestriction();
		if (dockarea_group_restr.isNull() || dockarea_group_restr == dockAreaGroup())
		{
			int id = (*a)->addToPopupMenu(p2, false);
			p2->setItemParameter(id, param);
			p2->connectItem(id, this, SLOT(addButtonClicked(int)));
		}
		param++;
	}
	p->insertItem(tr("Add new button"), p2);
	p->insertSeparator();
	QPopupMenu* panel_menu = dockArea()->createContextMenu(p);
	p->insertItem(tr("Panel menu"), panel_menu);
	return p;
}
