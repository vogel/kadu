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
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "url-handlers/url-handler-manager.h"
#include "debug.h"
#include "html_document.h"

#include <QDBusReply>
#include <QTimer>

#include "kde_notify.h"

extern "C" KADU_EXPORT int kde_notify_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	kde_notify = new KdeNotify();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/kde_notify.ui"));
	MainConfigurationWindow::registerUiHandler(kde_notify);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void kde_notify_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/kde_notify.ui"));
	MainConfigurationWindow::unregisterUiHandler(kde_notify);
	delete kde_notify;
	kde_notify = 0;

	kdebugf2();
}

KdeNotify::KdeNotify(QObject *parent) :
		Notifier("KNotify", tr("KDE4 notifications"), "kadu_icons/notify-hints", parent),
		UseFreedesktopStandard(false)
{
	kdebugf();

	StripHTML.setPattern(QString::fromLatin1("<.*>"));
	StripHTML.setMinimal(true);

	KNotify = new QDBusInterface("org.kde.VisualNotifications",
			"/VisualNotifications", "org.kde.VisualNotifications");

	/* Dorr: maybe we're using patched version of KDE */
	if (!KNotify->isValid())
	{
		delete (KNotify);
		KNotify = new QDBusInterface("org.freedesktop.Notifications",
				"/org/freedesktop/Notifications", "org.freedesktop.Notifications");

		UseFreedesktopStandard = true;
	}

	KNotify->connection().connect(KNotify->service(), KNotify->path(), KNotify->interface(),
		"ActionInvoked", this, SLOT(actionInvoked(unsigned int, QString)));

	NotificationManager::instance()->registerNotifier(this);
	createDefaultConfiguration();

	kdebugf2();
}

KdeNotify::~KdeNotify()
{
	kdebugf();

	NotificationManager::instance()->unregisterNotifier(this);
	delete KNotify;
	KNotify = 0;

	kdebugf2();
}

void KdeNotify::createDefaultConfiguration()
{
	config_file.addVariable("KDENotify", "Timeout", 10);
	config_file.addVariable("KDENotify", "ShowContentMessage", true);
	config_file.addVariable("KDENotify", "CiteSign", 100);
}

void KdeNotify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("kdenotify/showContent"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("kdenotify/showContentCount"), SLOT(setEnabled(bool)));
}

void KdeNotify::notify(Notification *notification)
{
	QList<QVariant> args;
	args.append("Kadu");
	args.append(0U);
	args.append(IconsManager::instance()->iconPath("kadu_icons/section-kadu", "32x32"));

	/* the new spec doesn't have this */
	if (!UseFreedesktopStandard)
		args.append(QString());

  	args.append("Kadu");

	QString text;
	if (!notification->iconPath().isEmpty())
		text = QString("<img src=\"%1\" alt=\"icon\" align=middle> ").arg(notification->iconPath().remove("file://"));

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

void KdeNotify::notificationClosed(Notification *notification)
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

void KdeNotify::actionInvoked(unsigned int id, QString action)
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

void KdeNotify::deleteMapItem()
{
	unsigned int id = IdQueue.dequeue();
	Notification *notification = NotificationMap.value(id);
	NotificationMap.remove(id);

	if (notification)
		notification->release();
}

KdeNotify *kde_notify = 0;
