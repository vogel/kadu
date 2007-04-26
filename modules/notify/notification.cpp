/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qtimer.h>

#include "debug.h"

#include "notification.h"

Notification::Notification(const QString &type, const QString &icon, const UserListElements &userListElements)
	: Type(type), Ule(userListElements), Title(""), Text(""), Icon(icon), DefaultCallbackTimer(0), ReferencesCount(0), Closing(false)
{
}

Notification::~Notification()
{
	if (DefaultCallbackTimer)
	{
		delete DefaultCallbackTimer;
		DefaultCallbackTimer = 0;
	}
}

void Notification::acquire()
{
	kdebugf();

	ReferencesCount++;
}

void Notification::release()
{
	kdebugf();

	ReferencesCount--;

	if (ReferencesCount <= 0)
		close();
}

void Notification::close()
{
	kdebugf();

	if (!Closing)
	{
		Closing = true;
		emit closed(this);
		deleteLater();
	}
}

void Notification::clearCallbacks()
{
	Callbacks.clear();
}

void Notification::addCallback(const QString &caption, const char *slot)
{
	Callbacks.append(qMakePair(caption, slot));
}

void Notification::setDefaultCallback(int timeout, const char *defaultSlot)
{
	DefaultCallbackTimer = new QTimer(this);
	connect(DefaultCallbackTimer, SIGNAL(timeout()), this, defaultSlot);
	DefaultCallbackTimer->start(timeout, true);
}

void Notification::callbackAccept()
{
	close();
}

void Notification::callbackDiscard()
{
	close();
}

void Notification::clearDefaultCallback()
{
	if (DefaultCallbackTimer)
	{
		delete DefaultCallbackTimer;
		DefaultCallbackTimer = 0;
	}
}

QString Notification::type() const
{
	return Type;
}

const UserListElements &Notification::userListElements() const
{
	return Ule;
}

void Notification::setTitle(const QString &title)
{
	Title = title;
}

QString Notification::title() const
{
	return Title;
}

void Notification::setText(const QString &text)
{
	Text = text;
}

QString Notification::text() const
{
	return Text;
}

void Notification::setDetails(const QString &details)
{
	Details = details;
}

QString Notification::details() const
{
	return Details;
}

void Notification::setIcon(const QString &icon)
{
	Icon = icon;
}

QString Notification::icon() const
{
	return Icon;
}

const QValueList<QPair<QString, const char *> > & Notification::getCallbacks()
{
	return Callbacks;
}
