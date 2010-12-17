/*
 * %kadu copyright begin%
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "notify/notification-manager.h"
#include "parser/parser.h"
#include "icons-manager.h"
#include "debug.h"

#include "notification.h"

static QString getNotificationTitle(const QObject * const object)
{
	kdebugf();

	const Notification * const notification = dynamic_cast<const Notification * const>(object);
	if (notification)
		return notification->title();
	else
		return QString();
}

Notification::Notification(const QString &type, const QString &iconPath) :
	Type(type), IconPath(IconsManager::instance()->iconPath(iconPath, "16x16")),
	Icon(IconsManager::instance()->iconByPath(iconPath)), DefaultCallbackTimer(0),
	ReferencesCount(0), Closing(false)
{
	Parser::registerObjectTag("event", getNotificationTitle);
}

Notification::~Notification()
{
	Parser::unregisterObjectTag("event", getNotificationTitle);
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

QString Notification::type() const
{
	return Type;
}

QString Notification::key() const
{
	return NotificationManager::instance()->notifyConfigurationKey(Type);
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

void Notification::setIcon(const QString& iconPath)
{
	IconPath = IconsManager::instance()->iconPath(iconPath, "16x16");

	Icon = IconsManager::instance()->iconByPath(iconPath);
}

QString Notification::iconPath() const
{
	return IconPath;
}

QIcon Notification::icon() const
{
	return Icon;
}

const QList<Notification::Callback> & Notification::getCallbacks()
{
	return Callbacks;
}
