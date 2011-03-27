/*
 * Copyright 2009 Jacek Jabłoński
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMetaMethod>
#include <QtDBus/QDBusInterface>

#include "configuration/configuration-file.h"
#include "misc/path-conversion.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "url-handlers/url-handler-manager.h"
#include "debug.h"
#include "html_document.h"
#include "icons-manager.h"

#include <QDBusReply>
#include <QTimer>

#include "freedesktop-notify.h"

FreedesktopNotify * FreedesktopNotify::Instance = 0;

void FreedesktopNotify::createInstance()
{
	Instance = new FreedesktopNotify();
}

FreedesktopNotify * FreedesktopNotify::instance()
{
	return Instance;
}

void FreedesktopNotify::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

FreedesktopNotify::FreedesktopNotify() :
		Notifier("KNotify", QT_TRANSLATE_NOOP("@default", "System notifications"), "kadu_icons/notify-hints"),
		UseFreedesktopStandard(true)
{
	StripHTML.setPattern(QString::fromLatin1("<.*>"));
	StripHTML.setMinimal(true);

	KNotify = new QDBusInterface("org.freedesktop.Notifications",
			"/org/freedesktop/Notifications", "org.freedesktop.Notifications");

	// Fallback for older knotify
	if (!KNotify->isValid())
	{
		delete (KNotify);
		KNotify = new QDBusInterface("org.kde.VisualNotifications",
				"/VisualNotifications", "org.kde.VisualNotifications");

		UseFreedesktopStandard = false;
	}

	KNotify->connection().connect(KNotify->service(), KNotify->path(), KNotify->interface(),
		"ActionInvoked", this, SLOT(actionInvoked(unsigned int, QString)));

	NotificationManager::instance()->registerNotifier(this);
	createDefaultConfiguration();
}

FreedesktopNotify::~FreedesktopNotify()
{
	NotificationManager::instance()->unregisterNotifier(this);
	delete KNotify;
	KNotify = 0;
}

void FreedesktopNotify::createDefaultConfiguration()
{
	config_file.addVariable("KDENotify", "Timeout", 10);
	config_file.addVariable("KDENotify", "ShowContentMessage", true);
	config_file.addVariable("KDENotify", "CiteSign", 100);
}

void FreedesktopNotify::notify(Notification *notification)
{
	QList<QVariant> args;
	args.append("Kadu");
	args.append(0U);

	if (notification->iconPath().isEmpty())
		args.append(IconsManager::instance()->iconPath("kadu_icons/section-kadu", "32x32"));
	else
		args.append(IconsManager::instance()->iconPath(notification->iconPath(), "32x32"));

	// the new spec doesn't have this
	if (!UseFreedesktopStandard)
		args.append(QString());

  	args.append("Kadu");

	QString text;

	if (((notification->type() == "NewMessage") || (notification->type() == "NewChat")) &&
			config_file.readBoolEntry("KDENotify", "ShowContentMessage"))
	{
		text.append(notification->text() + "<br/><small>");

		QString strippedDetails = notification->details().replace("<br/>", "\n").remove(StripHTML).replace('\n', QLatin1String("<br/>"));
		if (strippedDetails.length() > config_file.readNumEntry("KDENotify", "CiteSign", 10))
			text.append(strippedDetails.left(config_file.readNumEntry("KDENotify", "CiteSign", 10)) + "...");
		else
			text.append(strippedDetails);

		text.append("</small>");
	}
	else
		text.append(notification->text());

	HtmlDocument doc;
	doc.parseHtml(text);
	UrlHandlerManager::instance()->convertAllUrls(doc);

	args.append(doc.generateHtml());

	QStringList actions;

	foreach (const Notification::Callback &callback, notification->getCallbacks())
	{
		actions << callback.Signature;
		actions << callback.Caption;
	}

	args.append(actions);
	args.append(QVariantMap());
	args.append(config_file.readNumEntry("KDENotify", "Timeout", 10) * 1000);

	QDBusReply<unsigned int> reply = KNotify->callWithArgumentList(QDBus::Block, "Notify", args);
	if (reply.isValid())
	{
		notification->acquire(); // do not remove now

		connect(notification, SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));

		NotificationMap.insert(reply.value(), notification);
		IdQueue.enqueue(reply.value());
		QTimer::singleShot(config_file.readNumEntry("KDENotify", "Timeout", 10) * 1000 + 2000, this, SLOT(deleteMapItem()));
	}
}

void FreedesktopNotify::notificationClosed(Notification *notification)
{
	QMap<unsigned int, Notification *>::iterator i = NotificationMap.begin();
	while (i != NotificationMap.end())
	{
		if (i.value() == notification)
		{
			NotificationMap[i.key()] = 0;

			QList<QVariant> args;
			args.append(i.key());
			KNotify->callWithArgumentList(QDBus::Block, "CloseNotification", args);

			return;
		}
		++i;
	}
}

void FreedesktopNotify::actionInvoked(unsigned int id, QString action)
{
	if (!IdQueue.contains(id))
		return;

	Notification *notification = NotificationMap.value(id);
	if (!notification)
		return;

	const QMetaObject *metaObject = notification->metaObject();
	int slotIndex = -1;

	while (metaObject)
	{
		slotIndex = metaObject->indexOfSlot(action.toAscii().constData());
		if (slotIndex != -1)
			break;

		metaObject = metaObject->superClass();
	}

	if (-1 == slotIndex)
		return;

	QMetaMethod slot = notification->metaObject()->method(slotIndex);
	slot.invoke(notification, Qt::DirectConnection);

	QList<QVariant> args;
	args.append(id);
	KNotify->callWithArgumentList(QDBus::Block, "CloseNotification", args);

	NotificationMap[id] = 0;
}

void FreedesktopNotify::deleteMapItem()
{
	unsigned int id = IdQueue.dequeue();
	Notification *notification = NotificationMap.value(id);
	NotificationMap.remove(id);

	if (notification)
		notification->release();
}
