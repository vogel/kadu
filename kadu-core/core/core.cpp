/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010, 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2010, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/chat-style-renderer-factory-provider.h"
#include "chat-style/engine/configured-chat-style-renderer-factory-provider.h"
#include "chat/buddy-chat-manager.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "core/application.h"
#include "dom/dom-processor-service.h"
#include "file-transfer/file-transfer-manager.h"
#include "formatted-string/formatted-string-factory.h"
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
#include "gui/widgets/chat-widget/chat-widget-factory.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-message-handler-configurator.h"
#include "gui/widgets/chat-widget/chat-widget-message-handler.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget-state-persistence-service.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-handler-factory.h"
#include "gui/windows/buddy-data-window-repository.h"
#include "gui/windows/chat-data-window-repository.h"
#include "gui/windows/chat-window/chat-window-factory.h"
#include "gui/windows/chat-window/chat-window-manager.h"
#include "gui/windows/chat-window/chat-window-repository.h"
#include "gui/windows/chat-window/chat-window-storage-configurator.h"
#include "gui/windows/chat-window/chat-window-storage.h"
#include "gui/windows/chat-window/window-chat-widget-container-handler.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/search-window.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "message/message-html-renderer-service.h"
#include "message/message-manager.h"
#include "message/message-render-info-factory.h"
#include "message/message-render-info.h"
#include "message/unread-message-repository.h"
#include "misc/change-notifier-lock.h"
#include "misc/date-time-parser-tags.h"
#include "misc/paths-provider.h"
#include "notify/notification-manager.h"
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
#include "roster/roster-notifier.h"
#include "roster/roster-replacer.h"
#include "provider/default-provider.h"
#include "provider/simple-provider.h"
#include "services/chat-image-request-service-configurator.h"
#include "services/chat-image-request-service.h"
#include "services/image-storage-service.h"
#include "services/message-filter-service.h"
#include "services/message-transformer-service.h"
#include "services/notification-service.h"
#include "services/raw-message-transformer-service.h"
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

void Core::createInstance(injeqt::injector &injector)
{
	Instance = new Core(injector);
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

Application * Core::application()
{
	return Application::instance();
}

Core::Core(injeqt::injector &injector) :
		m_injector(injector),
		KaduWindowProvider{new SimpleProvider<QWidget *>(0)},
		MainWindowProvider{new DefaultProvider<QWidget *>(KaduWindowProvider)},
		CurrentBuddyDataWindowRepository{nullptr},
		CurrentChatDataWindowRepository{nullptr},
		CurrentChatImageRequestService{nullptr},
		CurrentDomProcessorService{nullptr},
		CurrentImageStorageService{nullptr},
		CurrentMessageFilterService{nullptr},
		CurrentMessageHtmlRendererService{nullptr},
		CurrentMessageRenderInfoFactory{nullptr},
		CurrentMessageTransformerService{nullptr},
		CurrentNotificationService{nullptr},
		CurrentRawMessageTransformerService{nullptr},
		CurrentClipboardHtmlTransformerService{nullptr},
		CurrentAccountConfigurationWidgetFactoryRepository{nullptr},
		CurrentBuddyConfigurationWidgetFactoryRepository{nullptr},
		CurrentChatConfigurationWidgetFactoryRepository{nullptr},
		CurrentChatTopBarWidgetFactoryRepository{nullptr},
		CurrentUnreadMessageRepository{nullptr},
		CurrentChatWidgetActions{nullptr},
		CurrentChatWidgetMessageHandler{nullptr},
		Window(0),
		Myself(Buddy::create()), IsClosing(false),
		ShowMainWindowOnStart(true)
{
	// must be created first
	// TODO: should be maybe created by factory factory?
	m_injector.get<StoragePointFactory>()->setConfigurationFile(Application::instance()->configuration()->api());
	Instance = this; // TODO: fix this hack

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

	createDefaultConfiguration();
	configurationUpdated();

	StatusConfigurationHolder::createInstance();

	Parser::GlobalVariables.insert(QLatin1String("DATA_PATH"), Application::instance()->pathsProvider()->dataPath());
	Parser::GlobalVariables.insert(QLatin1String("HOME"), PathsProvider::homePath());
	Parser::GlobalVariables.insert(QLatin1String("KADU_CONFIG"), Application::instance()->pathsProvider()->profilePath());
	DateTimeParserTags::registerParserTags();

	importPre10Configuration();
}

Core::~Core()
{
	IsClosing = true;

	m_injector.get<PluginStateManager>()->storePluginStates();
	// CurrentPluginStateManager->storePluginStates();

	// unloading modules does that
	/*StatusContainerManager::instance()->disconnectAndStoreLastStatus(disconnectWithCurrentDescription, disconnectDescription);*/
	m_injector.get<ChatWindowManager>()->storeOpenedChatWindows();

	// some plugins crash on deactivation
	// ensure we have at least some configuration stored
	ConfigurationManager::instance()->flush();
	Application::instance()->backupConfiguration();

	m_injector.get<PluginManager>()->deactivatePlugins();
	// CurrentPluginManager->deactivatePlugins();

	stopServices();

	ConfigurationManager::instance()->flush();
	Application::instance()->backupConfiguration();

#ifdef Q_OS_MAC
	QApplication::setWindowIcon(KaduIcon("kadu_icons/kadu").icon());
#endif // Q_OS_MAC

	KaduWindowProvider->provideValue(0);
	QWidget *hiddenParent = Window->parentWidget();
	delete Window;
	Window = 0;
	delete hiddenParent;

	StatusConfigurationHolder::destroyInstance();

	triggerAllAccountsUnregistered();
}

void Core::importPre10Configuration()
{
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "ImportedPre10"))
	{
		return;
	}

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		bool notify = buddy.property("notify:Notify", false).toBool() || Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Notify", "NotifyAboutAll");

		if (notify)
			buddy.removeProperty("notify:Notify");
		else
			buddy.addProperty("notify:Notify", false, CustomProperties::Storable);
	}

	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ImportedPre10", true);
}

void Core::createDefaultConfiguration()
{
	QWidget w;

	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "AutoSend", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "BlinkChatTitle", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ChatCloseTimer", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ChatCloseTimerPeriod", 2);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ChatPrune", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ChatPruneLen", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ConfirmChatClear", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousRichtext", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousUsers", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "LastImagePath", QDir::homePath() + '/');
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "NewMessagesInChatTitle", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessage", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageWhenOnline", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageMinimized", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "SaveOpenedWindows", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ReceiveMessages", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "RecentChatsTimeout", 240);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "RecentChatsClear", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "RememberPosition", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ShowEditWindowLabel", true);

	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AllowExecutingFromParser", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "CheckUpdates", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "DescriptionHeight", 60);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "DisconnectWithCurrentDescription", true);
#ifdef Q_OS_WIN32
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "HideMainWindowFromTaskbar", false);
#endif
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "Language",  QLocale::system().name().left(2));
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "Nick", tr("Me"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "NumberOfDescriptions", 20);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ParseStatus", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowBlocked", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowBlocking", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowMyself", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowOffline", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowOnlineAndDescription", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowWithoutDescription", true);

	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "AdvancedMode", false))
	{
		Application::instance()->configuration()->deprecatedApi()->addVariable("General", "StatusContainerType", "Account");
		Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowExpandingControl", true);
	}
	else
	{
		Application::instance()->configuration()->deprecatedApi()->addVariable("General", "StatusContainerType", "Identity");
		Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowExpandingControl", false);
	}

	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "StartupLastDescription", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "StartupStatus", "LastStatus");
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "UserBoxHeight", 300);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "WindowActivationMethod", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "MainConfiguration_Geometry", "50, 50, 790, 580");
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "LookChatAdvanced_Geometry", "50, 50, 620, 540");

	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "AlignUserboxIconsTop", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "AvatarBorder", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "AvatarGreyOut", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatContents", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ForceCustomChatFont", false);
	QFont chatFont = qApp->font();
#ifdef Q_OS_WIN32
	// On Windows default app font is often "MS Shell Dlg 2", and the default sans
	// family (Arial, at least in Qt 4.8) is better. Though, on X11 the default
	// sans family is the same while most users will have some nice default app
	// font, like DejaVu, Ubuntu (the font, not the distro) or alike.
	chatFont.setStyleHint(QFont::SansSerif);
	chatFont.setFamily(chatFont.defaultFamily());
#endif
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatFont", chatFont);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatBgFilled", // depends on configuration imported from older version
		Application::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatBgColor").isValid() &&
		Application::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatBgColor") != QColor("#ffffff"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatTextCustomColors", // depends on configuration imported from older version
		Application::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor").isValid() &&
		Application::instance()->configuration()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor") != QColor("#ffffff"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ChatTextFontColor", QColor("#000000"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ConferenceContents", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ConferencePrefix", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "DescriptionColor", w.palette().text().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "DisplayGroupTabs", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "HeaderSeparatorHeight", 1);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "InfoPanelFgColor", w.palette().text().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "InfoPanelBgFilled", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "InfoPanelBgColor", w.palette().base().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "InfoPanelSyntaxFile", "ultr");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "NiceDateFormat", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "NoHeaderInterval", 30);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "NoHeaderRepeat", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "NoServerTime", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "NoServerTimeDiff", 60);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "PanelFont", qApp->font());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "PanelVerticalScrollbar", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ParagraphSeparator", 4);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowAvatars", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "IconTheme", IconThemeManager::defaultTheme());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowGroupAll", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowBold", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowDesc", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowInfoPanel", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowMultilineDesc", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "ShowStatusButton", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "Style", "Satin");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxTransparency", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxAlpha", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxBlur", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxBgColor", w.palette().base().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxAlternateBgColor", w.palette().alternateBase().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserBoxColumnCount", 1);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxFgColor", w.palette().text().color());
	QFont userboxfont(qApp->font());
	userboxfont.setPointSize(qApp->font().pointSize() + 1);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UserboxFont", userboxfont);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "UseUserboxBackground", false);
#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the icon notification to animated which
	 * will prevent from blinking the dock icon
	 */
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("Look", "NewMessageIcon", 2);
#endif

	Application::instance()->configuration()->deprecatedApi()->addVariable("Network", "AllowDCC", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Network", "DefaultPort", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Network", "isDefServers", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Network", "Server", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Network", "UseProxy", false);

#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the function keys with 'apple' button
	 * as it is the default system configuration */
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_clear", "Ctrl+F9");
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_configure", "Ctrl+F2");
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_modulesmanager", "Ctrl+F4");
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showoffline", "Ctrl+F9");
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showonlydesc", "Ctrl+F10");
#else
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_clear", "F9");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_configure", "F2");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_modulesmanager", "F4");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showoffline", "F9");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_showonlydesc", "F10");
#endif
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_bold", "Ctrl+B");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_close", "Esc");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_italic", "Ctrl+I");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_deleteuser", "Del");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_persinfo", "Ins");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "kadu_exit", "Ctrl+Q");

	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "UseDefaultWebBrowser", Application::instance()->configuration()->deprecatedApi()->readEntry("Chat", "WebBrowser").isEmpty());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "UseDefaultEMailClient", Application::instance()->configuration()->deprecatedApi()->readEntry("Chat", "MailClient").isEmpty());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ContactStateChats", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitle", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitleSyntax", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitlePosition", 1);

	createAllDefaultToolbars();
}

void Core::createAllDefaultToolbars()
{
	// don't use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = Application::instance()->configuration()->api()->findElement(Application::instance()->configuration()->api()->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = Application::instance()->configuration()->api()->createElement(Application::instance()->configuration()->api()->rootElement(), "Toolbars");

	KaduWindow::createDefaultToolbars(toolbarsConfig);
	ChatEditBox::createDefaultToolbars(toolbarsConfig);
	SearchWindow::createDefaultToolbars(toolbarsConfig);

	Application::instance()->flushConfiguration();
}

void Core::init()
{
	MessageRenderInfo::registerParserTags();

	runServices();

	// protocol modules should be loaded before gui
	// it fixes crash on loading pending messages from config, contacts import from 0.6.5, and maybe other issues
	{
		auto changeNotifierLock = ChangeNotifierLock{m_injector.get<PluginStateService>()->changeNotifier()};
		m_injector.get<PluginManager>()->activateProtocolPlugins();
	}

	Myself.setAnonymous(false);
	Myself.setDisplay(Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Nick", tr("Me")));

	new Updates(this);

	QApplication::setWindowIcon(KaduIcon("kadu_icons/kadu").icon());
	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(updateIcon()));
	QTimer::singleShot(15000, this, SLOT(deleteOldConfigurationFiles()));

	// TODO: add some life-cycle management
	NotificationManager::instance();

	AccountManager::instance()->ensureLoaded();
	BuddyManager::instance()->ensureLoaded();
	ContactManager::instance()->ensureLoaded();
	// Without that UnreadMessageRepository is loaded while filtering buddies list for the first time.
	// It has to happen earlier because UnreadMessageRepository::loaded() might add buddies to the BuddyManager
	// which (the buddies) otherwise will not be taken into account by buddies list before its next update.
	CurrentUnreadMessageRepository->ensureLoaded();
	AvatarManager::instance(); // initialize that

#if WITH_LIBINDICATE_QT
	// Use a symbol from libindicate-qt so that it will not get dropped for example by --as-needed.
	(void)QIndicate::Server::defaultInstance();
#endif
}

void Core::initialized()
{
	StatusSetter::instance()->coreInitialized();
}

void Core::deleteOldConfigurationFiles()
{
	kdebugf();

	QDir oldConfigs(Application::instance()->pathsProvider()->profilePath(), "kadu-0.12.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs.count() > 20)
		for (unsigned int i = 0, max = oldConfigs.count() - 20; i < max; ++i)
			QFile::remove(Application::instance()->pathsProvider()->profilePath() + oldConfigs[static_cast<int>(i)]);

	QDir oldConfigs2(Application::instance()->pathsProvider()->profilePath(), "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
			QFile::remove(Application::instance()->pathsProvider()->profilePath() + oldConfigs2[static_cast<int>(i)]);

	QDir oldBacktraces(Application::instance()->pathsProvider()->profilePath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
			QFile::remove(Application::instance()->pathsProvider()->profilePath() + oldBacktraces[static_cast<int>(i)]);

	QDir oldDebugs(Application::instance()->pathsProvider()->profilePath(), "kadu.log.*", QDir::Name, QDir::Files);
	if (oldDebugs.count() > 20)
		for (unsigned int i = 0, max = oldDebugs.count() - 20; i < max; ++i)
			QFile::remove(Application::instance()->pathsProvider()->profilePath() + oldDebugs[static_cast<int>(i)]);

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
	if(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu",
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

	bool ok;
	int newMask = qgetenv("DEBUG_MASK").toInt(&ok);
	debug_mask = ok ? newMask : Application::instance()->configuration()->deprecatedApi()->readNumEntry("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);

	Myself.setDisplay(Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Nick", tr("Me")));
}

void Core::createGui()
{
	Window = new KaduWindow();
	connect(Window, SIGNAL(destroyed()), this, SLOT(kaduWindowDestroyed()));
	KaduWindowProvider->provideValue(Window);

	// initialize file transfers
	FileTransferManager::instance();
}

void Core::runServices()
{
	CurrentBuddyDataWindowRepository = new BuddyDataWindowRepository(this);
	CurrentChatDataWindowRepository = new ChatDataWindowRepository(this);
	CurrentChatImageRequestService = new ChatImageRequestService(this);
	CurrentDomProcessorService = new DomProcessorService(this);
	CurrentImageStorageService = new ImageStorageService(this);
	CurrentMessageFilterService = new MessageFilterService(this);
	CurrentMessageHtmlRendererService = new MessageHtmlRendererService(this);
	CurrentMessageTransformerService = new MessageTransformerService(this);
	CurrentRawMessageTransformerService = new RawMessageTransformerService(this);
	CurrentClipboardHtmlTransformerService = new ClipboardHtmlTransformerService(this);
	CurrentAccountConfigurationWidgetFactoryRepository = new AccountConfigurationWidgetFactoryRepository(this);
	CurrentBuddyConfigurationWidgetFactoryRepository = new BuddyConfigurationWidgetFactoryRepository(this);
	CurrentChatConfigurationWidgetFactoryRepository = new ChatConfigurationWidgetFactoryRepository(this);
	CurrentChatTopBarWidgetFactoryRepository = new ChatTopBarWidgetFactoryRepository(this);
	CurrentUnreadMessageRepository = new UnreadMessageRepository(this);

	auto rosterNotifier = m_injector.get<RosterNotifier>();
	for (auto &&notifyEvent : rosterNotifier->notifyEvents())
		NotificationManager::instance()->registerNotifyEvent(notifyEvent);

	CurrentChatWidgetActions = new ChatWidgetActions(this);

	auto chatWidgetContainerHandlerRepository = m_injector.get<ChatWidgetContainerHandlerRepository>();
	chatWidgetContainerHandlerRepository->registerChatWidgetContainerHandler(m_injector.get<WindowChatWidgetContainerHandler>());

	CurrentChatWidgetMessageHandler = new ChatWidgetMessageHandler(this);
	CurrentChatWidgetMessageHandler->setChatWidgetActivationService(m_injector.get<ChatWidgetActivationService>());
	CurrentChatWidgetMessageHandler->setChatWidgetManager(m_injector.get<ChatWidgetManager>());
	CurrentChatWidgetMessageHandler->setChatWidgetRepository(m_injector.get<ChatWidgetRepository>());
	CurrentChatWidgetMessageHandler->setMessageManager(MessageManager::instance());
	CurrentChatWidgetMessageHandler->setUnreadMessageRepository(CurrentUnreadMessageRepository);
	auto chatWidgetMessageHandlerConfigurator = new ChatWidgetMessageHandlerConfigurator(); // this is basically a global so we do not care about relesing it
	chatWidgetMessageHandlerConfigurator->setChatWidgetMessageHandler(CurrentChatWidgetMessageHandler);

	m_injector.get<ChatWindowStorage>()->setChatManager(ChatManager::instance());
	auto chatWindowStorageConfigurator = new ChatWindowStorageConfigurator(); // this is basically a global so we do not care about relesing it
	chatWindowStorageConfigurator->setChatWindowStorage(m_injector.get<ChatWindowStorage>());

	// this instance lives forever
	// TODO: maybe make it QObject and make CurrentChatImageRequestService its parent
	ChatImageRequestServiceConfigurator *configurator = new ChatImageRequestServiceConfigurator();
	configurator->setChatImageRequestService(CurrentChatImageRequestService);

	CurrentChatImageRequestService->setImageStorageService(CurrentImageStorageService);
	CurrentChatImageRequestService->setAccountManager(AccountManager::instance());
	CurrentChatImageRequestService->setContactManager(ContactManager::instance());

	MessageManager::instance()->setMessageFilterService(CurrentMessageFilterService);
	MessageManager::instance()->setMessageTransformerService(CurrentMessageTransformerService);
	MessageManager::instance()->setFormattedStringFactory(m_injector.get<FormattedStringFactory>());

	m_injector.get<FormattedStringFactory>()->setImageStorageService(CurrentImageStorageService);

	ChatStyleManager::instance()->setFormattedStringFactory(m_injector.get<FormattedStringFactory>());

	CurrentMessageHtmlRendererService->setDomProcessorService(CurrentDomProcessorService);
	CurrentMessageRenderInfoFactory = new MessageRenderInfoFactory();
	CurrentMessageRenderInfoFactory->setChatStyleManager(ChatStyleManager::instance());

	m_injector.get<PluginMetadataFinder>()->setDirectory(Application::instance()->pathsProvider()->dataPath() + QLatin1String{"plugins"});
	m_injector.get<PluginDependencyHandler>()->initialize();
	m_injector.get<PluginStateManager>()->loadPluginStates();

	CurrentChatStyleRendererFactoryProvider = make_owned<ConfiguredChatStyleRendererFactoryProvider>(this);

	ChatStyleManager::instance()->setConfiguredChatStyleRendererFactoryProvider(CurrentChatStyleRendererFactoryProvider.get());

	CurrentWebkitMessagesViewDisplayFactory = make_owned<WebkitMessagesViewDisplayFactory>(this);
	CurrentWebkitMessagesViewDisplayFactory->setChatStyleManager(ChatStyleManager::instance());
	CurrentWebkitMessagesViewDisplayFactory->setMessageRenderInfoFactory(CurrentMessageRenderInfoFactory);

	CurrentWebkitMessagesViewHandlerFactory = make_owned<WebkitMessagesViewHandlerFactory>(this);
	CurrentWebkitMessagesViewHandlerFactory->setChatStyleManager(ChatStyleManager::instance());
	CurrentWebkitMessagesViewHandlerFactory->setWebkitMessagesViewDisplayFactory(CurrentWebkitMessagesViewDisplayFactory.get());

	CurrentWebkitMessagesViewFactory = make_owned<WebkitMessagesViewFactory>(this);
	CurrentWebkitMessagesViewFactory->setChatImageRequestService(CurrentChatImageRequestService);
	CurrentWebkitMessagesViewFactory->setChatStyleRendererFactoryProvider(CurrentChatStyleRendererFactoryProvider.get());
	CurrentWebkitMessagesViewFactory->setImageStorageService(CurrentImageStorageService);
	CurrentWebkitMessagesViewFactory->setWebkitMessagesViewHandlerFactory(CurrentWebkitMessagesViewHandlerFactory.get());

	// instantiate = run in case of services
	m_injector.get<ChatWidgetStatePersistenceService>();

	// moved here because of #2758
	ContactManager::instance()->init();
}

void Core::runGuiServices()
{
	CurrentNotificationService = new NotificationService(this);
	CurrentChatWidgetMessageHandler->setNotificationService(CurrentNotificationService);

	m_injector.get<ChatWindowManager>()->openStoredChatWindows();
}

void Core::stopServices()
{
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

BuddyDataWindowRepository * Core::buddyDataWindowRepository() const
{
	return CurrentBuddyDataWindowRepository;
}

ChatDataWindowRepository * Core::chatDataWindowRepository() const
{
	return CurrentChatDataWindowRepository;
}

ChatImageRequestService * Core::chatImageRequestService() const
{
	return CurrentChatImageRequestService;
}

DomProcessorService * Core::domProcessorService() const
{
	return CurrentDomProcessorService;
}

ImageStorageService * Core::imageStorageService() const
{
	return CurrentImageStorageService;
}

MessageFilterService * Core::messageFilterService() const
{
	return CurrentMessageFilterService;
}

MessageHtmlRendererService * Core::messageHtmlRendererService() const
{
	return CurrentMessageHtmlRendererService;
}

MessageRenderInfoFactory * Core::messageRenderInfoFactory() const
{
	return CurrentMessageRenderInfoFactory;
}

MessageTransformerService * Core::messageTransformerService() const
{
	return CurrentMessageTransformerService;
}

NotificationService * Core::notificationService() const
{
	return CurrentNotificationService;
}

FormattedStringFactory * Core::formattedStringFactory() const
{
	return m_injector.get<FormattedStringFactory>();
}

RawMessageTransformerService * Core::rawMessageTransformerService() const
{
	return CurrentRawMessageTransformerService;
}

ClipboardHtmlTransformerService * Core::clipboardHtmlTransformerService() const
{
	return CurrentClipboardHtmlTransformerService;
}

AccountConfigurationWidgetFactoryRepository * Core::accountConfigurationWidgetFactoryRepository() const
{
	return CurrentAccountConfigurationWidgetFactoryRepository;
}

BuddyConfigurationWidgetFactoryRepository * Core::buddyConfigurationWidgetFactoryRepository() const
{
	return CurrentBuddyConfigurationWidgetFactoryRepository;
}

ChatConfigurationWidgetFactoryRepository * Core::chatConfigurationWidgetFactoryRepository() const
{
	return CurrentChatConfigurationWidgetFactoryRepository;
}

ChatTopBarWidgetFactoryRepository * Core::chatTopBarWidgetFactoryRepository() const
{
	return CurrentChatTopBarWidgetFactoryRepository;
}

UnreadMessageRepository * Core::unreadMessageRepository() const
{
	return CurrentUnreadMessageRepository;
}

RosterNotifier * Core::rosterNotifier() const
{
	return m_injector.get<RosterNotifier>();
}

ChatWidgetContainerHandlerRepository * Core::chatWidgetContainerHandlerRepository() const
{
	return m_injector.get<ChatWidgetContainerHandlerRepository>();
}

ChatWidgetActions * Core::chatWidgetActions() const
{
	return CurrentChatWidgetActions;
}

ChatWidgetManager * Core::chatWidgetManager() const
{
	return m_injector.get<ChatWidgetManager>();
}

ChatWidgetFactory * Core::chatWidgetFactory() const
{
	return m_injector.get<ChatWidgetFactory>();
}

ChatWidgetRepository * Core::chatWidgetRepository() const
{
	return m_injector.get<ChatWidgetRepository>();
}

StoragePointFactory * Core::storagePointFactory() const
{
	return m_injector.get<StoragePointFactory>();
}

PluginActivationService * Core::pluginActivationService() const
{
	return m_injector.get<PluginActivationService>();
}

PluginConflictResolver * Core::pluginConflictResolver() const
{
	return m_injector.get<PluginConflictResolver>();
}

PluginDependencyHandler * Core::pluginDependencyHandler() const
{
	return m_injector.get<PluginDependencyHandler>();
}

PluginStateManager * Core::pluginStateManager() const
{
	return m_injector.get<PluginStateManager>();
}

PluginStateService * Core::pluginStateService() const
{
	return m_injector.get<PluginStateService>();
}

ChatStyleRendererFactoryProvider * Core::chatStyleRendererFactoryProvider() const
{
	return CurrentChatStyleRendererFactoryProvider.get();
}

ConfiguredChatStyleRendererFactoryProvider * Core::configuredChatStyleRendererFactoryProvider() const
{
	return CurrentChatStyleRendererFactoryProvider.get();
}

WebkitMessagesViewDisplayFactory * Core::webkitMessagesViewDisplayFactory() const
{
	return CurrentWebkitMessagesViewDisplayFactory.get();
}

WebkitMessagesViewFactory * Core::webkitMessagesViewFactory() const
{
	return CurrentWebkitMessagesViewFactory.get();
}

WebkitMessagesViewHandlerFactory * Core::webkitMessagesViewHandlerFactory() const
{
	return CurrentWebkitMessagesViewHandlerFactory.get();
}

RosterReplacer * Core::rosterReplacer() const
{
	return m_injector.get<RosterReplacer>();
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
		UrlHandlerManager::instance()->openUrl(remoteCommand.toUtf8(), true);
}

void Core::quit()
{
	if (!Instance)
		return;

	delete Instance;
	Instance = 0;
}

#include "moc_core.cpp"
