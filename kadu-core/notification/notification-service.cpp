/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/gui/configuration-ui-handler-repository.h"
#include "core/injected-factory.h"
#include "gui/actions/actions.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "message/message-manager.h"
#include "notification/listener/account-event-listener.h"
#include "notification/listener/chat-event-listener.h"
#include "notification/listener/group-event-listener.h"
#include "notification/notification/notification.h"
#include "notification/notification-callback.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-event-repository.h"
#include "notification/notify-configuration-ui-handler.h"
#include "notification/window-notifier.h"
#include "parser/parser.h"
#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#include "notification/x11-screen-mode-checker.h"
#elif defined(Q_OS_WIN)
#include "notification/windows-screen-mode-checker.h"
#else
#include "notification/screen-mode-checker.h"
#endif

#include "notification-service.h"

NotificationService::NotificationService(QObject *parent) :
		QObject(parent), SilentMode(false), AutoSilentMode(false), IsFullScreen(false), FullscreenChecker(0)
{
}

NotificationService::~NotificationService()
{
}

void NotificationService::setActions(Actions *actions)
{
	m_actions = actions;
}

void NotificationService::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void NotificationService::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void NotificationService::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void NotificationService::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void NotificationService::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void NotificationService::setMessageManager(MessageManager *messageManager)
{
	m_messageManager = messageManager;
}

void NotificationService::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void NotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void NotificationService::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void NotificationService::setNotifyConfigurationUiHandler(NotifyConfigurationUiHandler *notifyConfigurationUiHandler)
{
	m_notifyConfigurationUiHandler = notifyConfigurationUiHandler;
}

void NotificationService::setParser(Parser *parser)
{
	m_parser = parser;
}

void NotificationService::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void NotificationService::setWindowNotifier(WindowNotifier *windowNotifier)
{
	m_windowNotifier = windowNotifier;
}

void NotificationService::init()
{
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_notifyConfigurationUiHandler);

	auto ignoreCallback = NotificationCallback{
		"ignore",
		tr("Ignore"),
		[](Notification *) {}
	};

	m_notificationCallbackRepository->addCallback(ignoreCallback);

	Notification::registerParserTags(m_parser);

	m_notificationManager->registerNotifier(m_windowNotifier);

	connect(m_statusContainerManager, SIGNAL(statusUpdated(StatusContainer *)), this, SLOT(statusUpdated(StatusContainer *)));

	createActionDescriptions();

	createDefaultConfiguration();
	configurationUpdated();
}

void NotificationService::done()
{
	Notification::unregisterParserTags(m_parser);

	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_notifyConfigurationUiHandler);

	m_notificationManager->unregisterNotifier(m_windowNotifier);

	delete notifyAboutUserActionDescription;
	delete SilentModeActionDescription;
}

void NotificationService::createActionDescriptions()
{
	notifyAboutUserActionDescription = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/notify-about-buddy"), tr("Notify About Buddy"), true,
		checkNotify
	);

	m_menuInventory
		->menu("buddy-list")
		->addAction(notifyAboutUserActionDescription, KaduMenu::SectionActions);

	SilentModeActionDescription = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeGlobal, "silentModeAction",
		this, SLOT(silentModeActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/enable-notifications"), tr("Silent Mode"), true
	);

	connect(SilentModeActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(silentModeActionCreated(Action *)));

	m_menuInventory
		->menu("main")
		->addAction(SilentModeActionDescription, KaduMenu::SectionMiscTools, 5);
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

	bool wasSilent = SilentMode;
	SilentMode = newSilentMode;
	foreach (Action *action, SilentModeActionDescription->actions())
		action->setChecked(SilentMode);

	m_configuration->deprecatedApi()->writeEntry("Notify", "SilentMode", SilentMode);

	if (SilentMode != wasSilent)
		emit silentModeToggled(SilentMode);
}

bool NotificationService::silentMode()
{
	return SilentMode || (IsFullScreen && m_configuration->deprecatedApi()->readBoolEntry("Notify", "FullscreenSilentMode", false));
}

bool NotificationService::ignoreNotifications()
{
	return AutoSilentMode || silentMode();
}

void NotificationService::configurationUpdated()
{
	NewMessageOnlyIfInactive = m_configuration->deprecatedApi()->readBoolEntry("Notify", "NewMessageOnlyIfInactive");
	NotifyIgnoreOnConnection = m_configuration->deprecatedApi()->readBoolEntry("Notify", "NotifyIgnoreOnConnection");
	IgnoreOnlineToOnline = m_configuration->deprecatedApi()->readBoolEntry("Notify", "IgnoreOnlineToOnline");
	SilentModeWhenDnD = m_configuration->deprecatedApi()->readBoolEntry("Notify", "AwaySilentMode", false);
	SilentModeWhenFullscreen = m_configuration->deprecatedApi()->readBoolEntry("Notify", "FullscreenSilentMode", false);
	setSilentMode(m_configuration->deprecatedApi()->readBoolEntry("Notify", "SilentMode", false));

	if (SilentModeWhenFullscreen)
		startScreenModeChecker();
	else
		stopScreenModeChecker();
}

void NotificationService::startScreenModeChecker()
{
	if (FullscreenChecker)
		return;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	FullscreenChecker = new X11ScreenModeChecker();
#elif defined(Q_OS_WIN)
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
	IsFullScreen = inFullscreen;
}

void NotificationService::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Notify", "IgnoreOnlineToOnline", false);
	m_configuration->deprecatedApi()->addVariable("Notify", "NewMessageOnlyIfInactive", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

void NotificationService::notify(Notification *notification)
{
	if (!ignoreNotifications())
		m_notificationManager->notify(notification);
	else
		discardNotification(notification);
}

void NotificationService::acceptNotification(Notification *notification)
{
	if (notification->acceptCallback().isEmpty())
	{
		auto chat = qvariant_cast<Chat>(notification->data()["chat"]);
		notification->close();
		if (chat)
			m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
	}
	else
		m_notificationCallbackRepository->callback(notification->acceptCallback()).call(notification);
}

void NotificationService::discardNotification(Notification *notification)
{
	if (notification->discardCallback().isEmpty())
		notification->close();
	else
		m_notificationCallbackRepository->callback(notification->discardCallback()).call(notification);
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

#undef Bool

#include "moc_notification-service.cpp"
