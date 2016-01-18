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

class Actions;
class Application;
class AvatarManager;
class BuddyChatManager;
class BuddyManager;
class BuddyPreferredManager;
class BuddyStorage;
class ChatImageRequestService;
class ChatManager;
class ChatTypeManager;
class ChatStyleManager;
class ChatWidgetActions;
class ChatWidgetManager;
class ChatWidgetMessageHandler;
class ChatWidgetRepository;
class ConfigurationManager;
class Configuration;
class ContactManager;
class FileTransferHandlerManager;
class FileTransferManager;
class FormattedStringFactory;
class GroupManager;
class IconsManager;
class InjectedFactory;
class KaduIcon;
class KaduWindow;
class MenuInventory;
class Message;
class MessageTransformerService;
class Myself;
class NetworkManager;
class NetworkProxyManager;
class NotificationManager;
class PathsProvider;
class PluginActivationService;
class PluginConflictResolver;
class PluginDependencyHandler;
class PluginRepository;
class PluginStateManager;
class PluginStateService;
class ProtocolsManager;
class ProxyEditWindowService;
class RawMessageTransformerService;
class Roster;
class SslCertificateManager;
class StatusChangerManager;
class StatusContainerManager;
class StatusSetter;
class StatusTypeManager;
class StoragePointFactory;
class SystemInfo;
class UnreadMessageRepository;
class UrlHandlerManager;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	mutable injeqt::v1::injector m_injector;
	std::shared_ptr<SimpleProvider<QWidget *>> KaduWindowProvider;
	std::shared_ptr<DefaultProvider<QWidget *>> MainWindowProvider;

	KaduWindow *Window;

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

	void createGui();
	void runServices();
	void runGuiServices();
	void stopServices();

	void activatePlugins();

	BuddyManager * buddyManager() const;
	NotificationManager * notificationManager() const;
	FormattedStringFactory * formattedStringFactory() const;
	RawMessageTransformerService * rawMessageTransformerService() const;
	UnreadMessageRepository * unreadMessageRepository() const;
	ChatWidgetActions * chatWidgetActions() const;
	ChatWidgetManager * chatWidgetManager() const;
	ChatWidgetRepository * chatWidgetRepository() const;
	ChatStyleManager * chatStyleManager() const;
	StoragePointFactory * storagePointFactory() const;

	PluginActivationService * pluginActivationService() const;
	PluginStateService * pluginStateService() const;

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
	NetworkProxyManager * networkProxyManager() const;
	ConfigurationManager * configurationManager() const;
	AvatarManager * avatarManager() const;
	BuddyChatManager * buddyChatManager() const;
	BuddyPreferredManager * buddyPreferredManager() const;
	ChatTypeManager * chatTypeManager() const;
	Actions * actions() const;
	MenuInventory * menuInventory() const;
	NetworkManager * networkManager() const;
	Roster * roster() const;
	SystemInfo * systemInfo() const;
	ProxyEditWindowService * proxyEditWindowService() const;
	Myself * myself() const;
	BuddyStorage * buddyStorage() const;

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
