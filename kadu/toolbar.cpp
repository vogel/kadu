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
		// w trakcie dragLeave nie mo¿na sprawdziæ ¼ród³a, wiêc zapamiêtujemy go sobie
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
			// TODO: uzywanie WId moze nie byc zbyt przenosne ;)
			// Jakis lepszy pomysl?
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

			// je¿eli upu¶cili¶my przycisk na nim samym,
			// to symulujemy zwyk³e naci¶niêcie przycisku
			if (button == widget)
				button->animateClick();
			source->dragButton = NULL;

			// zapisujemy dockarea
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
	QPopupMenu* p = new QPopupMenu(this);
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
	for (QObjectList::iterator i = l->begin(); i != l->end(); i++)
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

void ToolBar::loadFromConfig(QDomElement toolbar_element)
{
	kdebugf();
	setOffset(toolbar_element.attribute("offset").toInt());
	QDomNodeList buttons = toolbar_element.elementsByTagName("ToolButton");
	for (unsigned int i = 0; i < buttons.count(); i++)
	{
		QDomElement button_elem = buttons.item(i).toElement();
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

const UserGroup* ToolBar::selectedUsers()
{
	kdebugf();
	const UserGroup* users = dynamic_cast<DockArea*>(area())->selectedUsers();
	kdebugf2();
	return users;
}
