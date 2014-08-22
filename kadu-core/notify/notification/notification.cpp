/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QTimer>

#include "icons/kadu-icon.h"
#include "notify/notification-manager.h"
#include "notify/notifier.h"
#include "parser/parser.h"
#include "debug.h"

#include "notification.h"

static QString getNotificationTitle(const ParserData * const object)
{
	kdebugf();

	const Notification * const notification = dynamic_cast<const Notification * const>(object);
	if (notification)
		return notification->title();
	else
		return QString();
}

void Notification::registerParserTags()
{
	Parser::registerObjectTag("event", getNotificationTitle);
}

void Notification::unregisterParserTags()
{
	Parser::unregisterObjectTag("event");
}

Notification::Notification(const QString &type, const KaduIcon &icon) :
	Type(type), Icon(icon), DefaultCallbackTimer(0), Closing(false)
{
}

Notification::~Notification()
{
}

void Notification::acquire(Notifier *notifier)
{
	kdebugf();

	Notifiers.insert(notifier);
}

void Notification::release(Notifier *notifier)
{
	kdebugf();

	Notifiers.remove(notifier);

	if (Notifiers.size() <= 0)
		close();
}

void Notification::close()
{
	kdebugf();

	emit closed(this);
}

void Notification::partialClose()
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

void Notification::addCallback(const QString &caption, const char *slot, const char *signature)
{
	Callback callback;
	callback.Caption = caption;
	callback.Slot = slot;
	callback.Signature = signature;

	Callbacks.append(callback);
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

QString Notification::key() const
{
	return NotificationManager::instance()->notifyConfigurationKey(Type);
}

void Notification::setTitle(const QString &title)
{
	Title = title;
}

void Notification::setText(const QString &text)
{
	Text = text;
}

void Notification::setDetails(const QStringList &details)
{
	Details = details;
}

void Notification::setDetails(const QString &details)
{
	Details = QStringList(details);
}

void Notification::setIcon(const KaduIcon &icon)
{
	Icon = icon;
}

#include "moc_notification.cpp"
