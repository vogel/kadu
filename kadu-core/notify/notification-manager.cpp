/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include <QtGui/QApplication>

#ifdef Q_WS_X11
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtGui/QX11Info>
#endif

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/main-window.h"
#include "gui/windows/message-dialog.h"
#include "message/message-manager.h"
#include "message/message.h"
#include "multilogon/multilogon-session.h"
#include "notify/account-notification.h"
#include "notify/buddy-notify-data.h"
#include "notify/multilogon-notification.h"
#include "notify/notification.h"
#include "notify/notifier.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/window-notifier.h"
#include "protocols/services/multilogon-service.h"
#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#include "misc/misc.h"
#include "activate.h"
#include "debug.h"

#include "new-message-notification.h"
#include "status-changed-notification.h"

#ifdef Q_WS_X11
#include "os/x11tools.h" // this should be included as last one,
#undef Status            // and Status defined by Xlib.h must be undefined
#endif

#define FULLSCREENCHECKTIMER_INTERVAL 2000 /*ms*/

NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();
		Instance->init();

		MessageNotification::registerEvents();
		StatusChangedNotification::registerEvents();
		MultilogonNotification::registerEvents();
	}

	return Instance;
}

NotificationManager::NotificationManager()
{
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	x11display = XOpenDisplay(0);
#endif
}

void NotificationManager::init()
{
	kdebugf();

	Notification::registerParserTags();
	AccountNotification::registerParserTags();

	UiHandler = new NotifyConfigurationUiHandler(this);
	MainConfigurationWindow::registerUiHandler(UiHandler);

	SilentMode = false;

	FullScreenCheckTimer.setInterval(FULLSCREENCHECKTIMER_INTERVAL);
	connect(&FullScreenCheckTimer, SIGNAL(timeout()), this, SLOT(checkFullScreen()));
	IsFullScreen = false;

	createDefaultConfiguration();
	AutoSilentMode = false;
	//TODO 0.10.0:
	//triggerAllAccountsRegistered();

	notifyAboutUserActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/notify-about-buddy"), tr("Notify About Buddy"), true,
		checkNotify
	);

	SilentModeActionDescription = new ActionDescription(this,
		ActionDescription::TypeGlobal, "silentModeAction",
		this, SLOT(silentModeActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/enable-notifications"), tr("Enable Notifications"), true
	);
	configurationUpdated();
	connect(SilentModeActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(silentModeActionCreated(Action *)));

	connect(MessageManager::instance(), SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));
	connect(StatusContainerManager::instance(), SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));

	foreach (const Group &group, GroupManager::instance()->items())
		groupAdded(group);

	new WindowNotifier(this);
	kdebugf2();
}

NotificationManager::~NotificationManager()
{
	kdebugf();

	FullScreenCheckTimer.stop();

	MainConfigurationWindow::unregisterUiHandler(UiHandler);

	StatusChangedNotification::unregisterEvents();
	MessageNotification::unregisterEvents();
	MultilogonNotification::unregisterEvents();

	triggerAllAccountsUnregistered();

	while (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		unregisterNotifier(Notifiers.at(0));
	}

	AccountNotification::unregisterParserTags();
	Notification::unregisterParserTags();

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	XCloseDisplay(x11display);
#endif

	kdebugf2();
}

void NotificationManager::setSilentMode(bool silentMode)
{
	if (silentMode != SilentMode)
	{
		SilentMode = silentMode;
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(!silentMode);

		config_file.writeEntry("Notify", "SilentMode", SilentMode);

		emit silentModeToggled(SilentMode);
	}
}

bool NotificationManager::silentMode()
{
	return SilentMode || (IsFullScreen && config_file.readBoolEntry("Notify", "FullscreenSilentMode", false));
}

void NotificationManager::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const BuddySet &buddies = action->context()->buddies();

	bool on = true;
	foreach (const Buddy &buddy, buddies)
		if (buddy.data())
		{
			BuddyNotifyData *bnd = buddy.data()->moduleStorableData<BuddyNotifyData>("notify", this, false);

			if (!bnd || !bnd->notify())
			{
				on = false;
				break;
			}
		}

	if (NotifyAboutAll)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	foreach (const Buddy &buddy, buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		BuddyNotifyData *bnd = buddy.data()->moduleStorableData<BuddyNotifyData>("notify", this, true);
		if (bnd->notify() == on)
		{
			bnd->setNotify(!on);
			bnd->ensureStored();
		}
	}

	foreach (Action *action, notifyAboutUserActionDescription->actions())
		if (action->context()->contacts().toBuddySet() == buddies)
			action->setChecked(!on);

	kdebugf2();
}

void NotificationManager::silentModeActionCreated(Action *action)
{
	action->setChecked(!SilentMode);
}

void NotificationManager::silentModeActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	setSilentMode(!toggled);
}

void NotificationManager::statusUpdated()
{
	if (SilentModeWhenDnD && !silentMode() && StatusContainerManager::instance()->status().type() == StatusTypeDoNotDisturb)
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(false);

		AutoSilentMode = true;
	}
	else if (!silentMode() && AutoSilentMode)
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(true);

		AutoSilentMode = false;
	}
}

void NotificationManager::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	connect(account, SIGNAL(connected()), this, SLOT(accountConnected()));

	MultilogonService *multilogonService = protocol->multilogonService();
	if (multilogonService)
	{
		connect(multilogonService, SIGNAL(multilogonSessionConnected(MultilogonSession*)),
				this, SLOT(multilogonSessionConnected(MultilogonSession*)));
		connect(multilogonService, SIGNAL(multilogonSessionDisconnected(MultilogonSession*)),
				this, SLOT(multilogonSessionDisconnected(MultilogonSession*)));
	}
}

void NotificationManager::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (!protocol)
		return;

	disconnect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	disconnect(account, SIGNAL(connected()), this, SLOT(accountConnected()));

	MultilogonService *multilogonService = protocol->multilogonService();
	if (multilogonService)
	{
		disconnect(multilogonService, SIGNAL(multilogonSessionConnected(MultilogonSession*)),
				this, SLOT(multilogonSessionConnected(MultilogonSession*)));
		disconnect(multilogonService, SIGNAL(multilogonSessionDisconnected(MultilogonSession*)),
				this, SLOT(multilogonSessionDisconnected(MultilogonSession*)));
	}
}

void NotificationManager::accountConnected()
{
	Account account(sender());
	if (!account)
		return;

	if (NotifyIgnoreOnConnection)
	{
		QDateTime *dateTime = account.data()->moduleData<QDateTime>("notify-account-connected", true);
		*dateTime = QDateTime::currentDateTime().addSecs(10);
	}
}

void NotificationManager::contactStatusChanged(Contact contact, Status oldStatus)
{
	kdebugf();

	if (contact.isAnonymous() || !contact.contactAccount())
		return;

	Protocol *protocol = contact.contactAccount().protocolHandler();
	if (!protocol || !protocol->isConnected())
		return;

	if (NotifyIgnoreOnConnection)
	{
		QDateTime *dateTime = contact.contactAccount().data()->moduleData<QDateTime>("notify-account-connected");
		if (dateTime && (*dateTime >= QDateTime::currentDateTime()))
			return;
	}

	bool notify_contact = true;
	BuddyNotifyData *bnd = 0;
	bnd = contact.ownerBuddy().data()->moduleStorableData<BuddyNotifyData>("notify", this, false);

	if (!bnd || !bnd->notify())
		notify_contact = false;

	if (!notify_contact && !NotifyAboutAll)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (contact == contact.contactAccount().accountContact()) // myself
		return;

	Status status = contact.currentStatus();
	if (oldStatus == status)
		return;

	if (IgnoreOnlineToOnline &&
			!status.isDisconnected() &&
			!oldStatus.isDisconnected())
		return;

	const StatusTypeData &typeData = StatusTypeManager::instance()->statusTypeData(status.type());
	QString changedTo = "/To" + typeData.name();

	StatusChangedNotification *statusChangedNotification = new StatusChangedNotification(changedTo, contact);

	notify(statusChangedNotification);

	kdebugf2();
}

void NotificationManager::messageReceived(const Message &message)
{
	kdebugf();

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(message.messageChat(), false);
	if (!chatWidget)
		notify(new MessageNotification(MessageNotification::NewChat, message));
	else if (!NewMessageOnlyIfInactive || !_isWindowActiveOrFullyVisible(chatWidget))
		notify(new MessageNotification(MessageNotification::NewMessage, message));

	kdebugf2();
}

void NotificationManager::multilogonSessionConnected(MultilogonSession *session)
{
	MultilogonNotification::notifyMultilogonSessionConnected(session);
}

void NotificationManager::multilogonSessionDisconnected(MultilogonSession *session)
{
	MultilogonNotification::notifyMultilogonSessionDisonnected(session);
}

void NotificationManager::registerNotifyEvent(NotifyEvent *notifyEvent)
{
	kdebugf();

	NotifyEvents.append(notifyEvent);
	emit notifyEventRegistered(notifyEvent);

	kdebugf2();
}

void NotificationManager::unregisterNotifyEvent(NotifyEvent *notifyEvent)
{
	kdebugf();

	NotifyEvents.removeAll(notifyEvent);
	emit notifyEventUnregistered(notifyEvent);

	kdebugf2();
}

void NotificationManager::registerNotifier(Notifier *notifier)
{
	kdebugf();

	if (Notifiers.contains(notifier))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' already exists in notifiers! "
		"strange... unregistering old Notifier\n", qPrintable(notifier->name()));

		unregisterNotifier(notifier);
	}

	Notifiers.append(notifier);
	emit notiferRegistered(notifier);

	kdebugf2();
}

void NotificationManager::unregisterNotifier(Notifier *notifier)
{
	kdebugf();

	if (!Notifiers.contains(notifier))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' not registered!\n", qPrintable(notifier->name()));
		return;
	}

	emit notiferUnregistered(notifier);
	Notifiers.removeAll(notifier);

	kdebugf2();
}

const QList<Notifier *> & NotificationManager::notifiers() const
{
	return Notifiers;
}

const QList<NotifyEvent *> & NotificationManager::notifyEvents() const
{
	return NotifyEvents;
}

bool NotificationManager::ignoreNotifications()
{
	if (silentMode())
		return true;

	if (AutoSilentMode)
		return true;

	return false;
}

void NotificationManager::notify(Notification *notification)
{
	kdebugf();

	QString notifyType = notification->key();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = !notification->requireCallback();

	if (ignoreNotifications())
	{
		notification->callbackDiscard();
		return;
	}

	notification->acquire();

	foreach (Notifier *notifier, Notifiers)
	{
		if (config_file.readBoolEntry("Notify", notifyType + '_' + notifier->name()))
		{
			notifier->notify(notification);
			foundNotifier = true;
			foundNotifierWithCallbackSupported = foundNotifierWithCallbackSupported ||
					(Notifier::CallbackSupported == notifier->callbackCapacity());
		}
	}

	if (!foundNotifierWithCallbackSupported)
		foreach (Notifier *notifier, Notifiers)
		{
			if (Notifier::CallbackSupported == notifier->callbackCapacity())
			{
				notifier->notify(notification);
				foundNotifier = true;
				foundNotifierWithCallbackSupported = true;
				break;
			}
		}

	if (!foundNotifier)
		notification->callbackDiscard();

	notification->release();

	if (!foundNotifierWithCallbackSupported)
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Unable to find notifier for %1 event").arg(notification->type()));

	kdebugf2();
}

void NotificationManager::groupAdded(const Group &group)
{
	connect(group, SIGNAL(updated()), this, SLOT(groupUpdated()));
}

void NotificationManager::groupUpdated()
{
	Group group = sender();
	if (group.isNull())
		return;

	bool notify = group.notifyAboutStatusChanges();

	if (NotifyAboutAll && !notify)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous() || buddy.groups().contains(group))
			continue;

		BuddyNotifyData *bnd = buddy.data()->moduleStorableData<BuddyNotifyData>("notify", this, true);
		bnd->setNotify(notify);
		bnd->ensureStored();
	}
}

void NotificationManager::configurationUpdated()
{
	NotifyAboutAll = config_file.readBoolEntry("Notify", "NotifyAboutAll");
	NewMessageOnlyIfInactive = config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive");
	NotifyIgnoreOnConnection = config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection");
	IgnoreOnlineToOnline = config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline");
	SilentModeWhenDnD = config_file.readBoolEntry("Notify", "AwaySilentMode", false);
	SilentModeWhenFullscreen = config_file.readBoolEntry("Notify", "FullscreenSilentMode", false);
	setSilentMode(config_file.readBoolEntry("Notify", "SilentMode", false));
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	if (SilentModeWhenFullscreen)
		FullScreenCheckTimer.start();
	else
	{
		FullScreenCheckTimer.stop();
		IsFullScreen = false;
	}
#endif
}

void NotificationManager::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "IgnoreOnlineToOnline", false);
	config_file.addVariable("Notify", "NewMessageOnlyIfInactive", true);
	config_file.addVariable("Notify", "NotifyAboutAll", true);
	config_file.addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

QString NotificationManager::notifyConfigurationKey(const QString &eventType)
{
	QString event = eventType;

	while (true)
	{
		int slashPosition = event.lastIndexOf('/');
		if (-1 == slashPosition)
			return event;

		if (config_file.readBoolEntry("Notify", event + "_UseCustomSettings", false))
			return event;

		event = event.left(slashPosition);
	}

	Q_ASSERT(false);
}

ConfigurationUiHandler * NotificationManager::configurationUiHandler()
{
	return UiHandler;
}

void NotificationManager::checkFullScreen()
{
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	bool wasSilent = silentMode();
	IsFullScreen = X11_checkFullScreen(x11display) && (!isScreenSaverRunning());
	if (silentMode() != wasSilent)
		emit silentModeToggled(silentMode());
#endif
}

bool NotificationManager::isScreenSaverRunning()
{
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	// org.freedesktop.ScreenSaver
	{
		QDBusInterface dbus("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", QDBusConnection::sessionBus());
		if (dbus.isValid())
		{
			QDBusReply<bool> reply = dbus.call("GetActive");
			if (reply.isValid() && reply.value())
				return true;
		}
	}
	// org.kde.screensaver
	{
		QDBusInterface dbus("org.kde.screensaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", QDBusConnection::sessionBus());
		if (dbus.isValid())
		{
			QDBusReply<bool> reply = dbus.call("GetActive");
			if (reply.isValid() && reply.value())
				return true;
		}
	}
	// org.gnome.ScreenSaver
	{
		QDBusInterface dbus("org.gnome.ScreenSaver", "/", "org.gnome.ScreenSaver", QDBusConnection::sessionBus());
		if (dbus.isValid())
		{
			QDBusReply<bool> reply = dbus.call("GetActive");
			if (reply.isValid() && reply.value())
				return true;
		}
	}
#endif
	// no screensaver
	return false;
}

void checkNotify(Action *action)
{
	kdebugf();

	action->setEnabled(!action->context()->buddies().isEmpty());

	bool on = true;
	foreach (const Buddy &buddy, action->context()->contacts().toBuddySet())
		if (buddy.data())
		{
			BuddyNotifyData *bnd = buddy.data()->moduleStorableData<BuddyNotifyData>("notify", NotificationManager::instance(), false);
			if (!bnd || !bnd->notify())
			{
				on = false;
				break;
			}
		}

	action->setChecked(on);

	kdebugf2();
}
