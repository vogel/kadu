/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qpopupmenu.h>

#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "toolbar.h"
#include "toolbutton.h"

ToolButton::ToolButton(QWidget* parent, const QString& action_name)
	: QToolButton(parent, 0)
{
	kdebugf();
	ActionName = action_name;
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	kdebugf2();
}

ToolButton::~ToolButton()
{
	kdebugf();
	kdebugf2();
}

void ToolButton::setOnShape(const QIconSet& icon, const QString& text)
{
	OffIcon = iconSet();
	OffText = textLabel();
	OnIcon = icon;
	OnText = text;
	setOn(false);
}

bool ToolButton::isOn() const
{
	kdebugf();
	bool res;
	if (OnIcon.isNull())
		res = QToolButton::isOn();
	else
		res = InOnState;
	kdebugf2();
	return res;
}

void ToolButton::setOn(bool on)
{
	kdebugf();
	if (OnIcon.isNull())
		QToolButton::setOn(on);
	else
	{
		if (on)
		{
			setIconSet(OnIcon);
			setTextLabel(OnText);
			InOnState = true;
		}
		else
		{
			setIconSet(OffIcon);
			setTextLabel(OffText);
			InOnState = false;
		}
	}
	kdebugf2();
}

void ToolButton::mouseMoveEvent(QMouseEvent* e)
{
//	kdebugf();
	QToolButton::mouseMoveEvent(e);
	if (e->state() & LeftButton)
	{
		QDragObject* d = new ToolButtonDrag(this, parentWidget());
		d->dragMove();
	}
//	kdebugf2();
}

void ToolButton::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	int label_menu_id =
		p->insertItem(tr("Show text label"), this, SLOT(showTextLabelClicked()));
	p->setItemChecked(label_menu_id, usesTextLabel());
	p->insertItem(tr("Delete button"), this, SLOT(deleteButtonClicked()));
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void ToolButton::buttonClicked()
{
	kdebugf();
	if (!OnIcon.isNull())
		setOn(!InOnState);
	kdebugf2();
}

void ToolButton::deleteButtonClicked()
{
	kdebugf();
	ToolBar* toolbar = (ToolBar*)parent();
	toolbar->removeChild(this); //temporarily remove from list, for writeToConfig()
	DockArea* dockarea = (DockArea*)toolbar->area();
	dockarea->writeToConfig();
	toolbar->insertChild(this); // restore
	deleteLater();
	kdebugf2();
}

void ToolButton::showTextLabelClicked()
{
	kdebugf();
	setUsesTextLabel(!usesTextLabel());
	ToolBar* toolbar = (ToolBar*)parent();
	DockArea* dockarea = (DockArea*)toolbar->area();
	dockarea->writeToConfig();
	kdebugf2();
}

QString ToolButton::actionName()
{
	return ActionName;
}

void ToolButton::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement button_elem = xml_config_file->createElement(parent_element, "ToolButton");
	button_elem.setAttribute("action_name", ActionName);
	if (usesTextLabel())
		button_elem.setAttribute("uses_text_label", true);
	kdebugf2();
}

ToolButtonDrag::ToolButtonDrag(ToolButton* button, QWidget* dragSource, const char* name)
	: QTextDrag(QString::number(button->winId()), dragSource, name)
{
	kdebugf();
	kdebugf2();
}
