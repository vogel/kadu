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

ToolButton::ToolButton(QWidget* parent, const QString& action_name, Action::ActionType Type)
	: QToolButton(parent, 0), ActionName(action_name), InOnState(false),
	OffIcon(), OffText(), OnIcon(), OnText(), IsEnabled(true), Type(Type)

{
	kdebugf();
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));

	// ignore signal on global actions - they are always active

	if ((Type & Action::TypeGlobal) == 0)
		connect(parent, SIGNAL(placeChanged(QDockWindow::Place)), this, SLOT(toolbarPlaceChanged(QDockWindow::Place)));

	kdebugf2();
}

ToolButton::~ToolButton()
{
	kdebugf();
	kdebugf2();
}

void ToolButton::setEnabled(bool enabled)
{
	// TODO: refactor in 0.6
	// isEnabled is internal enabled state, it is used to save enabled state when action goes floating and then docked again

	IsEnabled = enabled;

	if (((Type & Action::TypeGlobal) != 0) || !enabled)
		QToolButton::setEnabled(IsEnabled);
	else
	{
		ToolBar *toolBar = dynamic_cast<ToolBar *>(parent());
		if (toolBar->place() == QDockWindow::InDock)
			QToolButton::setEnabled(toolBar->dockArea()->supportsAction(Type));
		else
			QToolButton::setEnabled(false);
	}
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
	if (e->state() & LeftButton && !toolbar()->dockArea()->blocked())
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
	p->insertSeparator();
	QPopupMenu* toolbar_menu = toolbar()->createContextMenu(p);
	p->insertItem(tr("Toolbar menu"), toolbar_menu);
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
	if (dockarea)
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

void ToolButton::toolbarPlaceChanged(QDockWindow::Place p)
{
	kdebugf();
	setEnabled(IsEnabled);
	kdebugf2();
}

QString ToolButton::actionName()
{
	return ActionName;
}

ToolBar* ToolButton::toolbar()
{
	return (ToolBar*)parent();
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
	: QTextDrag(QString::number(button->winId()), dragSource, name), Button(0)
{
	kdebugf();
	kdebugf2();
}
