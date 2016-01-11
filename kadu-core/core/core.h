/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>
#include <injeqt/injector.h>

#include "accounts/accounts-aware-object.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "icons/kadu-icon.h"
#include "misc/memory.h"
#include "provider/simple-provider.h"
#include "status/status.h"

#include "exports.h"

template<class T>
class DefaultProvider;

class AccountConfigurationWidgetFactoryRepository;
class AccountManager;
class Actions;
class Application;
class AvatarManager;
class BuddyChatManager;
class BuddyConfigurationWidgetFactoryRepository;
class BuddyManager;
class BuddyPreferredManager;
class ChatConfigurationHolder;
class ChatConfigurationWidgetFactoryRepository;
class ChatImageRequestService;
class ChatManager;
class ChatTopBarWidgetFactoryRepository;
class ChatTypeManager;
class ChatStyleManager;
class ChatWidgetActions;
class ChatWidgetContainerHandlerRepository;
class ChatWidgetManager;
class ChatWidgetMessageHandler;
class ChatWidgetRepository;
class ClipboardHtmlTransformerService;
class ConfigurationManager;
class ConfigurationUiHandlerRepository;
class Configuration;
class ContactManager;
class DescriptionManager;
class FileTransferHandlerManager;
class FileTransferManager;
class FormattedStringFactory;
class GroupManager;
class IconsManager;
class IdentityManager;
class InjectedFactory;
class KaduIcon;
class KaduWindow;
class MenuInventory;
class Message;
class MessageHtmlRendererService;
class MessageManager;
class MessageTransformerService;
class NetworkManager;
class NetworkProxyManager;
class NotificationCallbackRepository;
class NotificationEventRepository;
class NotificationManager;
class NotificationService;
class PathsProvider;
class PluginActivationService;
class PluginConflictResolver;
class PluginDependencyHandler;
class PluginRepository;
class PluginStateManager;
class PluginStateService;
class ProtocolsManager;
class RawMessageTransformerService;
class RecentChatManager;
class RosterNotifier;
class RosterReplacer;
class Roster;
class SslCertificateManager;
class StatusChangerManager;
class StatusConfigurationHolder;
class StatusContainerManager;
class StatusSetter;
class StatusTypeManager;
class StoragePointFactory;
class SystemInfo;
class UnreadMessageRepository;
class UrlHandlerManager;
class YourAccountsWindowService;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	mutable injeqt::v1::injector m_injector;
	std::shared_ptr<SimpleProvider<QWidget *>> KaduWindowProvider;
	std::shared_ptr<DefaultProvider<QWidget *>> MainWindowProvider;

	MessageHtmlRendererService *CurrentMessageHtmlRendererService;
	ChatWidgetActions *CurrentChatWidgetActions;
	ChatWidgetMessageHandler *CurrentChatWidgetMessageHandler;

	KaduWindow *Window;

	Buddy Myself;

	bool IsClosing;
	bool ShowMainWindowOnStart; // TODO: 0.11.0, it is a hack

	Core(injeqt::v1::injector &&injector);
	virtual ~Core();

	void importPre10Configuration();
	void createDefaultConfiguration();
	void createAllDefaultToolbars();

	void init();
	void loadDefaultStatus();

private slots:
	void updateIcon();

	void deleteOldConfigurationFiles();
	void kaduWindowDestroyed();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);
	virtual void configurationUpdated();

public:
	static void createInstance(injeqt::v1::injector &&injector);
	static Core * instance();

	static QString name();
	static QString version();
	static QString nameWithVersion();

	bool isClosing() { return IsClosing; }
	Buddy myself() { return Myself; }

	void createGui();
	void runServices();
	void runGuiServices();
	void stopServices();

	void activatePlugins();

	AccountManager * accountManager() const;
	BuddyManager * buddyManager() const;
	MessageManager * messageManager() const;
	MessageHtmlRendererService * messageHtmlRendererService() const;
	MessageTransformerService * messageTransformerService() const;
	NotificationCallbackRepository * notificationCallbackRepository() const;
	NotificationEventRepository * notificationEventRepository() const;
	NotificationManager * notificationManager() const;
	NotificationService * notificationService() const;
	FormattedStringFactory * formattedStringFactory() const;
	RawMessageTransformerService * rawMessageTransformerService() const;
	ClipboardHtmlTransformerService * clipboardHtmlTransformerService() const;
	AccountConfigurationWidgetFactoryRepository * accountConfigurationWidgetFactoryRepository() const;
	BuddyConfigurationWidgetFactoryRepository * buddyConfigurationWidgetFactoryRepository() const;
	ChatConfigurationWidgetFactoryRepository * chatConfigurationWidgetFactoryRepository() const;
	ChatTopBarWidgetFactoryRepository * chatTopBarWidgetFactoryRepository() const;
	UnreadMessageRepository * unreadMessageRepository() const;
	RosterNotifier * rosterNotifier() const;
	ChatWidgetActions * chatWidgetActions() const;
	ChatWidgetContainerHandlerRepository * chatWidgetContainerHandlerRepository() const;
	ChatWidgetManager * chatWidgetManager() const;
	ChatWidgetRepository * chatWidgetRepository() const;
	ChatStyleManager * chatStyleManager() const;
	StoragePointFactory * storagePointFactory() const;
	ConfigurationUiHandlerRepository * configurationUiHandlerRepository() const;

	PluginActivationService * pluginActivationService() const;
	PluginConflictResolver * pluginConflictResolver() const;
	PluginDependencyHandler * pluginDependencyHandler() const;
	PluginStateManager * pluginStateManager() const;
	PluginStateService * pluginStateService() const;

	RosterReplacer * rosterReplacer() const;
	SslCertificateManager * sslCertificateManager() const;

	FileTransferHandlerManager * fileTransferHandlerManager() const;
	FileTransferManager * fileTransferManager() const;

	StatusChangerManager * statusChangerManager() const;
	ProtocolsManager * protocolsManager() const;
	UrlHandlerManager * urlHandlerManager() const;
	ChatManager * chatManager() const;
	StatusContainerManager * statusContainerManager() const;
	StatusTypeManager * statusTypeManager() const;
	InjectedFactory * injectedFactory() const;
	IconsManager * iconsManager() const;
	StatusSetter * statusSetter() const;
	ContactManager * contactManager() const;
	Configuration * configuration() const;
	PathsProvider * pathsProvider() const;
	Application * application() const;
	GroupManager * groupManager() const;
	RecentChatManager * recentChatManager() const;
	IdentityManager * identityManager() const;
	NetworkProxyManager * networkProxyManager() const;
	DescriptionManager * descriptionManager() const;
	ConfigurationManager * configurationManager() const;
	AvatarManager * avatarManager() const;
	BuddyChatManager * buddyChatManager() const;
	ChatConfigurationHolder * chatConfigurationHolder() const;
	BuddyPreferredManager * buddyPreferredManager() const;
	ChatTypeManager * chatTypeManager() const;
	Actions * actions() const;
	MenuInventory * menuInventory() const;
	NetworkManager * networkManager() const;
	Roster * roster() const;
	StatusConfigurationHolder * statusConfigurationHolder() const;
	SystemInfo * systemInfo() const;
	YourAccountsWindowService * yourAccountsWindowService() const;

	void setShowMainWindowOnStart(bool show);
	void setMainWindow(QWidget *window);
	void showMainWindow();
	KaduWindow * kaduWindow();

	void initialized();
	void setIcon(const KaduIcon &icon);

	const std::shared_ptr<DefaultProvider<QWidget *>> & mainWindowProvider() const;

public slots:
	void executeRemoteCommand(const QString &signal);

	void quit();

signals:
	void connecting();
	void connected();
	void disconnected();

	//TODO:
	void searchingForTrayPosition(QPoint &);

};

#endif // CORE_H
