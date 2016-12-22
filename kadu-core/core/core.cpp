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

#include "core.h"

#include "actions/actions.h"
#include "avatars/avatar-manager.h"
#include "chat-style/chat-style-configuration-ui-handler.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-manager.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/gui/configuration-ui-handler-repository.h"
#include "contacts/contact-parser-tags.h"
#include "core/application.h"
#include "core/injected-factory.h"
#include "core/injector-provider.h"
#include "core/session-service.h"
#include "core/version-service.h"
#include "execution-arguments/execution-arguments.h"
#include "file-transfer/file-transfer-handler-manager.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "icons/icons-manager.h"
#include "message/message-html-renderer-service.h"
#include "message/message-render-info.h"
#include "misc/change-notifier-lock.h"
#include "misc/date-time-parser-tags.h"
#include "misc/paths-provider.h"
#include "notification/notification-event-repository.h"
#include "notification/notify-configuration-importer.h"
#include "os/single-application/single-application.h"
#include "parser/parser.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/metadata/plugin-metadata-finder.h"
#include "plugin/plugin-manager.h"
#include "plugin/state/plugin-state-manager.h"
#include "plugin/state/plugin-state-service.h"
#include "roster/roster-notifier.h"
#include "services/chat-image-request-service-configurator.h"
#include "services/chat-image-request-service.h"
#include "ssl/ssl-certificate-manager.h"
#include "themes/icon-theme-manager.h"
#include "url-handlers/url-handler-manager.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "widgets/chat-widget/chat-widget-manager.h"
#include "widgets/chat-widget/chat-widget-message-handler-configurator.h"
#include "widgets/chat-widget/chat-widget-message-handler.h"
#include "widgets/chat-widget/chat-widget-repository.h"
#include "windows/chat-window/chat-window-manager.h"
#include "windows/chat-window/chat-window-storage-configurator.h"
#include "windows/chat-window/chat-window-storage.h"
#include "windows/chat-window/window-chat-widget-container-handler.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"
#include "windows/search-window.h"
#include "activate.h"
#include "debug.h"
#include "injeqt-type-roles.h"
#include "updates.h"

#ifndef Q_OS_WIN
#	include "os/unix/unix-signal-handler.h"
#endif

#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QUuid>
#include <QtWidgets/QApplication>

#ifdef Q_OS_WIN
#	include <QtCore/QSettings>
#endif

Core::Core(injeqt::injector &&injector) :
		m_injector{std::move(injector)}
{
	// must be created first
	// TODO: should be maybe created by factory factory?
	m_injector.get<InjectorProvider>()->setInjector(&m_injector);
	m_injector.instantiate_all_with_type_role(STARTUP);

	createDefaultConfiguration();
	configurationUpdated();

	m_injector.get<Parser>()->GlobalVariables.insert(QStringLiteral("DATA_PATH"), m_injector.get<PathsProvider>()->dataPath());
	m_injector.get<Parser>()->GlobalVariables.insert(QStringLiteral("HOME"), PathsProvider::homePath());
	m_injector.get<Parser>()->GlobalVariables.insert(QStringLiteral("KADU_CONFIG"), m_injector.get<PathsProvider>()->profilePath());
	DateTimeParserTags::registerParserTags(m_injector.get<Parser>());

	m_injector.get<NotifyConfigurationImporter>()->import();

	init();
}

Core::~Core()
{
	m_injector.get<SessionService>()->setIsClosing(true);

	m_injector.get<PluginStateManager>()->storePluginStates();
	// CurrentPluginStateManager->storePluginStates();

	// unloading modules does that
	/*statusContainerManager()->disconnectAndStoreLastStatus(disconnectWithCurrentDescription, disconnectDescription);*/
	m_injector.get<SslCertificateManager>()->storePersistentSslCertificates();
	m_injector.get<ChatWindowManager>()->storeOpenedChatWindows();

	// some plugins crash on deactivation
	// ensure we have at least some configuration stored
	m_injector.get<ConfigurationManager>()->flush();
	m_injector.get<Application>()->backupConfiguration();

	m_injector.get<PluginManager>()->deactivatePlugins();
	// CurrentPluginManager->deactivatePlugins();

	stopServices();

	m_injector.get<ConfigurationManager>()->flush();
	m_injector.get<Application>()->backupConfiguration();
}

void Core::createDefaultConfiguration()
{
	QWidget w;

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "AutoSend", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "BlinkChatTitle", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ChatCloseTimer", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ChatCloseTimerPeriod", 2);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ChatPrune", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ChatPruneLen", 0);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ConfirmChatClear", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousRichtext", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousUsers", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "LastImagePath", QDir::homePath() + '/');
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "NewMessagesInChatTitle", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessage", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageWhenOnline", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "OpenChatOnMessageMinimized", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "SaveOpenedWindows", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ReceiveMessages", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "RememberPosition", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ShowEditWindowLabel", true);

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "AllowExecutingFromParser", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "CheckUpdates", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "DescriptionHeight", 60);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "DisconnectWithCurrentDescription", true);
#ifdef Q_OS_WIN
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "HideMainWindowFromTaskbar", false);
#endif
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "Language",  QLocale::system().name().left(2));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "Nick", tr("Me"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "NumberOfDescriptions", 20);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "ParseStatus", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "ShowBlocked", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "ShowBlocking", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "ShowMyself", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "ShowOffline", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "ShowOnlineAndDescription", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "ShowWithoutDescription", true);

	if (m_injector.get<Configuration>()->deprecatedApi()->readBoolEntry("General", "AdvancedMode", false))
	{
		m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "StatusContainerType", "Account");
		m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowExpandingControl", true);
	}
	else
	{
		m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "StatusContainerType", "Identity");
		m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowExpandingControl", false);
	}

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "StartupLastDescription", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "StartupStatus", "LastStatus");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "UserBoxHeight", 300);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "WindowActivationMethod", 0);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "MainConfiguration_Geometry", "50, 50, 790, 580");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("General", "LookChatAdvanced_Geometry", "50, 50, 620, 540");

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "AlignUserboxIconsTop", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "AvatarBorder", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "AvatarGreyOut", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ForceCustomChatFont", false);
	QFont chatFont = qApp->font();
#ifdef Q_OS_WIN
	// On Windows default app font is often "MS Shell Dlg 2", and the default sans
	// family (Arial, at least in Qt 4.8) is better. Though, on X11 the default
	// sans family is the same while most users will have some nice default app
	// font, like DejaVu, Ubuntu (the font, not the distro) or alike.
	chatFont.setStyleHint(QFont::SansSerif);
	chatFont.setFamily(chatFont.defaultFamily());
#endif
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatFont", chatFont);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatBgFilled", // depends on configuration imported from older version
		m_injector.get<Configuration>()->deprecatedApi()->readColorEntry("Look", "ChatBgColor").isValid() &&
		m_injector.get<Configuration>()->deprecatedApi()->readColorEntry("Look", "ChatBgColor") != QColor("#ffffff"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatTextCustomColors", // depends on configuration imported from older version
		m_injector.get<Configuration>()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor").isValid() &&
		m_injector.get<Configuration>()->deprecatedApi()->readColorEntry("Look", "ChatTextBgColor") != QColor("#ffffff"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ChatTextFontColor", QColor("#000000"));
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "DescriptionColor", w.palette().text().color());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "DisplayGroupTabs", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "HeaderSeparatorHeight", 1);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "InfoPanelFgColor", w.palette().text().color());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "InfoPanelBgFilled", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "InfoPanelBgColor", w.palette().base().color());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "InfoPanelSyntaxFile", "ultr");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "NiceDateFormat", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "NoHeaderInterval", 30);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "NoHeaderRepeat", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "NoServerTime", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "NoServerTimeDiff", 60);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "PanelFont", qApp->font());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "PanelVerticalScrollbar", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ParagraphSeparator", 4);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowAvatars", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "IconTheme", IconThemeManager::defaultTheme());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowGroupAll", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowBold", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowDesc", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowInfoPanel", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowMultilineDesc", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "ShowStatusButton", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "Style", "Satin");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxTransparency", false);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxAlpha", 0);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxBlur", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxBgColor", w.palette().base().color());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxAlternateBgColor", w.palette().alternateBase().color());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserBoxColumnCount", 1);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxFgColor", w.palette().text().color());
	QFont userboxfont(qApp->font());
	userboxfont.setPointSize(qApp->font().pointSize() + 1);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UserboxFont", userboxfont);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Look", "UseUserboxBackground", false);

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Network", "DefaultPort", 0);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Network", "isDefServers", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Network", "Server", QString());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Network", "UseProxy", false);

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "chat_clear", "F9");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_configure", "F2");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_modulesmanager", "F4");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_showoffline", "F9");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_showonlydesc", "F10");

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "chat_bold", "Ctrl+B");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "chat_close", "Esc");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "chat_italic", "Ctrl+I");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_deleteuser", "Del");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_persinfo", "Ins");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("ShortCuts", "kadu_exit", "Ctrl+Q");

	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "UseDefaultWebBrowser", m_injector.get<Configuration>()->deprecatedApi()->readEntry("Chat", "WebBrowser").isEmpty());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "UseDefaultEMailClient", m_injector.get<Configuration>()->deprecatedApi()->readEntry("Chat", "MailClient").isEmpty());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ContactStateChats", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitle", true);
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitleSyntax", QString());
	m_injector.get<Configuration>()->deprecatedApi()->addVariable("Chat", "ContactStateWindowTitlePosition", 1);

	createAllDefaultToolbars();
}

void Core::createAllDefaultToolbars()
{
	// don't use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = m_injector.get<Configuration>()->api()->findElement(m_injector.get<Configuration>()->api()->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = m_injector.get<Configuration>()->api()->createElement(m_injector.get<Configuration>()->api()->rootElement(), "Toolbars");

	KaduWindow::createDefaultToolbars(m_injector.get<Configuration>(), toolbarsConfig);
	ChatEditBox::createDefaultToolbars(m_injector.get<Configuration>(), toolbarsConfig);
	SearchWindow::createDefaultToolbars(m_injector.get<Configuration>(), toolbarsConfig);

	m_injector.get<Application>()->flushConfiguration();
}

void Core::init()
{
	MessageRenderInfo::registerParserTags(m_injector.get<Parser>(), m_injector.get<ChatConfigurationHolder>(), m_injector.get<MessageHtmlRendererService>());

	runServices();

	// protocol modules should be loaded before gui
	// it fixes crash on loading pending messages from config, contacts import from 0.6.5, and maybe other issues
	{
		auto changeNotifierLock = ChangeNotifierLock{m_injector.get<PluginStateService>()->changeNotifier()};
		m_injector.get<PluginManager>()->activateProtocolPlugins();
	}

	m_injector.get<InjectedFactory>()->makeInjected<Updates>(this);

	QApplication::setWindowIcon(m_injector.get<IconsManager>()->iconByPath(KaduIcon("kadu_icons/kadu")));
	connect(m_injector.get<IconsManager>(), SIGNAL(themeChanged()), this, SLOT(updateIcon()));
	QTimer::singleShot(15000, this, SLOT(deleteOldConfigurationFiles()));

	// TODO: add some life-cycle management
	m_injector.instantiate<AvatarManager>();
}

void Core::deleteOldConfigurationFiles()
{
	kdebugf();

	QDir oldConfigs(m_injector.get<PathsProvider>()->profilePath(), "kadu-4.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs.count() > 20)
		for (unsigned int i = 0, max = oldConfigs.count() - 20; i < max; ++i)
			QFile::remove(m_injector.get<PathsProvider>()->profilePath() + oldConfigs[static_cast<int>(i)]);

	QDir oldConfigs1(m_injector.get<PathsProvider>()->profilePath(), "kadu-0.12.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs1.count() > 20)
		for (unsigned int i = 0, max = oldConfigs1.count() - 20; i < max; ++i)
			QFile::remove(m_injector.get<PathsProvider>()->profilePath() + oldConfigs1[static_cast<int>(i)]);

	QDir oldConfigs2(m_injector.get<PathsProvider>()->profilePath(), "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
			QFile::remove(m_injector.get<PathsProvider>()->profilePath() + oldConfigs2[static_cast<int>(i)]);

	QDir oldBacktraces(m_injector.get<PathsProvider>()->profilePath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
			QFile::remove(m_injector.get<PathsProvider>()->profilePath() + oldBacktraces[static_cast<int>(i)]);

	QDir oldDebugs(m_injector.get<PathsProvider>()->profilePath(), "kadu.log.*", QDir::Name, QDir::Files);
	if (oldDebugs.count() > 20)
		for (unsigned int i = 0, max = oldDebugs.count() - 20; i < max; ++i)
			QFile::remove(m_injector.get<PathsProvider>()->profilePath() + oldDebugs[static_cast<int>(i)]);

	kdebugf2();
}

void Core::updateIcon()
{
	if (m_injector.get<SessionService>()->isClosing())
		return;

	QApplication::setWindowIcon(m_injector.get<IconsManager>()->iconByPath(KaduIcon("kadu_icons/kadu")));
}

void Core::accountAdded(Account account)
{
	connect(account, SIGNAL(connecting()), this, SIGNAL(connecting()));
	connect(account, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(account, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

void Core::accountRemoved(Account account)
{
	disconnect(account, 0, this, 0);
}

void Core::configurationUpdated()
{
#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		       QSettings::NativeFormat);
	if(m_injector.get<Configuration>()->deprecatedApi()->readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu",
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

	bool ok;
	int newMask = qgetenv("DEBUG_MASK").toInt(&ok);
	debug_mask = ok ? newMask : m_injector.get<Configuration>()->deprecatedApi()->readNumEntry("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
}

int Core::executeSingle(const ExecutionArguments &executionArguments)
{
#ifndef Q_OS_WIN
	m_injector.get<UnixSignalHandler>()->startSignalHandling();
#endif

	auto ret = 0;
	auto applicationId = QString{"kadu-%1"}.arg(m_injector.get<PathsProvider>()->profilePath());

	auto executeAsFirst = [&](){
		execute(executionArguments.openIds(), executionArguments.openUuid());
		ret = QApplication::exec();
		kdebugm(KDEBUG_INFO, "after exec\n");
		kdebugm(KDEBUG_INFO, "exiting main\n");
	};

	auto executeAsNext = [&](SingleApplication &singleApplication){
		if (!executionArguments.openIds().isEmpty() || !executionArguments.openUuid().isEmpty())
		{
			for (auto const &id : executionArguments.openIds())
				singleApplication.sendMessage(id, 1000);
			if (!executionArguments.openUuid().isEmpty())
				singleApplication.sendMessage(executionArguments.openUuid(), 1000);
		}
		else
			singleApplication.sendMessage("activate", 1000);

		ret = 1;
	};

	auto receivedMessage = [&](const QString &message){
		executeRemoteCommand(message);
	};

	SingleApplication singleApplication{applicationId, executeAsFirst, executeAsNext, receivedMessage};

	return ret;
}

void Core::execute(const QStringList &openIds, const QString &openUuid)
{
	createGui();
	runGuiServices();
	activatePlugins();

	for (auto const &id : openIds)
		executeRemoteCommand(id);
	if (!openUuid.isEmpty())
		executeRemoteCommand(openUuid);

	// it has to be called after loading modules (docking might want to block showing the window)
	m_injector.get<KaduWindowService>()->showMainWindow();
}

void Core::createGui()
{
	m_injector.get<KaduWindowService>()->createWindow();

	// initialize file transfers
	m_injector.instantiate<FileTransferHandlerManager>();
	m_injector.instantiate<FileTransferManager>();

	m_injectorRegisteredActions = std::make_unique<InjectorRegisteredActions>(*m_injector.get<Actions>(), m_injector);
}

void Core::runServices()
{
	m_injector.instantiate_all_with_type_role(SERVICE);

	m_injector.instantiate<ContactParserTags>();

	auto rosterNotifier = m_injector.get<RosterNotifier>();
	for (auto &&notifyEvent : rosterNotifier->notifyEvents())
		m_injector.get<NotificationEventRepository>()->addNotificationEvent(notifyEvent);

	auto chatWidgetContainerHandlerRepository = m_injector.get<ChatWidgetContainerHandlerRepository>();
	chatWidgetContainerHandlerRepository->registerChatWidgetContainerHandler(m_injector.get<WindowChatWidgetContainerHandler>());

	m_injector.get<ChatWidgetMessageHandlerConfigurator>();

	auto chatWindowStorageConfigurator = new ChatWindowStorageConfigurator(m_injector.get<Configuration>()); // this is basically a global so we do not care about relesing it
	chatWindowStorageConfigurator->setChatWindowStorage(m_injector.get<ChatWindowStorage>());

	// this instance lives forever
	// TODO: maybe make it QObject and make CurrentChatImageRequestService its parent
	auto configurator = new ChatImageRequestServiceConfigurator(m_injector.get<Configuration>());
	configurator->setChatImageRequestService(m_injector.get<ChatImageRequestService>());

	m_injector.get<PluginMetadataFinder>()->setDirectory(m_injector.get<PathsProvider>()->dataPath() + QStringLiteral("plugins"));
	m_injector.get<PluginStateManager>()->loadPluginStates();

	m_injector.get<ConfigurationUiHandlerRepository>()->addConfigurationUiHandler(m_injector.get<ChatStyleConfigurationUiHandler>());

	m_injector.instantiate_all_with_type_role(LISTENER);
}

void Core::runGuiServices()
{
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

void Core::executeRemoteCommand(const QString &remoteCommand)
{
	if ("activate" == remoteCommand)
		_activateWindow(m_injector.get<Configuration>(), m_injector.get<KaduWindowService>()->mainWindowProvider()->provide());
	else if (!remoteCommand.startsWith("{"))
		m_injector.get<UrlHandlerManager>()->openUrl(remoteCommand.toUtf8(), true);
	else
	{
		auto uuid = QUuid{remoteCommand};
		if (uuid.isNull())
			return;
		auto chat = m_injector.get<ChatManager>()->byUuid(uuid);
		if (!chat)
			return;
		m_injector.get<ChatWidgetManager>()->openChat(chat, OpenChatActivation::Activate);
	}
}

#include "moc_core.cpp"
