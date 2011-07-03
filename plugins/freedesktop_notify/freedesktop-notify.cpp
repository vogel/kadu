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
#include <QtCore/QTimer>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusServiceWatcher>

#include "configuration/configuration-file.h"
#include "icons/kadu-icon.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "notify/notify-event.h"
#include "url-handlers/url-handler-manager.h"
#include "html_document.h"

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
		Notifier("FreedesktopNotify", QT_TRANSLATE_NOOP("@default", "System notifications"), KaduIcon("kadu_icons/notify-hints")),
		UseFreedesktopStandard(false), ServerSupportsActions(true), ServerSupportsHtml(true), ServerCapabilitesReqiuresChecking(true)
{
	StripHTML.setPattern(QString::fromLatin1("<.*>"));
	StripHTML.setMinimal(true);

	KNotify = new QDBusInterface("org.kde.VisualNotifications",
			"/VisualNotifications", "org.kde.VisualNotifications");

	if (!KNotify->isValid())
	{
		delete (KNotify);
		KNotify = new QDBusInterface("org.freedesktop.Notifications",
				"/org/freedesktop/Notifications", "org.freedesktop.Notifications");

		QDBusServiceWatcher *watcher = new QDBusServiceWatcher(this);
		watcher->setConnection(QDBusConnection::sessionBus());
		watcher->setWatchMode(QDBusServiceWatcher::WatchForOwnerChange);
		watcher->addWatchedService("org.freedesktop.Notifications");

		connect(watcher, SIGNAL(serviceOwnerChanged(const QString &, const QString &, const QString &)),
				SLOT(slotServiceOwnerChanged(const QString &, const QString &, const QString &)));

		UseFreedesktopStandard = true;
	}

	KNotify->connection().connect(KNotify->service(), KNotify->path(), KNotify->interface(),
		"ActionInvoked", this, SLOT(actionInvoked(unsigned int, QString)));

	configurationUpdated();

	NotificationManager::instance()->registerNotifier(this);

	import_0_9_0_Configuration();
	createDefaultConfiguration();
}

FreedesktopNotify::~FreedesktopNotify()
{
	NotificationManager::instance()->unregisterNotifier(this);

	delete KNotify;
	KNotify = 0;
}

void FreedesktopNotify::checkServerCapabilities()
{
	if (ServerCapabilitesReqiuresChecking)
	{
		QDBusMessage replyMsg = KNotify->call(QDBus::Block, "GetCapabilities");

		if (replyMsg.type() != QDBusMessage::ReplyMessage)
		{
			ServerSupportsActions = false;
			ServerSupportsHtml = false;
		}

		QStringList capabilities = replyMsg.arguments().at(0).toStringList();

		ServerSupportsActions = capabilities.contains("actions");
		ServerSupportsHtml = capabilities.contains("body-markup");

		ServerCapabilitesReqiuresChecking = false;
	}
}

void FreedesktopNotify::notify(Notification *notification)
{
	checkServerCapabilities();

	QList<QVariant> args;
	args.append("Kadu");
	args.append(0U);

	KaduIcon icon(notification->icon());
	if (icon.isNull())
		icon.setPath("kadu_icons/section-kadu");
	icon.setSize("64x64");
	args.append(icon.fullPath());

	// the new spec doesn't have this
	if (!UseFreedesktopStandard)
		args.append(QString());

  	args.append("Kadu");

	QString text;

	if (((notification->type() == "NewMessage") || (notification->type() == "NewChat")) && ShowContentMessage)
	{
		text.append(notification->text() + (ServerSupportsHtml ? "<br/><small>" : "\n"));

		QString strippedDetails = QString(notification->details()).replace("<br/>", "\n").remove(StripHTML);
		if (ServerSupportsHtml)
			strippedDetails.replace('\n', QLatin1String("<br/>"));

		if (strippedDetails.length() > CiteSign)
			text.append(strippedDetails.left(CiteSign) + "...");
		else
			text.append(strippedDetails);

		if (ServerSupportsHtml)
			text.append("</small>");
	}
	else
		text.append(notification->text());

	if (ServerSupportsHtml)
	{
		HtmlDocument doc;
		doc.parseHtml(text);
		UrlHandlerManager::instance()->convertAllUrls(doc);

		args.append(doc.generateHtml());
	}
	else
		args.append(text);

	QStringList actions;

	if (ServerSupportsActions)
	{
		foreach (const Notification::Callback &callback, notification->getCallbacks())
		{
			actions << callback.Signature;
			actions << callback.Caption;
		}
	}
	args.append(actions);
	args.append(QVariantMap());
	args.append(Timeout * 1000);

	QDBusReply<unsigned int> reply = KNotify->callWithArgumentList(QDBus::Block, "Notify", args);
	if (reply.isValid())
	{
		notification->acquire(); // do not remove now

		connect(notification, SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));

		NotificationMap.insert(reply.value(), notification);
		IdQueue.enqueue(reply.value());
		QTimer::singleShot(Timeout * 1000 + 2000, this, SLOT(deleteMapItem()));
	}
}

void FreedesktopNotify::notificationClosed(Notification *notification)
{
	QMap<unsigned int, Notification *>::const_iterator i = NotificationMap.constBegin();
	while (i != NotificationMap.constEnd())
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

void FreedesktopNotify::slotServiceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
	Q_UNUSED(serviceName)
	Q_UNUSED(oldOwner)
	Q_UNUSED(newOwner)

	while (!IdQueue.isEmpty())
	{
		unsigned int id = IdQueue.dequeue();
		Notification *notification = NotificationMap.value(id);
		NotificationMap.remove(id);

		if (notification)
			notification->release();
	}

	ServerCapabilitesReqiuresChecking = true;
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
	if (IdQueue.isEmpty())
		return;

	unsigned int id = IdQueue.dequeue();
	Notification *notification = NotificationMap.value(id);
	NotificationMap.remove(id);

	if (notification)
		notification->release();
}

void FreedesktopNotify::configurationUpdated()
{
	Timeout = config_file.readNumEntry("FreedesktopNotify", "Timeout");
	ShowContentMessage = config_file.readBoolEntry("FreedesktopNotify", "ShowContentMessage");
	CiteSign = config_file.readNumEntry("FreedesktopNotify", "CiteSign");
}

void FreedesktopNotify::import_0_9_0_Configuration()
{
	config_file.addVariable("FreedesktopNotify", "Timeout", config_file.readEntry("KDENotify", "Timeout"));
	config_file.addVariable("FreedesktopNotify", "ShowContentMessage", config_file.readEntry("KDENotify", "ShowContentMessage"));
	config_file.addVariable("FreedesktopNotify", "CiteSign", config_file.readEntry("KDENotify", "CiteSign"));

	foreach (NotifyEvent *event, NotificationManager::instance()->notifyEvents())
		config_file.addVariable("Notify", event->name() + "_FreedesktopNotify", config_file.readEntry("Notify", event->name() + "_KNotify"));
}

void FreedesktopNotify::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "ConnectionError_FreedesktopNotify", true);
	config_file.addVariable("Notify", "NewChat_FreedesktopNotify", true);
	config_file.addVariable("Notify", "NewMessage_FreedesktopNotify", true);
	config_file.addVariable("Notify", "StatusChanged_FreedesktopNotify", true);
	config_file.addVariable("Notify", "StatusChanged/ToFreeForChat_FreedesktopNotify", true);
	config_file.addVariable("Notify", "StatusChanged/ToOnline_FreedesktopNotify", true);
	config_file.addVariable("Notify", "StatusChanged/ToAway_FreedesktopNotify", true);
	config_file.addVariable("Notify", "StatusChanged/ToNotAvailable_FreedesktopNotify", true);
	config_file.addVariable("Notify", "StatusChanged/ToDoNotDisturb_FreedesktopNotify", true);
	config_file.addVariable("Notify", "StatusChanged/ToOffline_FreedesktopNotify", true);
	config_file.addVariable("Notify", "FileTransfer_FreedesktopNotify", true);
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_FreedesktopNotify", true);
	config_file.addVariable("Notify", "FileTransfer/Finished_FreedesktopNotify", true);
	config_file.addVariable("Notify", "multilogon_FreedesktopNotify", true);
	config_file.addVariable("Notify", "multilogon/sessionConnected_FreedesktopNotify", true);
	config_file.addVariable("Notify", "multilogon/sessionDisconnected_FreedesktopNotify", true);

	config_file.addVariable("FreedesktopNotify", "Timeout", 10);
	config_file.addVariable("FreedesktopNotify", "ShowContentMessage", true);
	config_file.addVariable("FreedesktopNotify", "CiteSign", 100);
}
