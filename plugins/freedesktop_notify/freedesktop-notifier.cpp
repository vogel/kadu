/*
 * Copyright 2009 Jacek Jabłoński
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "dom/dom-processor-service.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/paths-provider.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"
#include "url-handlers/url-handler-manager.h"

#include "freedesktop-notifier.h"

FreedesktopNotifier::FreedesktopNotifier(QObject *parent) :
		QObject{parent},
		Notifier("FreedesktopNotifier", QT_TRANSLATE_NOOP("@default", "System notifications"), KaduIcon("kadu_icons/notify-hints")),
		KdePlasmaNotifications(true), IsXCanonicalAppendSupported(false), ServerSupportsActions(true), ServerSupportsBody(true),
		ServerSupportsHyperlinks(true), ServerSupportsMarkup(true), ServerCapabilitiesRequireChecking(true)
{
}

FreedesktopNotifier::~FreedesktopNotifier()
{
	delete NotificationsInterface;
	NotificationsInterface = 0;
}

void FreedesktopNotifier::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void FreedesktopNotifier::setDomProcessorService(DomProcessorService *domProcessorService)
{
	m_domProcessorService = domProcessorService;
}

void FreedesktopNotifier::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void FreedesktopNotifier::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void FreedesktopNotifier::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void FreedesktopNotifier::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void FreedesktopNotifier::init()
{
	StripBr.setPattern(QStringLiteral("<br ?/?>"));
	StripHtml.setPattern(QStringLiteral("<[^>]*>"));
	// this is meant to catch all HTML tags except <b>, <i>, <u>
	StripUnsupportedHtml.setPattern(QStringLiteral("<(/?[^/<>][^<>]+|//[^>]*|/?[^biu])>"));

	DesktopEntry = QFileInfo(m_pathsProvider->desktopFilePath()).baseName();
	NotificationsInterface = new QDBusInterface("org.freedesktop.Notifications",
			"/org/freedesktop/Notifications", "org.freedesktop.Notifications");

	QDBusServiceWatcher *watcher = new QDBusServiceWatcher(this);
	watcher->setConnection(QDBusConnection::sessionBus());
	watcher->setWatchMode(QDBusServiceWatcher::WatchForOwnerChange);
	watcher->addWatchedService("org.freedesktop.Notifications");

	connect(watcher, SIGNAL(serviceOwnerChanged(QString,QString,QString)),
			SLOT(slotServiceOwnerChanged(QString,QString,QString)));

	NotificationsInterface->connection().connect(NotificationsInterface->service(), NotificationsInterface->path(), NotificationsInterface->interface(),
			"ActionInvoked", this, SLOT(actionInvoked(unsigned int, QString)));
	NotificationsInterface->connection().connect(NotificationsInterface->service(), NotificationsInterface->path(), NotificationsInterface->interface(),
			"NotificationClosed", this, SLOT(notificationClosed(unsigned int, unsigned int)));

	import_0_9_0_Configuration();
	createDefaultConfiguration();

	configurationUpdated();
}

void FreedesktopNotifier::checkServerCapabilities()
{
	if (!ServerCapabilitiesRequireChecking)
		return;

	QDBusMessage replyMsg = NotificationsInterface->call(QDBus::Block, "GetServerInformation");
	if (replyMsg.type() != QDBusMessage::ReplyMessage)
		KdePlasmaNotifications = false;
	else
		KdePlasmaNotifications = replyMsg.arguments().at(0).toString().contains("Plasma") && replyMsg.arguments().at(1).toString().contains("KDE");

	replyMsg = NotificationsInterface->call(QDBus::Block, "GetCapabilities");
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

void FreedesktopNotifier::notify(const Notification &notification)
{
	checkServerCapabilities();

	bool useKdeStyle = KdePlasmaNotifications && ServerSupportsBody && ServerSupportsMarkup;

	QList<QVariant> args;
	args.append("Kadu");

	unsigned int replacedNotificationId = 0U;
	unsigned int notificationUid = NotificationMap.key(notification);

	if (notificationUid)
		replacedNotificationId = notificationUid;

	args.append(replacedNotificationId);

	KaduIcon icon(notification.icon);
	if (icon.isNull())
	{
		icon.setPath("kadu_icons/section-kadu");
		icon.setSize("32x32");
	}
	else
		icon.setSize("64x64");

	args.append(m_iconsManager->iconPath(icon));

	QString summary;
	if (useKdeStyle)
		summary = "Kadu";
	else
	{
		summary = notification.text;
		summary.replace(StripBr, QStringLiteral(" "));
		summary.remove(StripHtml);
	}

	args.append(summary);

	bool typeNewMessage = (notification.type == "NewMessage" || notification.type == "NewChat");
	QString body;
	if (ServerSupportsBody)
	{
		if (!typeNewMessage || ShowContentMessage)
		{
			body = notification.details;
			body.replace(StripBr, QStringLiteral("\n"));
			if (ServerSupportsMarkup)
				body.remove(StripUnsupportedHtml);
			else
				body.remove(StripHtml);

			if (body.length() > CiteSign)
				body = body.left(CiteSign) + QStringLiteral("...");
		}

		if (useKdeStyle)
		{
			if (body.isEmpty())
				body = notification.text;
			else
			{
				body.prepend(notification.text + "\n<small>");
				body.append("</small>");
			}

			if (body.length() > CiteSign)
				body = body.left(CiteSign) + QStringLiteral("...");
		}

		if (ServerSupportsHyperlinks && !body.isEmpty())
			body = m_domProcessorService->process(body);
	}

	args.append(body);

	QStringList actions;
	if (ServerSupportsActions)
	{
		for (auto &&callbackName : notification.callbacks)
		{
			auto callback = m_notificationCallbackRepository->callback(callbackName);
			actions << callbackName;
			actions << callback.title();
		}
	}
	args.append(actions);

	QVariantMap hints;
	hints.insert("category", typeNewMessage ? "im.received" : "im");
	hints.insert("desktop-entry", DesktopEntry);
	if (IsXCanonicalAppendSupported && !useKdeStyle)
		hints.insert("x-canonical-append", "allowed");
	args.append(hints);

	// -1 is server default
	args.append(CustomTimeout ? Timeout * 1000 : -1);

	QDBusReply<unsigned int> reply = NotificationsInterface->callWithArgumentList(QDBus::Block, "Notify", args);
	if (reply.isValid())
	{
		NotificationMap.insert(reply.value(), notification);
	}
}

void FreedesktopNotifier::notificationClosed(unsigned int id, unsigned int reason)
{
	Q_UNUSED(reason);

	NotificationMap.remove(id);
}

void FreedesktopNotifier::slotServiceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
	Q_UNUSED(serviceName)
	Q_UNUSED(oldOwner)
	Q_UNUSED(newOwner)

	NotificationMap.clear();
	ServerCapabilitiesRequireChecking = true;
}

void FreedesktopNotifier::actionInvoked(unsigned int id, QString callbackName)
{
	if (!NotificationMap.contains(id))
		return;

	auto notification = NotificationMap.take(id);
	auto callback = m_notificationCallbackRepository->callback(callbackName);
	callback.call(notification);

	QList<QVariant> args;
	args.append(id);
	NotificationsInterface->callWithArgumentList(QDBus::Block, "CloseNotification", args);
}

void FreedesktopNotifier::configurationUpdated()
{
	CustomTimeout = m_configuration->deprecatedApi()->readBoolEntry("FreedesktopNotifier", "CustomTimeout");
	Timeout = m_configuration->deprecatedApi()->readNumEntry("FreedesktopNotifier", "Timeout");
	ShowContentMessage = m_configuration->deprecatedApi()->readBoolEntry("FreedesktopNotifier", "ShowContentMessage");
	CiteSign = m_configuration->deprecatedApi()->readNumEntry("FreedesktopNotifier", "CiteSign");
}

void FreedesktopNotifier::import_0_9_0_Configuration()
{
	m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "Timeout", m_configuration->deprecatedApi()->readEntry("KDENotify", "Timeout"));
	m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "ShowContentMessage", m_configuration->deprecatedApi()->readEntry("KDENotify", "ShowContentMessage"));
	m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "CiteSign", m_configuration->deprecatedApi()->readEntry("KDENotify", "CiteSign"));
	if (!m_configuration->deprecatedApi()->readEntry("KDENotify", "Timeout").isEmpty() || !m_configuration->deprecatedApi()->readEntry("FreedesktopNotifier", "Timeout").isEmpty())
		m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "CustomTimeout", true);

	for (auto &&event : m_notificationEventRepository->notificationEvents())
		m_configuration->deprecatedApi()->addVariable("Notify", event.name() + "_FreedesktopNotifier", m_configuration->deprecatedApi()->readEntry("Notify", event.name() + "_KNotify"));
}

void FreedesktopNotifier::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Notify", "ConnectionError_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "NewChat_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "NewMessage_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToFreeForChat_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOnline_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToAway_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToNotAvailable_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToDoNotDisturb_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOffline_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/IncomingFile_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/Finished_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "multilogon_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "multilogon/sessionConnected_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "multilogon/sessionDisconnected_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_UseCustomSettings", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_FreedesktopNotifier", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_UseCustomSettings", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_FreedesktopNotifier", true);

	m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "CustomTimeout", false);
	m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "Timeout", 10);
	m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "ShowContentMessage", true);
	m_configuration->deprecatedApi()->addVariable("FreedesktopNotifier", "CiteSign", 100);
}

#include "moc_freedesktop-notifier.cpp"
