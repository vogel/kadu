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
#include <qmessagebox.h>
#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "misc.h"
#include "toolbar.h"
#include "kadu.h"

ToolBar::ToolBar(QWidget* /*parent*/, const char *name)
	: QToolBar(NULL, name), dragButton(NULL)
{
	kdebugf();
	setAcceptDrops(true);
	setMovingEnabled(!DockArea::blocked());
	kdebugf2();
}


ToolBar::~ToolBar()
{
	kdebugf();
	kdebugf2();
}

void ToolBar::addButtonClicked(int action_index)
{
	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "action_index = %d\n", action_index);
	KaduActions[KaduActions.keys()[action_index]]->addToToolbar(this);
	DockArea* dockarea = (DockArea*)area();
	if (dockarea != NULL)
		dockarea->writeToConfig();
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
		QString text;
		// during dragLeave event we cannot check source, so we are remembering it here
		if (QTextDrag::decode(event, text))
		{
			dragButton = (ToolButton*)source->find(text.toULong());
			QString action_name = dragButton->actionName();

			// REVIEW: maybe we should remove it, accept by default and possibly turn off the button at once
			event->accept(dockArea()->supportsAction(KaduActions[action_name]->actionType()));
		}
	}
	else
		event->accept(false);
	kdebugf2();
}

void ToolBar::dragLeaveEvent(QDragLeaveEvent * /*event*/)
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

	if (DockArea::blocked())
		e->ignore();
	else
	{
		//NOTE: parent MUST be dockArea(), NOT this, because when user is choosing "remove toolbar",
		//      it calls deleteLater(), which is invoked _before_ exec returns! so QPopupMenu would
		//      be deleted when exec returns!
		QPopupMenu* p = createContextMenu(dockArea());
		showPopupMenu(p);
		delete p;
		e->accept();
	}

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
	delete l;
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
			if (dockArea()->supportsAction(KaduActions[action_name]->actionType()))
				KaduActions[action_name]->addToToolbar(this, uses_text_label);
		}
	}
	kdebugf2();
}

const UserGroup* ToolBar::selectedUsers() const
{
	kdebugf();
	DockArea *dockArea = dynamic_cast<DockArea*>(area());
	const UserGroup* users;

	// dont segfault on floating toolbars

	if (dockArea)
		users = dockArea->selectedUsers();
	else
		users = NULL;

	kdebugf2();
	return users;
}

QPopupMenu* ToolBar::createContextMenu(QWidget* parent)
{
	QPopupMenu* p = new QPopupMenu(parent);
	p->insertItem(tr("Delete toolbar"), this, SLOT(deleteToolbar()));
	QPopupMenu* p2 = new QPopupMenu(p);
	int param = 0;
	CONST_FOREACH(a, KaduActions)
	{
		if (
			(dockArea() != NULL &&
				dockArea()->supportsAction((*a)->actionType())) ||
			(dockArea() == NULL &&
				((*a)->actionType() & Action::TypeGlobal) != 0))
		{
			int id = (*a)->addToPopupMenu(p2, false);
			p2->setItemParameter(id, param);
			p2->connectItem(id, this, SLOT(addButtonClicked(int)));
		}
		param++;
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
	if (QMessageBox::question(kadu, tr("Remove toolbar?"), tr("Remove toolbar?"), tr("Yes"), tr("No"), QString::null, 1, 1) == 0)
		deleteLater();
	kdebugf2();
}
