/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <injeqt/injector.h>

#include "accounts/account-storage.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "buddies/buddy-dummy-factory.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy-storage.h"
#include "buddies/group-manager.h"
#include "chat-style/chat-style-configuration-ui-handler.h"
#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/chat-style-renderer-factory-provider.h"
#include "chat-style/engine/configured-chat-style-renderer-factory-provider.h"
#include "chat/buddy-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat/recent-chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/gui/configuration-ui-handler-repository.h"
#include "contacts/contact-manager.h"
#include "core/application.h"
#include "core/core.h"
#include "core/injected-factory.h"
#include "core/injector-provider.h"
#include "myself.h"
#include "dom/dom-processor-service.h"
#include "file-transfer/file-transfer-handler-manager.h"
#include "file-transfer/file-transfer-manager.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/actions/actions.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/menu/menu-inventory.h"
#include "gui/services/clipboard-html-transformer-service.h"
#include "gui/widgets/account-configuration-widget-factory-repository.h"
#include "gui/widgets/buddy-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-top-bar-widget-factory-repository.h"
#include "gui/widgets/chat-widget/chat-widget-actions.h"
#include "gui/widgets/chat-widget/chat-widget-activation-service.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-mapper.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-message-handler-configurator.h"
#include "gui/widgets/chat-widget/chat-widget-message-handler.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "gui/windows/buddy-data-window-repository.h"
#include "gui/windows/chat-data-window-repository.h"
#include "gui/windows/chat-window/chat-window-manager.h"
#include "gui/windows/chat-window/chat-window-repository.h"
#include "gui/windows/chat-window/chat-window-storage-configurator.h"
#include "gui/windows/chat-window/chat-window-storage.h"
#include "gui/windows/chat-window/window-chat-widget-container-handler.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/proxy-edit-window-service.h"
#include "gui/windows/search-window-actions.h"
#include "gui/windows/search-window.h"
#include "gui/windows/your-accounts-window-service.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity-manager.h"
#include "message/message-filter-service.h"
#include "message/message-html-renderer-service.h"
#include "message/message-manager.h"
#include "message/message-render-info-factory.h"
#include "message/message-render-info.h"
#include "message/unread-message-repository.h"
#include "misc/change-notifier-lock.h"
#include "misc/date-time-parser-tags.h"
#include "misc/paths-provider.h"
#include "network/network-manager.h"
#include "network/proxy/network-proxy-manager.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-manager.h"
#include "notification/notification-service.h"
#include "os/generic/system-info.h"
#include "parser/parser.h"
#include "plugin/activation/plugin-activation-error-handler.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugin/metadata/plugin-metadata-finder.h"
#include "plugin/metadata/plugin-metadata-reader.h"
#include "plugin/plugin-conflict-resolver.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/plugin-manager.h"
#include "plugin/state/plugin-state-manager.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state-storage.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "provider/default-provider.h"
#include "provider/simple-provider.h"
#include "roster/roster-notifier.h"
#include "roster/roster-replacer.h"
#include "roster/roster.h"
#include "services/chat-image-request-service-configurator.h"
#include "services/chat-image-request-service.h"
#include "services/image-storage-service.h"
#include "services/message-transformer-service.h"
#include "services/raw-message-transformer-service.h"
#include "ssl/ssl-certificate-manager.h"
#include "status/description-manager.h"
#include "status/status-changer-manager.h"
#include "status/status-configuration-holder.h"
#include "status/status-container-manager.h"
#include "status/status-setter.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "storage/storage-point-factory.h"
#include "themes/icon-theme-manager.h"
#include "url-handlers/url-handler-manager.h"
#include "activate.h"
#include "debug.h"
#include "kadu-config.h"
#include "updates.h"

#if WITH_LIBINDICATE_QT
#include <libindicate-qt/qindicateserver.h>
#endif

#include "core.h"

Core * Core::Instance = 0;

void Core::createInstance(injeqt::injector &&injector)
{
	Instance = new Core(std::move(injector));
	Instance->init();
}

Core * Core::instance()
{
	return Instance;
}

QString Core::name()
{
	return QLatin1String("Kadu");
}

QString Core::version()
{
	return QLatin1String(KADU_VERSION);
}

QString Core::nameWithVersion()
{
	return name() + QLatin1String(" ")  + version();
}

Core::Core(injeqt::injector &&injector) :
		m_injector{std::move(injector)},
		KaduWindowProvider{new SimpleProvider<QWidget *>(0)},
		MainWindowProvider{new DefaultProvider<QWidget *>(KaduWindowProvider)},
		CurrentChatWidgetMessageHandler{nullptr},
		Window(0),
		IsClosing(false),
		ShowMainWindowOnStart(true)
{
	// must be created first
	// TODO: should be maybe created by factory factory?
	m_injector.get<InjectorProvider>()->setInjector(&m_injector);
	m_injector.get<StoragePointFactory>()->setConfigurationFile(configuration()->api());
	Instance = this; // TODO: fix this hack

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

	createDefaultConfiguration();
	configurationUpdated();

	Parser::GlobalVariables.insert(QLatin1String("DATA_PATH"), pathsProvider()->dataPath());
	Parser::GlobalVariables.insert(QLatin1String("HOME"), PathsProvider::homePath());
	Parser::GlobalVariables.insert(QLatin1String("KADU_CONFIG"), pathsProvider()->profilePath());
	DateTimeParserTags::registerParserTags();

	importPre10Configuration();
}

Core::~Core()
{
	IsClosing = true;

	m_injector.get<PluginStateManager>()->storePluginStates();
	// CurrentPluginStateManager->storePluginStates();

	// unloading modules does that
	/*statusContainerManager()->disconnectAndStoreLastStatus(disconnectWithCurrentDescription, disconnectDescription);*/
	m_injector.get<SslCertificateManager>()->storePersistentSslCertificates();
	m_injector.get<ChatWindowManager>()->storeOpenedChatWindows();

	// some plugins crash on deactivation
	// ensure we have at least some configuration stored
	configurationManager()->flush();
	application()->backupConfiguration();

	m_injector.get<PluginManager>()->deactivatePlugins();
	// CurrentPluginManager->deactivatePlugins();

	stopServices();

	configurationManager()->flush();
	application()->backupConfiguration();

	KaduWindowProvider->provideValue(0);
	QWidget *hiddenParent = Window->parentWidget();
	delete Window;
	Window = 0;
	delete hiddenParent;
}

void Core::importPre10Configuration()
{
	if (configuration()->deprecatedApi()->readBoolEntry("General", "ImportedPre10"))
	{
		return;
	}

	foreach (const Buddy &buddy, buddyManager()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		bool notify = buddy.property("notify:Notify", false).toBool() || configuration()->deprecatedApi()->readBoolEntry("Notify", "NotifyAboutAll");

		if (notify)
			buddy.removeProperty("notify:Notify");
		else
			buddy.addProperty("notify:Notify", false, CustomProperties::Storable);
	}

	configuration()->deprecatedApi()->addVariable("General", "ImportedPre10", true);
}

void Core::createDefaultConfiguration()
{
	QWidget w;

	configuration()->deprecatedApi()->addVariable("Chat", "AutoSend", true);
	configuration()->deprecatedApi()->addVariable("Chat", "BlinkChatTitle", false);
	configuration()->deprecatedApi()->addVariable("Chat", "ChatCloseTimer", true);
	configuration()->deprecatedApi()->addVariable("Chat", "ChatCloseTimerPeriod", 2);
	configuration()->deprecatedApi()->addVariable("Chat", "ChatPrune", false);
	configuration()->deprecatedApi()->addVariable("Chat", "ChatPruneLen", 0);
	configuration()->deprecatedApi()->addVariable("Chat", "ConfirmChatClear", true);
	configuration()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousRichtext", true);
	configuration()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousUsers", false);
	configuration()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	configuration()->deprecatedApi()->addVariable("Chat", "LastImagePath", QDir::homePath() + '/');
	configuration()->deprecatedApi()->addVariable("Chat", "NewMessagesInChatTitle", false);
	configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessage", false);
	configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageWhenOnline", true);
	configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageMinimized", false);
	configuration()->deprecatedApi()->addVariable("Chat", "SaveOpenedWindows", true);
	configuration()->deprecatedApi()->addVariable("Chat", "ReceiveMessages", true);
	configuration()->deprecatedApi()->addVariable("Chat", "RecentChatsTimeout", 240);
	configuration()->deprecatedApi()->addVariable("Chat", "RecentChatsClear", false);
	configuration()->deprecatedApi()->addVariable("Chat", "RememberPosition", true);
	configuration()->deprecatedApi()->addVariable("Chat", "ShowEditWindowLabel", true);

	configuration()->deprecatedApi()->addVariable("General", "AllowExecutingFromParser", false);
	configuration()->deprecatedApi()->addVariable("General", "CheckUpdates", true);
	configuration()->deprecatedApi()->addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	configuration()->deprecatedApi()->addVariable("General", "DescriptionHeight", 60);
	configuration()->deprecatedApi()->addVariable("General", "DisconnectWithCurrentDescription", true);
#ifdef Q_OS_WIN
	configuration()->deprecatedApi()->addVariable("General", "HideMainWindowFromTaskbar", false);
#endif
	configuration()->deprecatedApi()->addVariable("General", "Language",  QLocale::system().name().left(2));
	configuration()->deprecatedApi()->addVariable("General", "Nick", tr("Me"));
	configuration()->deprecatedApi()->addVariable("General", "NumberOfDescriptions", 20);
	configuration()->deprecatedApi()->addVariable("General", "ParseStatus", false);
	configuration()->deprecatedApi()->addVariable("General", "ShowBlocked", true);
	configuration()->deprecatedApi()->addVariable("General", "ShowBlocking", true);
	configuration()->deprecatedApi()->addVariable("General", "ShowMyself", false);
	configuration()->deprecatedApi()->addVariable("General", "ShowOffline", true);
	configuration()->deprecatedApi()->addVariable("General", "ShowOnlineAndDescription", false);
	configuration()->deprecatedApi()->addVariable("General", "ShowWithoutDescription", true);

	if (configuration()->deprecatedApi()->readBoolEntry("General", "AdvancedMode", false))
	{
		configuration()->deprecatedApi()->addVariable("General", "StatusContainerType", "Account");
		configuration()->deprecatedApi()->addVariable("Look", "ShowExpandingControl", true);
	}
	else
	{
		configuration()->deprecatedApi()->addVariable("General", "StatusContainerType", "Identity");
		configuration()->deprecatedApi()->addVariable("Look", "ShowExpandingControl", false);
	}

	configuration()->deprecatedApi()->addVariable("General", "StartupLastDescription", true);
	configuration()->deprecatedApi()->addVariable("General", "StartupStatus", "LastStatus");
	configuration()->deprecatedApi()->addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", false);
	configuration()->deprecatedApi()->addVariable("General", "UserBoxHeight", 300);
	configuration()->deprecatedApi()->addVariable("General", "WindowActivationMethod", 0);
	configuration()->deprecatedApi()->addVariable("General", "MainConfiguration_Geometry", "50, 50, 790, 580");
	configuration()->deprecatedApi()->addVariable("General", "LookChatAdvanced_Geometry", "50, 50, 620, 540");

	configuration()->deprecatedApi()->addVariable("Look", "AlignUserboxIconsTop", true);
	configuration()->deprecatedApi()->addVariable("Look", "AvatarBorder", false);
	configuration()->deprecatedApi()->addVariable("Look", "AvatarGreyOut", true);
	configuration()->deprecatedApi()->addVariable("Look", "ForceCustomChatFont", false);
	QFont chatFont = qApp->font();
#ifdef Q_OS_WIN
	// On Windows default app font is often "MS Shell Dlg 2", and the default sans
	// family (Arial, at least in Qt 4.8) is better. Though, on X11 the default
	// sans family is the same while most users will have some nice default app
	// font, like DejaVu, Ubuntu (the font, not the distro) or alike.
	chatFont.setStyleHint(QFont::SansSerif);
	chatFont.setFamily(chatFont.defaultFamily());
#endif
	configuration()->deprecatedApi()->addVariable("Look", "ChatFont", chatFont);
	configuration()->deprecatedApi()->addVariable("Look", "ChatBgFilled", // depends on configuration imported from older version
		configuration()->deprecatedApi()->readColorEntry("Look", "ChatBgColor").isValid() &&
		configuration()->deprecatedApi()->readColorEntry("Look", "ChatBgColor") != QColor("#ffffff"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatTextCustomColors", // depends on configuration imported from older version
		configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor").isValid() &&
		configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor") != QColor("#ffffff"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	configuration()->deprecatedApi()->addVariable("Look", "ChatTextFontColor", QColor("#000000"));
	configuration()->deprecatedApi()->addVariable("Look", "DescriptionColor", w.palette().text().color());
	configuration()->deprecatedApi()->addVariable("Look", "DisplayGroupTabs", true);
	configuration()->deprecatedApi()->addVariable("Look", "HeaderSeparatorHeight", 1);
	configuration()->deprecatedApi()->addVariable("Look", "InfoPanelFgColor", w.palette().text().color());
	configuration()->deprecatedApi()->addVariable("Look", "InfoPanelBgFilled", false);
	configuration()->deprecatedApi()->addVariable("Look", "InfoPanelBgColor", w.palette().base().color());
	configuration()->deprecatedApi()->addVariable("Look", "InfoPanelSyntaxFile", "ultr");
	configuration()->deprecatedApi()->addVariable("Look", "NiceDateFormat", true);
	configuration()->deprecatedApi()->addVariable("Look", "NoHeaderInterval", 30);
	configuration()->deprecatedApi()->addVariable("Look", "NoHeaderRepeat", true);
	configuration()->deprecatedApi()->addVariable("Look", "NoServerTime", true);
	configuration()->deprecatedApi()->addVariable("Look", "NoServerTimeDiff", 60);
	configuration()->deprecatedApi()->addVariable("Look", "PanelFont", qApp->font());
	configuration()->deprecatedApi()->addVariable("Look", "PanelVerticalScrollbar", false);
	configuration()->deprecatedApi()->addVariable("Look", "ParagraphSeparator", 4);
	configuration()->deprecatedApi()->addVariable("Look", "ShowAvatars", true);
	configuration()->deprecatedApi()->addVariable("Look", "IconTheme", IconThemeManager::defaultTheme());
	configuration()->deprecatedApi()->addVariable("Look", "ShowGroupAll", true);
	configuration()->deprecatedApi()->addVariable("Look", "ShowBold", true);
	configuration()->deprecatedApi()->addVariable("Look", "ShowDesc", true);
	configuration()->deprecatedApi()->addVariable("Look", "ShowInfoPanel", false);
	configuration()->deprecatedApi()->addVariable("Look", "ShowMultilineDesc", true);
	configuration()->deprecatedApi()->addVariable("Look", "ShowStatusButton", true);
	configuration()->deprecatedApi()->addVariable("Look", "Style", "Satin");
	configuration()->deprecatedApi()->addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	configuration()->deprecatedApi()->addVariable("Look", "UserboxTransparency", false);
	configuration()->deprecatedApi()->addVariable("Look", "UserboxAlpha", 0);
	configuration()->deprecatedApi()->addVariable("Look", "UserboxBlur", true);
	configuration()->deprecatedApi()->addVariable("Look", "UserboxBgColor", w.palette().base().color());
	configuration()->deprecatedApi()->addVariable("Look", "UserboxAlternateBgColor", w.palette().alternateBase().color());
	configuration()->deprecatedApi()->addVariable("Look", "UserBoxColumnCount", 1);
	configuration()->deprecatedApi()->addVariable("Look", "UserboxFgColor", w.palette().text().color());
	QFont userboxfont(qApp->font());
	userboxfont.setPointSize(qApp->font().pointSize() + 1);
	configuration()->deprecatedApi()->addVariable("Look", "UserboxFont", userboxfont);
	configuration()->deprecatedApi()->addVariable("Look", "UseUserboxBackground", false);

	configuration()->deprecatedApi()->addVariable("Network", "DefaultPort", 0);
	configuration()->deprecatedApi()->addVariable("Network", "isDefServers", true);
	configuration()->deprecatedApi()->addVariable("Network", "Server", QString());
	configuration()->deprecatedApi()->addVariable("Network", "UseProxy", false);

#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the function keys with 'apple' button
	 * as it is the default system configuration */
	configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_clear", "Ctrl+F9");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_configure", "Ctrl+F2");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_modulesmanager", "Ctrl+F4");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showoffline", "Ctrl+F9");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showonlydesc", "Ctrl+F10");
#else
	configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_clear", "F9");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_configure", "F2");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_modulesmanager", "F4");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showoffline", "F9");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showonlydesc", "F10");
#endif
	configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_bold", "Ctrl+B");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_close", "Esc");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_italic", "Ctrl+I");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_deleteuser", "Del");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_persinfo", "Ins");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");
	configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_exit", "Ctrl+Q");

	configuration()->deprecatedApi()->addVariable("Chat", "UseDefaultWebBrowser", configuration()->deprecatedApi()->readEntry("Chat", "WebBrowser").isEmpty());
	configuration()->deprecatedApi()->addVariable("Chat", "UseDefaultEMailClient", configuration()->deprecatedApi()->readEntry("Chat", "MailClient").isEmpty());
	configuration()->deprecatedApi()->addVariable("Chat", "ContactStateChats", true);
	configuration()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitle", true);
	configuration()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitleSyntax", QString());
	configuration()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitlePosition", 1);

	createAllDefaultToolbars();
}

void Core::createAllDefaultToolbars()
{
	// don't use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = configuration()->api()->findElement(configuration()->api()->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = configuration()->api()->createElement(configuration()->api()->rootElement(), "Toolbars");

	KaduWindow::createDefaultToolbars(configuration(), toolbarsConfig);
	ChatEditBox::createDefaultToolbars(configuration(), toolbarsConfig);
	SearchWindow::createDefaultToolbars(configuration(), toolbarsConfig);

	application()->flushConfiguration();
}

void Core::init()
{
	MessageRenderInfo::registerParserTags(m_injector.get<ChatConfigurationHolder>(), m_injector.get<MessageHtmlRendererService>());

	runServices();

	// protocol modules should be loaded before gui
	// it fixes crash on loading pending messages from config, contacts import from 0.6.5, and maybe other issues
	{
		auto changeNotifierLock = ChangeNotifierLock{m_injector.get<PluginStateService>()->changeNotifier()};
		m_injector.get<PluginManager>()->activateProtocolPlugins();
	}

	injectedFactory()->makeInjected<Updates>(this);

	QApplication::setWindowIcon(KaduIcon("kadu_icons/kadu").icon());
	connect(iconsManager(), SIGNAL(themeChanged()), this, SLOT(updateIcon()));
	QTimer::singleShot(15000, this, SLOT(deleteOldConfigurationFiles()));

	// TODO: add some life-cycle management
	notificationManager();
	m_injector.get<SearchWindowActions>(); // temporary, during full-injection-port

	buddyManager()->ensureLoaded();
	contactManager()->ensureLoaded();
	// Without that UnreadMessageRepository is loaded while filtering buddies list for the first time.
	// It has to happen earlier because UnreadMessageRepository::loaded() might add buddies to the BuddyManager
	// which (the buddies) otherwise will not be taken into account by buddies list before its next update.
	unreadMessageRepository()->ensureLoaded();
	Core::instance()->avatarManager(); // initialize that

#if WITH_LIBINDICATE_QT
	// Use a symbol from libindicate-qt so that it will not get dropped for example by --as-needed.
	(void)QIndicate::Server::defaultInstance();
#endif
}

void Core::initialized()
{
	statusSetter()->coreInitialized();
}

void Core::deleteOldConfigurationFiles()
{
	kdebugf();

	QDir oldConfigs(pathsProvider()->profilePath(), "kadu-0.12.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs.count() > 20)
		for (unsigned int i = 0, max = oldConfigs.count() - 20; i < max; ++i)
			QFile::remove(pathsProvider()->profilePath() + oldConfigs[static_cast<int>(i)]);

	QDir oldConfigs2(pathsProvider()->profilePath(), "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
			QFile::remove(pathsProvider()->profilePath() + oldConfigs2[static_cast<int>(i)]);

	QDir oldBacktraces(pathsProvider()->profilePath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
			QFile::remove(pathsProvider()->profilePath() + oldBacktraces[static_cast<int>(i)]);

	QDir oldDebugs(pathsProvider()->profilePath(), "kadu.log.*", QDir::Name, QDir::Files);
	if (oldDebugs.count() > 20)
		for (unsigned int i = 0, max = oldDebugs.count() - 20; i < max; ++i)
			QFile::remove(pathsProvider()->profilePath() + oldDebugs[static_cast<int>(i)]);

	kdebugf2();
}

void Core::updateIcon()
{
	if (isClosing())
		return;

	QApplication::setWindowIcon(KaduIcon("kadu_icons/kadu").icon());
}

void Core::kaduWindowDestroyed()
{
	KaduWindowProvider->provideValue(0);
	Window = 0;
}

void Core::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	connect(protocol, SIGNAL(connecting(Account)), this, SIGNAL(connecting()));
	connect(protocol, SIGNAL(connected(Account)), this, SIGNAL(connected()));
	connect(protocol, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
}

void Core::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (protocol)
		disconnect(protocol, 0, this, 0);
}

void Core::configurationUpdated()
{
#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		       QSettings::NativeFormat);
	if(configuration()->deprecatedApi()->readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu",
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

	bool ok;
	int newMask = qgetenv("DEBUG_MASK").toInt(&ok);
	debug_mask = ok ? newMask : configuration()->deprecatedApi()->readNumEntry("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
}

void Core::createGui()
{
	Window = m_injector.get<InjectedFactory>()->makeInjected<KaduWindow>();
	connect(Window, SIGNAL(destroyed()), this, SLOT(kaduWindowDestroyed()));
	KaduWindowProvider->provideValue(Window);

	// initialize file transfers
	m_injector.get<FileTransferHandlerManager>();
	m_injector.get<FileTransferManager>();
}

void Core::runServices()
{
	auto rosterNotifier = m_injector.get<RosterNotifier>();
	for (auto &&notifyEvent : rosterNotifier->notifyEvents())
		notificationEventRepository()->addNotificationEvent(notifyEvent);

	auto chatWidgetContainerHandlerRepository = m_injector.get<ChatWidgetContainerHandlerRepository>();
	chatWidgetContainerHandlerRepository->registerChatWidgetContainerHandler(m_injector.get<WindowChatWidgetContainerHandler>());

	CurrentChatWidgetMessageHandler = new ChatWidgetMessageHandler(this);
	CurrentChatWidgetMessageHandler->setChatWidgetActivationService(m_injector.get<ChatWidgetActivationService>());
	CurrentChatWidgetMessageHandler->setChatWidgetManager(m_injector.get<ChatWidgetManager>());
	CurrentChatWidgetMessageHandler->setChatWidgetRepository(m_injector.get<ChatWidgetRepository>());
	CurrentChatWidgetMessageHandler->setMessageManager(m_injector.get<MessageManager>());
	CurrentChatWidgetMessageHandler->setUnreadMessageRepository(m_injector.get<UnreadMessageRepository>());
	auto chatWidgetMessageHandlerConfigurator = new ChatWidgetMessageHandlerConfigurator(); // this is basically a global so we do not care about relesing it
	chatWidgetMessageHandlerConfigurator->setChatWidgetMessageHandler(CurrentChatWidgetMessageHandler);

	m_injector.get<ChatWindowStorage>()->setChatManager(chatManager());
	auto chatWindowStorageConfigurator = new ChatWindowStorageConfigurator(); // this is basically a global so we do not care about relesing it
	chatWindowStorageConfigurator->setChatWindowStorage(m_injector.get<ChatWindowStorage>());

	// this instance lives forever
	// TODO: maybe make it QObject and make CurrentChatImageRequestService its parent
	ChatImageRequestServiceConfigurator *configurator = new ChatImageRequestServiceConfigurator();
	configurator->setChatImageRequestService(m_injector.get<ChatImageRequestService>());

	m_injector.get<PluginMetadataFinder>()->setDirectory(pathsProvider()->dataPath() + QLatin1String{"plugins"});
	m_injector.get<PluginStateManager>()->loadPluginStates();

	m_injector.get<ConfigurationUiHandlerRepository>()->addConfigurationUiHandler(m_injector.get<ChatStyleConfigurationUiHandler>());
}

void Core::runGuiServices()
{
	CurrentChatWidgetMessageHandler->setNotificationService(m_injector.get<NotificationService>());

	m_injector.get<ChatWindowManager>()->openStoredChatWindows();
	m_injector.get<SslCertificateManager>()->loadPersistentSslCertificates();
}

void Core::stopServices()
{
	m_injector.get<ConfigurationUiHandlerRepository>()->removeConfigurationUiHandler(m_injector.get<ChatStyleConfigurationUiHandler>());

	auto chatWidgetRepository = m_injector.get<ChatWidgetRepository>();
	while (begin(chatWidgetRepository) != end(chatWidgetRepository))
		chatWidgetRepository->removeChatWidget(*begin(chatWidgetRepository));
}

void Core::activatePlugins()
{
	auto changeNotifierLock = ChangeNotifierLock{m_injector.get<PluginStateService>()->changeNotifier()};
	m_injector.get<PluginManager>()->activatePlugins();
	m_injector.get<PluginManager>()->activateReplacementPlugins();
}

BuddyManager * Core::buddyManager() const
{
	return m_injector.get<BuddyManager>();
}

NotificationCallbackRepository * Core::notificationCallbackRepository() const
{
	return m_injector.get<NotificationCallbackRepository>();
}

NotificationEventRepository * Core::notificationEventRepository() const
{
	return m_injector.get<NotificationEventRepository>();
}

NotificationManager * Core::notificationManager() const
{
	return m_injector.get<NotificationManager>();
}

NotificationService * Core::notificationService() const
{
	return m_injector.get<NotificationService>();
}

FormattedStringFactory * Core::formattedStringFactory() const
{
	return m_injector.get<FormattedStringFactory>();
}

RawMessageTransformerService * Core::rawMessageTransformerService() const
{
	return m_injector.get<RawMessageTransformerService>();
}

ClipboardHtmlTransformerService * Core::clipboardHtmlTransformerService() const
{
	return m_injector.get<ClipboardHtmlTransformerService>();
}

BuddyConfigurationWidgetFactoryRepository * Core::buddyConfigurationWidgetFactoryRepository() const
{
	return m_injector.get<BuddyConfigurationWidgetFactoryRepository>();
}

ChatConfigurationWidgetFactoryRepository * Core::chatConfigurationWidgetFactoryRepository() const
{
	return m_injector.get<ChatConfigurationWidgetFactoryRepository>();
}

ChatTopBarWidgetFactoryRepository * Core::chatTopBarWidgetFactoryRepository() const
{
	return m_injector.get<ChatTopBarWidgetFactoryRepository>();
}

UnreadMessageRepository * Core::unreadMessageRepository() const
{
	return m_injector.get<UnreadMessageRepository>();
}

RosterNotifier * Core::rosterNotifier() const
{
	return m_injector.get<RosterNotifier>();
}

ChatWidgetActions * Core::chatWidgetActions() const
{
	return m_injector.get<ChatWidgetActions>();
}

ChatWidgetManager * Core::chatWidgetManager() const
{
	return m_injector.get<ChatWidgetManager>();
}

ChatWidgetRepository * Core::chatWidgetRepository() const
{
	return m_injector.get<ChatWidgetRepository>();
}

ChatStyleManager * Core::chatStyleManager() const
{
	return m_injector.get<ChatStyleManager>();
}

StoragePointFactory * Core::storagePointFactory() const
{
	return m_injector.get<StoragePointFactory>();
}

PluginActivationService * Core::pluginActivationService() const
{
	return m_injector.get<PluginActivationService>();
}

PluginStateService * Core::pluginStateService() const
{
	return m_injector.get<PluginStateService>();
}

RosterReplacer * Core::rosterReplacer() const
{
	return m_injector.get<RosterReplacer>();
}

SslCertificateManager * Core::sslCertificateManager() const
{
	return m_injector.get<SslCertificateManager>();
}

FileTransferHandlerManager * Core::fileTransferHandlerManager() const
{
	return m_injector.get<FileTransferHandlerManager>();
}

FileTransferManager * Core::fileTransferManager() const
{
	return m_injector.get<FileTransferManager>();
}

StatusChangerManager * Core::statusChangerManager() const
{
	return m_injector.get<StatusChangerManager>();
}

ProtocolsManager * Core::protocolsManager() const
{
	return m_injector.get<ProtocolsManager>();
}

UrlHandlerManager * Core::urlHandlerManager() const
{
	return m_injector.get<UrlHandlerManager>();
}

ChatManager * Core::chatManager() const
{
	return m_injector.get<ChatManager>();
}

StatusContainerManager * Core::statusContainerManager() const
{
	return m_injector.get<StatusContainerManager>();
}

StatusTypeManager * Core::statusTypeManager() const
{
	return m_injector.get<StatusTypeManager>();
}

InjectedFactory * Core::injectedFactory() const
{
	return m_injector.get<InjectedFactory>();
}

IconsManager * Core::iconsManager() const
{
	return m_injector.get<IconsManager>();
}

StatusSetter * Core::statusSetter() const
{
	return m_injector.get<StatusSetter>();
}

ContactManager * Core::contactManager() const
{
	return m_injector.get<ContactManager>();
}

Configuration * Core::configuration() const
{
	return m_injector.get<Configuration>();
}

PathsProvider * Core::pathsProvider() const
{
	return m_injector.get<PathsProvider>();
}

Application * Core::application() const
{
	return m_injector.get<Application>();
}

GroupManager * Core::groupManager() const
{
	return m_injector.get<GroupManager>();
}

IdentityManager * Core::identityManager() const
{
	return m_injector.get<IdentityManager>();
}

NetworkProxyManager * Core::networkProxyManager() const
{
	return m_injector.get<NetworkProxyManager>();
}

DescriptionManager * Core::descriptionManager() const
{
	return m_injector.get<DescriptionManager>();
}

ConfigurationManager * Core::configurationManager() const
{
	return m_injector.get<ConfigurationManager>();
}

AvatarManager * Core::avatarManager() const
{
	return m_injector.get<AvatarManager>();
}

BuddyChatManager * Core::buddyChatManager() const
{
	return m_injector.get<BuddyChatManager>();
}

BuddyPreferredManager * Core::buddyPreferredManager() const
{
	return m_injector.get<BuddyPreferredManager>();
}

ChatTypeManager * Core::chatTypeManager() const
{
	return m_injector.get<ChatTypeManager>();
}

Actions * Core::actions() const
{
	return m_injector.get<Actions>();
}

MenuInventory * Core::menuInventory() const
{
	return m_injector.get<MenuInventory>();
}

NetworkManager * Core::networkManager() const
{
	return m_injector.get<NetworkManager>();
}

Roster * Core::roster() const
{
	return m_injector.get<Roster>();
}

StatusConfigurationHolder * Core::statusConfigurationHolder() const
{
	return m_injector.get<StatusConfigurationHolder>();
}

SystemInfo * Core::systemInfo() const
{
	return m_injector.get<SystemInfo>();
}

ProxyEditWindowService * Core::proxyEditWindowService() const
{
	return m_injector.get<ProxyEditWindowService>();
}

ToolTipClassManager * Core::toolTipClassManager() const
{
	return m_injector.get<ToolTipClassManager>();
}

Myself * Core::myself() const
{
	return m_injector.get<Myself>();
}

BuddyStorage * Core::buddyStorage() const
{
	return m_injector.get<BuddyStorage>();
}

BuddyDummyFactory * Core::buddyDummyFactory() const
{
	return m_injector.get<BuddyDummyFactory>();
}

void Core::showMainWindow()
{
	if (ShowMainWindowOnStart)
		MainWindowProvider->provide()->show();

	// after first call which has to be placed in main(), this method should always show main window
	ShowMainWindowOnStart = true;
}

void Core::setShowMainWindowOnStart(bool show)
{
	ShowMainWindowOnStart = show;
}

KaduWindow * Core::kaduWindow()
{
	return Window;
}

const std::shared_ptr<DefaultProvider<QWidget *>> & Core::mainWindowProvider() const
{
	return MainWindowProvider;
}

void Core::executeRemoteCommand(const QString &remoteCommand)
{
	if ("activate" == remoteCommand)
		_activateWindow(MainWindowProvider->provide());
	else
		urlHandlerManager()->openUrl(remoteCommand.toUtf8(), true);
}

void Core::quit()
{
	if (!Instance)
		return;

	delete Instance;
	Instance = 0;
}

#include "moc_core.cpp"
