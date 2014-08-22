/*
 * %kadu copyright begin%
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "configuration/configuration-file.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "notify/listener/account-event-listener.h"
#include "notify/listener/chat-event-listener.h"
#include "notify/listener/group-event-listener.h"
#include "notify/notification/account-notification.h"
#include "notify/notification/multilogon-notification.h"
#include "notify/notification/new-message-notification.h"
#include "notify/notification/notification.h"
#include "notify/notification/status-changed-notification.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/window-notifier.h"
#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#if defined(Q_WS_X11)
#include "notify/x11-screen-mode-checker.h"
#elif defined(Q_OS_WIN32)
#include "notify/windows-screen-mode-checker.h"
#else
#include "notify/screen-mode-checker.h"
#endif

#include "notification-service.h"

NotificationService::NotificationService(QObject *parent) :
		QObject(parent), SilentMode(false), AutoSilentMode(false), IsFullScreen(false), FullscreenChecker(0)
{
	Notification::registerParserTags();
	AccountNotification::registerParserTags();

	NotifyUiHandler = new NotifyConfigurationUiHandler(this);
	MainConfigurationWindow::registerUiHandler(NotifyUiHandler);

	MessageNotification::registerEvents();
	StatusChangedNotification::registerEvents();
	MultilogonNotification::registerEvents();

	connect(StatusContainerManager::instance(), SIGNAL(statusUpdated(StatusContainer *)), this, SLOT(statusUpdated(StatusContainer *)));

	createEventListeners();
	createActionDescriptions();

	createDefaultConfiguration();
	configurationUpdated();

	new WindowNotifier(this);
}

NotificationService::~NotificationService()
{

	AccountNotification::unregisterParserTags();
	Notification::unregisterParserTags();

	MainConfigurationWindow::unregisterUiHandler(NotifyUiHandler);

	StatusChangedNotification::unregisterEvents();
	MessageNotification::unregisterEvents();
	MultilogonNotification::unregisterEvents();
}

void NotificationService::createActionDescriptions()
{
	notifyAboutUserActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/notify-about-buddy"), tr("Notify About Buddy"), true,
		checkNotify
	);

	MenuInventory::instance()
		->menu("buddy-list")
		->addAction(notifyAboutUserActionDescription, KaduMenu::SectionActions);

	SilentModeActionDescription = new ActionDescription(this,
		ActionDescription::TypeGlobal, "silentModeAction",
		this, SLOT(silentModeActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/enable-notifications"), tr("Silent Mode"), true
	);

	connect(SilentModeActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(silentModeActionCreated(Action *)));

	MenuInventory::instance()
		->menu("main")
		->addAction(SilentModeActionDescription, KaduMenu::SectionMiscTools, 5);
}

void NotificationService::createEventListeners()
{
	ChatListener = new ChatEventListener(this);
	AccountListener = new AccountEventListener(this);
	GroupListener = new GroupEventListener(this);
}

void NotificationService::statusUpdated(StatusContainer *container)
{
	if (SilentModeWhenDnD && !silentMode() && container->status().type() == StatusTypeDoNotDisturb)
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(true);

		AutoSilentMode = true;
	}
	else if (!silentMode() && AutoSilentMode)
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(false);

		AutoSilentMode = false;
	}
}

void NotificationService::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const BuddySet &buddies = action->context()->buddies();

	foreach (const Buddy &buddy, buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		if (toggled)
			buddy.removeProperty("notify:Notify");
		else
			buddy.addProperty("notify:Notify", false, CustomProperties::Storable);
	}

	foreach (Action *action, notifyAboutUserActionDescription->actions())
		if (action->context()->contacts().toBuddySet() == buddies)
			action->setChecked(toggled);
}

void NotificationService::silentModeActionCreated(Action *action)
{
	action->setChecked(SilentMode);
}

void NotificationService::silentModeActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	setSilentMode(toggled);
}

void NotificationService::setSilentMode(bool newSilentMode)
{
	if (newSilentMode == SilentMode)
		return;

	bool wasSilent = silentMode();
	SilentMode = newSilentMode;
	foreach (Action *action, SilentModeActionDescription->actions())
		action->setChecked(SilentMode);

	config_file.writeEntry("Notify", "SilentMode", SilentMode);

	if (silentMode() != wasSilent)
		emit silentModeToggled(silentMode());
}

bool NotificationService::silentMode()
{
	return SilentMode || (IsFullScreen && config_file.readBoolEntry("Notify", "FullscreenSilentMode", false));
}

bool NotificationService::ignoreNotifications()
{
	return AutoSilentMode || silentMode();
}

void NotificationService::configurationUpdated()
{
	NewMessageOnlyIfInactive = config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive");
	NotifyIgnoreOnConnection = config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection");
	IgnoreOnlineToOnline = config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline");
	SilentModeWhenDnD = config_file.readBoolEntry("Notify", "AwaySilentMode", false);
	SilentModeWhenFullscreen = config_file.readBoolEntry("Notify", "FullscreenSilentMode", false);
	setSilentMode(config_file.readBoolEntry("Notify", "SilentMode", false));

	if (SilentModeWhenFullscreen)
		startScreenModeChecker();
	else
		stopScreenModeChecker();
}

void NotificationService::startScreenModeChecker()
{
	if (FullscreenChecker)
		return;

#if defined(Q_WS_X11)
	FullscreenChecker = new X11ScreenModeChecker();
#elif defined(Q_OS_WIN32)
	FullscreenChecker = new WindowsScreenModeChecker();
#else
	FullscreenChecker = new ScreenModeChecker();
#endif
	connect(FullscreenChecker, SIGNAL(fullscreenToggled(bool)), this, SLOT(fullscreenToggled(bool)));

	FullscreenChecker->enable();
}

void NotificationService::stopScreenModeChecker()
{
	if (!FullscreenChecker)
		return;

	disconnect(FullscreenChecker, SIGNAL(fullscreenToggled(bool)), this, SLOT(fullscreenToggled(bool)));

	FullscreenChecker->disable();
}

void NotificationService::fullscreenToggled(bool inFullscreen)
{
	bool wasSilent = silentMode();
	IsFullScreen = inFullscreen;

	if (silentMode() != wasSilent)
		emit silentModeToggled(silentMode());
}

void NotificationService::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "IgnoreOnlineToOnline", false);
	config_file.addVariable("Notify", "NewMessageOnlyIfInactive", true);
	config_file.addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

void NotificationService::notify(Notification* notification)
{
	if (!ignoreNotifications())
		NotificationManager::instance()->notify(notification);
	else
		notification->callbackDiscard();
}

void checkNotify(Action *action)
{
	action->setEnabled(!action->context()->buddies().isEmpty());

	bool notifyAll = true;
	foreach (const Buddy &buddy, action->context()->contacts().toBuddySet())
		if (buddy.data())
		{
			if (!buddy.data()->customProperties()->property("notify:Notify", true).toBool())
			{
				notifyAll = false;
				break;
			}
		}

	action->setChecked(notifyAll);
}



#include "moc_notification-service.cpp"
