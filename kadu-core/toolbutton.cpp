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
#include <q3popupmenu.h>
#include <q3textstream.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QEvent>

#include "config_file.h"
#include "debug.h"
#include "dockarea.h"
#include "misc.h"
#include "toolbar.h"
#include "toolbutton.h"

class DisabledToolButtonWatcher : public QObject
{
public:
	DisabledToolButtonWatcher()
	{
		qApp->installEventFilter(this);
	}

	virtual bool eventFilter(QObject *o, QEvent *e)
	{
		ToolButton *button = dynamic_cast<ToolButton *>(o);
		if (!button)
			return false;

		switch (e->type())
		{
			case QEvent::MouseMove:
				button->mouseMoveEvent((QMouseEvent *)e);
				return ((QMouseEvent *)e)->isAccepted();
			case QEvent::ContextMenu:
				button->contextMenuEvent((QContextMenuEvent *)e);
				return ((QContextMenuEvent *)e)->isAccepted();
			default:
				return false;
		}
	}
};

DisabledToolButtonWatcher *watcher = 0;

ToolButton::ToolButton(QWidget* parent, const QString& action_name, ActionDescription::ActionType Type)
	: QToolButton(parent, 0), ActionName(action_name), InOnState(false),
	OffIcon(), OffText(), OnIcon(), OnText(), IsEnabled(true), Type(Type)

{
	kdebugf();
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));

	if (!watcher)
		watcher = new DisabledToolButtonWatcher();

	// ignore signal on global actions - they are always active

	if ((Type & ActionDescription::TypeGlobal) == 0)
	{
		connect(parent, SIGNAL(placeChanged(Q3DockWindow::Place)), this, SLOT(toolbarPlaceChanged(Q3DockWindow::Place)));
		setEnabled(IsEnabled);
	}

	kdebugf2();
}

ToolButton::~ToolButton()
{
	kdebugf();
	kdebugf2();
}

void ToolButton::usersChanged()
{
	setEnabled(IsEnabled);
}

void ToolButton::setEnabled(bool enabled)
{
	// TODO: refactor in 0.6
	// isEnabled is internal enabled state, it is used to save enabled state when action goes floating and then docked again

	IsEnabled = enabled;

	if (((Type & ActionDescription::TypeGlobal) != 0) || !enabled)
		QToolButton::setEnabled(IsEnabled);
	else
	{
		ToolBar *toolBar = dynamic_cast<ToolBar *>(parent());
		if (toolBar->place() == Q3DockWindow::InDock)
		{
			if (toolBar->dockArea()->supportsAction(Type))
			{
				if (Type & ActionDescription::TypeUser)
					QToolButton::setEnabled(toolBar->selectedUsers() && toolBar->selectedUsers()->count());
				else
					QToolButton::setEnabled(true);
			}
			else
				QToolButton::setEnabled(false);
		}
		else
			QToolButton::setEnabled(false);
	}
}

void ToolButton::setOnShape(const QIcon& on_icon, const QString& on_text)
{
	OffIcon = icon();
	OffText = text();
	OnIcon = on_icon;
	OnText = on_text;

	setOn(isOn());
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
	if (!OnIcon.isNull())
	{
		if (on)
		{
			setIcon(OnIcon);
			setText(OnText);
			InOnState = true;
		}
		else
		{
			setIcon(OffIcon);
			setText(OffText);
			InOnState = false;
		}
	}
	QToolButton::setOn(on);
	kdebugf2();
}

void ToolButton::mousePressEvent(QMouseEvent* e)
{
	QToolButton::mousePressEvent(e);
	MouseStart = e->pos();
}

void ToolButton::mouseMoveEvent(QMouseEvent* e)
{
	QToolButton::mouseMoveEvent(e);
	if (e->state() & Qt::LeftButton && !toolbar()->dockArea()->blocked() && (MouseStart - e->pos()).manhattanLength() >= 15)
	{
		setDown(false);
		Q3DragObject* d = new ActionDrag(ActionName, usesTextLabel(), parentWidget());
		d->dragMove();
	}
}

void ToolButton::contextMenuEvent(QContextMenuEvent *e)
{
	kdebugf();
	if (DockArea::blocked())
		e->ignore();
	else
	{
		Q3PopupMenu* p = new Q3PopupMenu(this);
		int label_menu_id = p->insertItem(tr("Show text label"), this, SLOT(showTextLabelClicked()));
		p->setItemChecked(label_menu_id, usesTextLabel());
		p->insertItem(tr("Delete button"), this, SLOT(deleteButtonClicked()));
		p->insertSeparator();
		Q3PopupMenu* toolbar_menu = toolbar()->createContextMenu(p);
		p->insertItem(tr("Toolbar menu"), toolbar_menu);
		showPopupMenu(p);
		delete p;
		e->accept();
	}
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

	emit removedFromToolbar(this);

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
	if (dockarea)
		dockarea->writeToConfig();

	kdebugf2();
}

void ToolButton::toolbarPlaceChanged(Q3DockWindow::Place p)
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

ActionDrag::ActionDrag(const QString &actionName, bool showLabel, QWidget* dragSource, const char* name)
	: DragSimple("application/x-kadu-action", actionName + " " + QString::number(showLabel ? 1 : 0), dragSource, name)
{
	kdebugf();
	kdebugf2();
}

bool ActionDrag::decode(const QMimeSource *source, QString &actionName, bool &showLabel)
{
	Q3TextStream stream(new QString(source->encodedData("application/x-kadu-action")), QIODevice::ReadOnly);

	if (stream.atEnd())
		return false;

	stream >> actionName;

	if (stream.atEnd())
		return false;

	int tmp;
	stream >> tmp;
	showLabel = tmp;

	return true;
}
