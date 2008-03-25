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
#include "misc.h"
#include "toolbar.h"
#include "toolbutton.h"
/*
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

DisabledToolButtonWatcher *watcher = 0;*/

/*
void ToolButton::mousePressEvent(QMouseEvent* e)
{
	QToolButton::mousePressEvent(e);
	MouseStart = e->pos();
}

void ToolButton::mouseMoveEvent(QMouseEvent* e)
{
	QToolButton::mouseMoveEvent(e);
	if (e->state() & Qt::LeftButton && / *!toolbar()->dockArea()->blocked() && * /(MouseStart - e->pos()).manhattanLength() >= 15)
	{
		setDown(false);
		Q3DragObject* d = new ActionDrag(ActionName, usesTextLabel(), parentWidget());
		d->dragMove();
	}
}*/

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
