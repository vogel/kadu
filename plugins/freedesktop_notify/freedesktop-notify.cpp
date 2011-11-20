/*
 * Copyright 2009 Jacek Jabłoński
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtCore/QMetaMethod>
#include <QtCore/QTimer>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusServiceWatcher>

#include "configuration/configuration-file.h"
#include "icons/kadu-icon.h"
#include "misc/path-conversion.h"
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
		KdePlasmaNotifications(true), IsXCanonicalAppendSupported(false), UseFreedesktopStandard(false), ServerSupportsActions(true), ServerSupportsBody(true),
		ServerSupportsHyperlinks(true), ServerSupportsMarkup(true), ServerCapabilitiesRequireChecking(false)
{
	StripBr.setPattern(QLatin1String("<br ?/?>"));
	StripHtml.setPattern(QLatin1String("<[^>]*>"));
	// this is meant to catch all HTML tags except <b>, <i>, <u>
	StripUnsupportedHtml.setPattern(QLatin1String("<(/?[^/<>][^<>]+|//[^>]*|/?[^biu])>"));

	DesktopEntry = QFileInfo(desktopFilePath()).baseName();

	KNotify = new QDBusInterface("org.kde.VisualNotifications",
			"/VisualNotifications", "org.kde.VisualNotifications");

	if (!KNotify->isValid())
	{
		delete KNotify;
		KNotify = new QDBusInterface("org.freedesktop.Notifications",
				"/org/freedesktop/Notifications", "org.freedesktop.Notifications");

		QDBusServiceWatcher *watcher = new QDBusServiceWatcher(this);
		watcher->setConnection(QDBusConnection::sessionBus());
		watcher->setWatchMode(QDBusServiceWatcher::WatchForOwnerChange);
		watcher->addWatchedService("org.freedesktop.Notifications");

		connect(watcher, SIGNAL(serviceOwnerChanged(const QString &, const QString &, const QString &)),
				SLOT(slotServiceOwnerChanged(const QString &, const QString &, const QString &)));

		UseFreedesktopStandard = true;
		ServerCapabilitiesRequireChecking = true;
	}

	KNotify->connection().connect(KNotify->service(), KNotify->path(), KNotify->interface(),
			"ActionInvoked", this, SLOT(actionInvoked(unsigned int, QString)));

	import_0_9_0_Configuration();
	createDefaultConfiguration();

	configurationUpdated();

	NotificationManager::instance()->registerNotifier(this);
}

FreedesktopNotify::~FreedesktopNotify()
{
	NotificationManager::instance()->unregisterNotifier(this);

	delete KNotify;
	KNotify = 0;
}

void FreedesktopNotify::checkServerCapabilities()
{
	if (!ServerCapabilitiesRequireChecking)
		return;

	QDBusMessage replyMsg = KNotify->call(QDBus::Block, "GetServerInformation");
	if (replyMsg.type() != QDBusMessage::ReplyMessage)
		KdePlasmaNotifications = false;
	else
		KdePlasmaNotifications = replyMsg.arguments().at(0).toString().contains("Plasma") && replyMsg.arguments().at(1).toString().contains("KDE");

	replyMsg = KNotify->call(QDBus::Block, "GetCapabilities");
	if (replyMsg.type() != QDBusMessage::ReplyMessage)
	{
		IsXCanonicalAppendSupported = false;
		ServerSupportsActions = false;
		ServerSupportsBody = false;
		ServerSupportsHyperlinks = false;
		ServerSupportsMarkup = false;
	}
	else
	{
		QStringList capabilities = replyMsg.arguments().at(0).toStringList();

		IsXCanonicalAppendSupported = capabilities.contains("x-canonical-append");
		ServerSupportsActions = capabilities.contains("actions");
		ServerSupportsBody = capabilities.contains("body");
		ServerSupportsHyperlinks = capabilities.contains("body-hyperlinks");
		ServerSupportsMarkup = capabilities.contains("body-markup");

		ServerCapabilitiesRequireChecking = false;
	}
}

Notifier::CallbackCapacity FreedesktopNotify::callbackCapacity()
{
	checkServerCapabilities();

	return ServerSupportsActions ? CallbackSupported : CallbackNotSupported;
}

void FreedesktopNotify::notify(Notification *notification)
{
	checkServerCapabilities();

	bool useKdeStyle = KdePlasmaNotifications && ServerSupportsBody && ServerSupportsMarkup;

	QList<QVariant> args;
	args.append("Kadu");
	args.append(0U);

	KaduIcon icon(notification->icon());
	if (icon.isNull())
	{
		icon.setPath("kadu_icons/section-kadu");
		icon.setSize("32x32");
	}
	else
		icon.setSize("64x64");
	args.append(icon.fullPath());

	// the new spec doesn't have this
	if (!UseFreedesktopStandard)
		args.append(QString());

	QString summary;
	if (useKdeStyle)
		summary = "Kadu";
	else
	{
		summary = notification->text();
		summary.replace(StripBr, QLatin1String(" "));
		summary.remove(StripHtml);
	}

	args.append(summary);

	bool msgRcv = (notification->type() == "NewMessage" || notification->type() == "NewChat");
	QString body;
	if (ServerSupportsBody)
	{
		if (!msgRcv || ShowContentMessage)
		{
			body = notification->details();
			body.replace(StripBr, QLatin1String("\n"));
			if (ServerSupportsMarkup)
				body.remove(StripUnsupportedHtml);
			else
				body.remove(StripHtml);

			if (body.length() > CiteSign)
				body = body.left(CiteSign) + QLatin1String("...");
		}

		if (useKdeStyle)
		{
			if (body.isEmpty())
				body = notification->text();
			else
			{
				body.prepend(notification->text() + "\n<small>");
				body.append("</small>");
			}
		}

		if (ServerSupportsHyperlinks && !body.isEmpty())
		{
			HtmlDocument doc;
			doc.parseHtml(body);
			UrlHandlerManager::instance()->convertAllUrls(doc, true);

			body = doc.generateHtml();
		}
	}

	args.append(body);

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

	QVariantMap hints;
	hints.insert("category", msgRcv ? "im.received" : "im");
	hints.insert("desktop-entry", DesktopEntry);
	if (IsXCanonicalAppendSupported && !useKdeStyle)
		hints.insert("x-canonical-append", "allowed");
	args.append(hints);

	// -1 is server default
	args.append(CustomTimeout ? Timeout * 1000 : -1);

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
		Notification *notification = NotificationMap.take(id);

		if (notification)
			notification->release();
	}

	ServerCapabilitiesRequireChecking = true;
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
	Notification *notification = NotificationMap.take(id);

	if (notification)
		notification->release();
}

void FreedesktopNotify::configurationUpdated()
{
	CustomTimeout = config_file.readBoolEntry("FreedesktopNotify", "CustomTimeout");
	Timeout = config_file.readNumEntry("FreedesktopNotify", "Timeout");
	ShowContentMessage = config_file.readBoolEntry("FreedesktopNotify", "ShowContentMessage");
	CiteSign = config_file.readNumEntry("FreedesktopNotify", "CiteSign");
}

void FreedesktopNotify::import_0_9_0_Configuration()
{
	config_file.addVariable("FreedesktopNotify", "Timeout", config_file.readEntry("KDENotify", "Timeout"));
	config_file.addVariable("FreedesktopNotify", "ShowContentMessage", config_file.readEntry("KDENotify", "ShowContentMessage"));
	config_file.addVariable("FreedesktopNotify", "CiteSign", config_file.readEntry("KDENotify", "CiteSign"));
	if (!config_file.readEntry("KDENotify", "Timeout").isEmpty() || !config_file.readEntry("FreedesktopNotify", "Timeout").isEmpty())
		config_file.addVariable("FreedesktopNotify", "CustomTimeout", true);

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

	config_file.addVariable("FreedesktopNotify", "CustomTimeout", false);
	config_file.addVariable("FreedesktopNotify", "Timeout", 10);
	config_file.addVariable("FreedesktopNotify", "ShowContentMessage", true);
	config_file.addVariable("FreedesktopNotify", "CiteSign", 100);
}
