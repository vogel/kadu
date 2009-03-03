/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>

#include "debug.h"
#include "kadu_parser.h"
#include "notification.h"

static QString getNotificationTitle(const QObject * const object)
{
	kdebugf();

	const Notification * const notification = dynamic_cast<const Notification * const>(object);
	if (notification)
		return notification->title();
	else
		return "";
}

Notification::Notification(const QString &type, const QIcon &icon, const ContactList &contacts)
	: Type(type), Contacts(contacts), Title(""), Text(""), Icon(icon), DefaultCallbackTimer(0), ReferencesCount(0), Closing(false)
{
	KaduParser::registerObjectTag("event", getNotificationTitle);
}

Notification::~Notification()
{
	KaduParser::unregisterObjectTag("event", getNotificationTitle);

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
	DefaultCallbackTimer->setSingleShot(true);
	connect(DefaultCallbackTimer, SIGNAL(timeout()), this, defaultSlot);
	DefaultCallbackTimer->start(timeout);
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

const ContactList &Notification::contacts() const
{
	return Contacts;
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

void Notification::setIcon(const QPixmap &icon)
{
	Icon = icon;
}

QIcon Notification::icon() const
{
	return Icon;
}

const QList<QPair<QString, const char *> > & Notification::getCallbacks()
{
	return Callbacks;
}
