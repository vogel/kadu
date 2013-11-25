/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "icons/kadu-icon.h"
#include "provider/simple-provider.h"
#include "status/status.h"

#include "exports.h"

template<class T>
class DefaultProvider;

namespace QCA
{
	class Initializer;
}

class AccountConfigurationWidgetFactoryRepository;
class BuddyConfigurationWidgetFactoryRepository;
class BuddyDataWindowRepository;
class ChatConfigurationWidgetFactoryRepository;
class ChatDataWindowRepository;
class ChatImageRequestService;
class ChatTopBarWidgetFactoryRepository;
class ChatWidgetActions;
class ChatWidgetActivationService;
class ChatWidgetContainerHandlerMapper;
class ChatWidgetContainerHandlerRepository;
class ChatWidgetFactory;
class ChatWidgetManager;
class ChatWidgetMessageHandler;
class ChatWidgetRepository;
class ChatWindowFactory;
class ChatWindowManager;
class ChatWindowStorage;
class ChatWindowRepository;
class ClipboardHtmlTransformerService;
class DomProcessorService;
class FormattedStringFactory;
class ImageStorageService;
class KaduApplication;
class KaduIcon;
class KaduWindow;
class Message;
class MessageFilterService;
class MessageHtmlRendererService;
class MessageTransformerService;
class NotificationService;
class PluginDependencyGraphBuilder;
class PluginInfoReader;
class PluginRepository;
class RawMessageTransformerService;
class RosterNotifier;
class StoragePointFactory;
class UnreadMessageRepository;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	QSharedPointer<SimpleProvider<QWidget *> > KaduWindowProvider;
	QSharedPointer<DefaultProvider<QWidget *> > MainWindowProvider;

	BuddyDataWindowRepository *CurrentBuddyDataWindowRepository;
	ChatDataWindowRepository *CurrentChatDataWindowRepository;
	ChatImageRequestService *CurrentChatImageRequestService;
	DomProcessorService *CurrentDomProcessorService;
	ImageStorageService *CurrentImageStorageService;
	MessageFilterService *CurrentMessageFilterService;
	MessageHtmlRendererService *CurrentMessageHtmlRendererService;
	MessageTransformerService *CurrentMessageTransformerService;
	NotificationService *CurrentNotificationService;
	FormattedStringFactory *CurrentFormattedStringFactory;
	RawMessageTransformerService *CurrentRawMessageTransformerService;
	ClipboardHtmlTransformerService *CurrentClipboardHtmlTransformerService;
	AccountConfigurationWidgetFactoryRepository *CurrentAccountConfigurationWidgetFactoryRepository;
	BuddyConfigurationWidgetFactoryRepository *CurrentBuddyConfigurationWidgetFactoryRepository;
	ChatConfigurationWidgetFactoryRepository *CurrentChatConfigurationWidgetFactoryRepository;
	ChatTopBarWidgetFactoryRepository *CurrentChatTopBarWidgetFactoryRepository;
	UnreadMessageRepository *CurrentUnreadMessageRepository;
	RosterNotifier *CurrentRosterNotifier;
	ChatWidgetActions *CurrentChatWidgetActions;
	ChatWidgetActivationService *CurrentChatWidgetActivationService;
	ChatWidgetContainerHandlerMapper *CurrentChatWidgetContainerHandlerMapper;
	ChatWidgetContainerHandlerRepository *CurrentChatWidgetContainerHandlerRepository;
	ChatWidgetFactory *CurrentChatWidgetFactory;
	ChatWidgetManager *CurrentChatWidgetManager;
	ChatWidgetMessageHandler *CurrentChatWidgetMessageHandler;
	ChatWidgetRepository *CurrentChatWidgetRepository;
	ChatWindowFactory *CurrentChatWindowFactory;
	ChatWindowManager *CurrentChatWindowManager;
	ChatWindowStorage *CurrentChatWindowStorage;
	ChatWindowRepository *CurrentChatWindowRepository;
	StoragePointFactory *CurrentStoragePointFactory;
	PluginDependencyGraphBuilder *CurrentPluginDependencyGraphBuilder;
	PluginInfoReader *CurrentPluginInfoReader;
	PluginRepository *CurrentPluginRepository;

	KaduWindow *Window;

	Buddy Myself;

	bool IsClosing;
	bool ShowMainWindowOnStart; // TODO: 0.11.0, it is a hack

	// NOTE: Kadu core itself doesn't use QCA, but important plugins do. And QCA lib
	// isn't very well suited to be unloaded, so we just link to it in core and initialize
	// here.
	QCA::Initializer *QcaInit;

	Core();
	virtual ~Core();

	void import_0_6_5_configuration();
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
	static Core * instance();

	static QString name();
	static QString version();
	static QString nameWithVersion();
	static KaduApplication * application();

	bool isClosing() { return IsClosing; }
	Buddy myself() { return Myself; }

	void createGui();
	void runServices();
	void runGuiServices();
	void stopServices();

	BuddyDataWindowRepository * buddyDataWindowRepository() const;
	ChatDataWindowRepository * chatDataWindowRepository() const;
	ChatImageRequestService * chatImageRequestService() const;
	DomProcessorService * domProcessorService() const;
	ImageStorageService * imageStorageService() const;
	MessageFilterService * messageFilterService() const;
	MessageHtmlRendererService * messageHtmlRendererService() const;
	MessageTransformerService * messageTransformerService() const;
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
	ChatWidgetActivationService * chatWidgetActivationService() const;
	ChatWidgetContainerHandlerMapper * chatWidgetContainerHandlerMapper() const;
	ChatWidgetContainerHandlerRepository * chatWidgetContainerHandlerRepository() const;
	ChatWidgetFactory * chatWidgetFactory() const;
	ChatWidgetManager * chatWidgetManager() const;
	ChatWidgetRepository * chatWidgetRepository() const;
	ChatWindowFactory * chatWindowFactory() const;
	ChatWindowManager * chatWindowManager() const;
	ChatWindowStorage * chatWindowStorage() const;
	ChatWindowRepository * chatWindowRepository() const;
	StoragePointFactory * storagePointFactory() const;
	PluginDependencyGraphBuilder * pluginDependencyGraphBuilder() const;
	PluginInfoReader * pluginInfoReader() const;
	PluginRepository * pluginRepository() const;

	void setShowMainWindowOnStart(bool show);
	void setMainWindow(QWidget *window);
	void showMainWindow();
	KaduWindow * kaduWindow();

	void initialized();
	void setIcon(const KaduIcon &icon);

	const QSharedPointer<DefaultProvider<QWidget *> > & mainWindowProvider() const;

public slots:
	void receivedSignal(const QString &signal);

	void quit();

signals:
	void connecting();
	void connected();
	void disconnected();

	//TODO:
	void searchingForTrayPosition(QPoint &);

};

#endif // CORE_H
