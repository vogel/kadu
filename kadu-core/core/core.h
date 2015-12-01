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
class AttentionService;
class BuddyConfigurationWidgetFactoryRepository;
class BuddyDataWindowRepository;
class ChatConfigurationWidgetFactoryRepository;
class ChatDataWindowRepository;
class ChatImageRequestService;
class ChatTopBarWidgetFactoryRepository;
class ChatStyleRendererFactoryProvider;
class ChatWidgetActions;
class ChatWidgetContainerHandlerRepository;
class ChatWidgetFactory;
class ChatWidgetManager;
class ChatWidgetMessageHandler;
class ChatWidgetRepository;
class ClipboardHtmlTransformerService;
class ConfiguredChatStyleRendererFactoryProvider;
class DomProcessorService;
class FileTransferHandlerManager;
class FileTransferManager;
class FormattedStringFactory;
class ImageStorageService;
class Application;
class KaduIcon;
class KaduWindow;
class Message;
class MessageFilterService;
class MessageHtmlRendererService;
class MessageRenderInfoFactory;
class MessageTransformerService;
class NotificationCallbackRepository;
class NotificationEventRepository;
class NotificationManager;
class NotificationService;
class PluginActivationService;
class PluginConflictResolver;
class PluginDependencyHandler;
class PluginStateManager;
class PluginStateService;
class RawMessageTransformerService;
class RosterNotifier;
class RosterReplacer;
class SslCertificateManager;
class StoragePointFactory;
class UnreadMessageRepository;
class WebkitMessagesViewDisplayFactory;
class WebkitMessagesViewFactory;
class WebkitMessagesViewHandlerFactory;

namespace injeqt { namespace v1 { class injector; } }

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	injeqt::v1::injector &m_injector;
	std::shared_ptr<SimpleProvider<QWidget *>> KaduWindowProvider;
	std::shared_ptr<DefaultProvider<QWidget *>> MainWindowProvider;

	BuddyDataWindowRepository *CurrentBuddyDataWindowRepository;
	ChatDataWindowRepository *CurrentChatDataWindowRepository;
	ChatImageRequestService *CurrentChatImageRequestService;
	DomProcessorService *CurrentDomProcessorService;
	ImageStorageService *CurrentImageStorageService;
	MessageFilterService *CurrentMessageFilterService;
	MessageHtmlRendererService *CurrentMessageHtmlRendererService;
	MessageRenderInfoFactory *CurrentMessageRenderInfoFactory;
	MessageTransformerService *CurrentMessageTransformerService;
	RawMessageTransformerService *CurrentRawMessageTransformerService;
	ClipboardHtmlTransformerService *CurrentClipboardHtmlTransformerService;
	AccountConfigurationWidgetFactoryRepository *CurrentAccountConfigurationWidgetFactoryRepository;
	BuddyConfigurationWidgetFactoryRepository *CurrentBuddyConfigurationWidgetFactoryRepository;
	ChatConfigurationWidgetFactoryRepository *CurrentChatConfigurationWidgetFactoryRepository;
	ChatTopBarWidgetFactoryRepository *CurrentChatTopBarWidgetFactoryRepository;
	UnreadMessageRepository *CurrentUnreadMessageRepository;
	ChatWidgetActions *CurrentChatWidgetActions;
	ChatWidgetMessageHandler *CurrentChatWidgetMessageHandler;

	owned_qptr<ConfiguredChatStyleRendererFactoryProvider> CurrentChatStyleRendererFactoryProvider;
	owned_qptr<WebkitMessagesViewDisplayFactory> CurrentWebkitMessagesViewDisplayFactory;
	owned_qptr<WebkitMessagesViewFactory> CurrentWebkitMessagesViewFactory;
	owned_qptr<WebkitMessagesViewHandlerFactory> CurrentWebkitMessagesViewHandlerFactory;

	KaduWindow *Window;

	Buddy Myself;

	bool IsClosing;
	bool ShowMainWindowOnStart; // TODO: 0.11.0, it is a hack

	Core(injeqt::v1::injector &injector);
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
	static void createInstance(injeqt::v1::injector &injector);
	static Core * instance();

	static QString name();
	static QString version();
	static QString nameWithVersion();
	static Application * application();

	bool isClosing() { return IsClosing; }
	Buddy myself() { return Myself; }

	void createGui();
	void runServices();
	void runGuiServices();
	void stopServices();

	void activatePlugins();

	AttentionService * attentionService() const;
	BuddyDataWindowRepository * buddyDataWindowRepository() const;
	ChatDataWindowRepository * chatDataWindowRepository() const;
	ChatImageRequestService * chatImageRequestService() const;
	DomProcessorService * domProcessorService() const;
	ImageStorageService * imageStorageService() const;
	MessageFilterService * messageFilterService() const;
	MessageHtmlRendererService * messageHtmlRendererService() const;
	MessageRenderInfoFactory * messageRenderInfoFactory() const;
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
	ChatWidgetFactory * chatWidgetFactory() const;
	ChatWidgetManager * chatWidgetManager() const;
	ChatWidgetRepository * chatWidgetRepository() const;
	StoragePointFactory * storagePointFactory() const;

	PluginActivationService * pluginActivationService() const;
	PluginConflictResolver * pluginConflictResolver() const;
	PluginDependencyHandler * pluginDependencyHandler() const;
	PluginStateManager * pluginStateManager() const;
	PluginStateService * pluginStateService() const;

	ChatStyleRendererFactoryProvider * chatStyleRendererFactoryProvider() const;
	ConfiguredChatStyleRendererFactoryProvider * configuredChatStyleRendererFactoryProvider() const;
	WebkitMessagesViewDisplayFactory * webkitMessagesViewDisplayFactory() const;
	WebkitMessagesViewFactory * webkitMessagesViewFactory() const;
	WebkitMessagesViewHandlerFactory * webkitMessagesViewHandlerFactory() const;
	RosterReplacer * rosterReplacer() const;
	SslCertificateManager * sslCertificateManager() const;

	FileTransferHandlerManager * fileTransferHandlerManager() const;
	FileTransferManager * fileTransferManager() const;

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
