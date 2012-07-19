/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "notify/account-event-listener.h"
#include "notify/account-notification.h"
#include "notify/chat-event-listener.h"
#include "notify/group-event-listener.h"
#include "notify/multilogon-notification.h"
#include "notify/new-message-notification.h"
#include "notify/notification.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/status-changed-notification.h"
#include "notify/window-notifier.h"
#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
#include "notify/x11-screen-mode-checker.h"
#elif defined(Q_WS_WIN)
#include "notify/windows-screen-mode-checker.h"
#else
#include "notify/screen-mode-checker.h"
#endif

#define FULLSCREENCHECKTIMER_INTERVAL 2000 /*ms*/

#include "notification-service.h"

NotificationService::NotificationService(QObject *parent) :
		QObject(parent), SilentMode(false), AutoSilentMode(false), IsFullScreen(false)
{
	Notification::registerParserTags();
	AccountNotification::registerParserTags();

	NotifyUiHandler = new NotifyConfigurationUiHandler(this);
	MainConfigurationWindow::registerUiHandler(NotifyUiHandler);

	MessageNotification::registerEvents();
	StatusChangedNotification::registerEvents();
	MultilogonNotification::registerEvents();

	connect(StatusContainerManager::instance(), SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));

	createActionDescriptions();

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	FullscreenChecker = new X11ScreenModeChecker();
#elif defined(Q_WS_WIN)
	FullscreenChecker = new WindowsScreenModeChecker();
#else
	FullscreenChecker = new ScreenModeChecker();
#endif

	FullScreenCheckTimer.setInterval(FULLSCREENCHECKTIMER_INTERVAL);
	connect(&FullScreenCheckTimer, SIGNAL(timeout()), this, SLOT(checkFullScreen()));

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

	FullScreenCheckTimer.stop();
}

void NotificationService::createActionDescriptions()
{
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

	connect(SilentModeActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(silentModeActionCreated(Action *)));

	Core::instance()->kaduWindow()->insertMenuActionDescription(SilentModeActionDescription, KaduWindow::MenuKadu);
}

void NotificationService::createEventListeners()
{
	ChatListener = new ChatEventListener(this);
	AccountListener = new AccountEventListener(this);
	GroupListener = new GroupEventListener(this);
}

void NotificationService::statusUpdated()
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

void NotificationService::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const BuddySet &buddies = action->context()->buddies();

	bool on = true;
	foreach (const Buddy &buddy, buddies)
		if (buddy.data())
		{
			if (!buddy.property("notify:Notify", false).toBool())
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

		if (on)
			buddy.addProperty("notify:Notify", true, CustomProperties::Storable);
		else
			buddy.removeProperty("notify:Notify");
	}

	foreach (Action *action, notifyAboutUserActionDescription->actions())
		if (action->context()->contacts().toBuddySet() == buddies)
			action->setChecked(!on);
}

void NotificationService::silentModeActionCreated(Action *action)
{
	action->setChecked(!SilentMode);
}

void NotificationService::silentModeActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	setSilentMode(!toggled);
}

void NotificationService::setSilentMode(bool silentMode)
{
	if (silentMode == SilentMode)
		return;

	SilentMode = silentMode;
	foreach (Action *action, SilentModeActionDescription->actions())
		action->setChecked(!silentMode);

	config_file.writeEntry("Notify", "SilentMode", SilentMode);

	emit silentModeToggled(SilentMode);
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
	NotifyAboutAll = config_file.readBoolEntry("Notify", "NotifyAboutAll");
	NewMessageOnlyIfInactive = config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive");
	NotifyIgnoreOnConnection = config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection");
	IgnoreOnlineToOnline = config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline");
	SilentModeWhenDnD = config_file.readBoolEntry("Notify", "AwaySilentMode", false);
	SilentModeWhenFullscreen = config_file.readBoolEntry("Notify", "FullscreenSilentMode", false);
	setSilentMode(config_file.readBoolEntry("Notify", "SilentMode", false));

	if (SilentModeWhenFullscreen && !FullscreenChecker->isDummy())
		FullScreenCheckTimer.start();
	else
	{
		FullScreenCheckTimer.stop();
		IsFullScreen = false;
	}
}

void NotificationService::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "IgnoreOnlineToOnline", false);
	config_file.addVariable("Notify", "NewMessageOnlyIfInactive", true);
	config_file.addVariable("Notify", "NotifyAboutAll", true);
	config_file.addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

void NotificationService::checkFullScreen()
{
	bool wasSilent = silentMode();

	IsFullScreen = FullscreenChecker->isFullscreenAppActive() && !FullscreenChecker->isScreensaverActive();
	if (silentMode() != wasSilent)
		emit silentModeToggled(silentMode());
}

void NotificationService::notify ( Notification* notification )
{
	if (!ignoreNotifications())
		NotificationManager::instance()->notify(notification);
	else
		notification->callbackDiscard();
}

void checkNotify(Action *action)
{
	action->setEnabled(!action->context()->buddies().isEmpty());

	bool on = true;
	foreach (const Buddy &buddy, action->context()->contacts().toBuddySet())
		if (buddy.data())
		{
			if (!buddy.data()->customProperties()->property("notify:Notify", false).toBool())
			{
				on = false;
				break;
			}
		}

	action->setChecked(on);
}


