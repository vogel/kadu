/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QApplication>

#include <QtCrypto>

#include "accounts/account-manager.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/main-configuration-holder.h"
#include "contacts/contact-manager.h"
#include "emoticons/emoticons.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/search-window.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "message/message-manager.h"
#include "misc/date-time-parser-tags.h"
#include "misc/misc.h"
#include "notify/notification-manager.h"
#include "plugins/plugins-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "status/status-setter.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
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

Core * Core::instance()
{
	if (!Instance)
	{
		Instance = new Core();
		Instance->init();
	}

	return Instance;
}

QString Core::name()
{
	return QLatin1String("Kadu");
}

QString Core::version()
{
	return QLatin1String(VERSION);
}

QString Core::nameWithVersion()
{
	return name() + QLatin1String(" ")  + version();
}

Core::Core() :
		Application(0), Myself(Buddy::create()), Window(0), IsClosing(false),
		ShowMainWindowOnStart(true), QcaInit(new QCA::Initializer())
{
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

	import_0_6_5_configuration();
	createDefaultConfiguration();
	configurationUpdated();

	MainConfigurationHolder::createInstance();

	DateTimeParserTags::registerParserTags();
}

Core::~Core()
{
	IsClosing = true;

	// unloading modules does that
	/*StatusContainerManager::instance()->disconnectAndStoreLastStatus(disconnectWithCurrentDescription, disconnectDescription);*/
	ChatWidgetManager::instance()->closeAllWindows();
	ConfigurationManager::instance()->store();
// 	delete Configuration;
// 	Configuration = 0;

	storeConfiguration();

	PluginsManager::instance()->deactivatePlugins();

#ifdef Q_OS_MAC
	setIcon(KaduIcon("kadu_icons/kadu"));
#endif // Q_OS_MAC

	QWidget *hiddenParent = Window->parentWidget();
	delete Window;
	Window = 0;
	delete hiddenParent;

	MainConfigurationHolder::destroyInstance();

	triggerAllAccountsUnregistered();

	xml_config_file->sync();

	// Sometimes it causes crash which I don't understand. For me 100% reproducible
	// if Kadu was compiled with Clang and we logged in to a jabber account. --beevvy
	// TODO: fix it
	// delete QcaInit;
	// QcaInit = 0;

	delete xml_config_file;
	delete config_file_ptr;

	xml_config_file = 0;
	config_file_ptr = 0;
}

void Core::setApplication(KaduApplication *application)
{
	Q_ASSERT(!Application);
	Q_ASSERT(application);

	Application = application;
}

KaduApplication * Core::application() const
{
	return Application;
}

void Core::import_0_6_5_configuration()
{
	config_file.addVariable("Look", "UserboxAlternateBgColor", config_file.readEntry("Look", "UserboxBgColor"));
}

void Core::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Chat", "AutoSend", true);
	config_file.addVariable("Chat", "BlinkChatTitle", true);
	config_file.addVariable("Chat", "ChatCloseTimer", true);
	config_file.addVariable("Chat", "ChatCloseTimerPeriod", 2);
#ifdef Q_WS_MAEMO_5
	config_file.addVariable("Chat", "ChatPrune", true);
#else
	config_file.addVariable("Chat", "ChatPrune", false);
#endif
	config_file.addVariable("Chat", "ChatPruneLen", 20);
	config_file.addVariable("Chat", "ConfirmChatClear", true);
	config_file.addVariable("Chat", "EmoticonsPaths", QString());
	config_file.addVariable("Chat", "EmoticonsStyle", EmoticonsStyleAnimated);
	config_file.addVariable("Chat", "EmoticonsScaling", EmoticonsScalingStatic);
#ifdef Q_WS_X11
	config_file.addVariable("Chat", "EmoticonsTheme", "penguins");
#else
	config_file.addVariable("Chat", "EmoticonsTheme", "tango");
#endif
	config_file.addVariable("Chat", "FoldLink", true);
	config_file.addVariable("Chat", "LinkFoldTreshold", 50);
	config_file.addVariable("Chat", "IgnoreAnonymousRichtext", true);
	config_file.addVariable("Chat", "IgnoreAnonymousUsers", false);
	config_file.addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	config_file.addVariable("Chat", "LastImagePath", QString(getenv("HOME")) + '/');
	config_file.addVariable("Chat", "NewMessagesInChatTitle", false);
	config_file.addVariable("Chat", "OpenChatOnMessage", false);
	config_file.addVariable("Chat", "OpenChatOnMessageWhenOnline", false);
	config_file.addVariable("Chat", "SaveOpenedWindows", true);
	config_file.addVariable("Chat", "ReceiveMessages", true);
	config_file.addVariable("Chat", "RecentChatsTimeout", 240);
	config_file.addVariable("Chat", "RecentChatsClear", false);
	config_file.addVariable("Chat", "RememberPosition", true);
	config_file.addVariable("Chat", "ShowEditWindowLabel", true);

	config_file.addVariable("General", "AllowExecutingFromParser", false);
	config_file.addVariable("General", "AutoRaise", false);
	config_file.addVariable("General", "CheckUpdates", true);
	config_file.addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	config_file.addVariable("General", "DescriptionHeight", 60);
	config_file.addVariable("General", "DisconnectWithCurrentDescription", true);
#ifdef Q_WS_WIN
	config_file.addVariable("General", "HideMainWindowFromTaskbar", false);
#endif
	config_file.addVariable("General", "Language",  QLocale::system().name().left(2));
	config_file.addVariable("General", "Nick", tr("Me"));
	config_file.addVariable("General", "NumberOfDescriptions", 20);
	config_file.addVariable("General", "ParseStatus", false);
	config_file.addVariable("General", "SaveStdErr", false);
	config_file.addVariable("General", "ShowBlocked", true);
	config_file.addVariable("General", "ShowBlocking", true);
	config_file.addVariable("General", "ShowMyself", false);
	config_file.addVariable("General", "ShowEmotPanel", true);
	config_file.addVariable("General", "ShowOffline", true);
	config_file.addVariable("General", "ShowOnlineAndDescription", false);
	config_file.addVariable("General", "ShowWithoutDescription", true);
	config_file.addVariable("General", "StartDelay", 0);

	if (config_file.readBoolEntry("General", "AdvancedMode", false))
	{
		config_file.addVariable("General", "StatusContainerType", "Account");
		config_file.addVariable("Look", "ShowExpandingControl", true);
	}
	else
	{
		config_file.addVariable("General", "StatusContainerType", "Identity");
		config_file.addVariable("Look", "ShowExpandingControl", false);
	}

	config_file.addVariable("General", "StartupLastDescription", true);
	config_file.addVariable("General", "StartupStatus", "LastStatus");
	config_file.addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", false);
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "WindowActivationMethod", 0);
	config_file.addVariable("General", "MainConfiguration_Geometry", "50, 50, 790, 580");
	config_file.addVariable("General", "LookChatAdvanced_Geometry", "50, 50, 620, 540");

	config_file.addVariable("Look", "AlignUserboxIconsTop", true);
	config_file.addVariable("Look", "AvatarBorder", false);
	config_file.addVariable("Look", "AvatarGreyOut", true);
	config_file.addVariable("Look", "ChatContents", QString());
	config_file.addVariable("Look", "ForceCustomChatFont", false);
	QFont chatFont = qApp->font();
#ifdef Q_WS_WIN
	// On Windows default app font is often "MS Shell Dlg 2", and the default sans
	// family (Arial, at least in Qt 4.8) is better. Though, on X11 the default
	// sans family is the same while most users will have some nice default app
	// font, like DejaVu, Ubuntu (the font, not the distro) or alike.
	chatFont.setStyleHint(QFont::SansSerif);
	chatFont.setFamily(chatFont.defaultFamily());
#endif
	config_file.addVariable("Look", "ChatFont", chatFont);
	config_file.addVariable("Look", "ChatBgFilled", // depends on configuration imported from older version
		config_file.readColorEntry("Look", "ChatBgColor").isValid() &&
		config_file.readColorEntry("Look", "ChatBgColor") != QColor("#ffffff"));
	config_file.addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatTextCustomColors", // depends on configuration imported from older version
		config_file.readColorEntry("Look", "ChatTextBgColor").isValid() &&
		config_file.readColorEntry("Look", "ChatTextBgColor") != QColor("#ffffff"));
	config_file.addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatTextFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ConferenceContents", QString());
	config_file.addVariable("Look", "ConferencePrefix", QString());
	config_file.addVariable("Look", "DescriptionColor", w.palette().text().color());
	config_file.addVariable("Look", "DisplayGroupTabs", true);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "IconsPaths", QString());
	config_file.addVariable("Look", "InfoPanelFgColor", w.palette().text().color());
	config_file.addVariable("Look", "InfoPanelBgFilled", false);
	config_file.addVariable("Look", "InfoPanelBgColor", w.palette().base().color());
	config_file.addVariable("Look", "InfoPanelSyntaxFile", "ultr");
	config_file.addVariable("Look", "NiceDateFormat", true);
	config_file.addVariable("Look", "NoHeaderInterval", 30);
	config_file.addVariable("Look", "NoHeaderRepeat", true);
	config_file.addVariable("Look", "NoServerTime", true);
	config_file.addVariable("Look", "NoServerTimeDiff", 60);
	config_file.addVariable("Look", "PanelFont", qApp->font());
	config_file.addVariable("Look", "PanelVerticalScrollbar", false);
	config_file.addVariable("Look", "ParagraphSeparator", 4);
#ifdef Q_WS_MAEMO_5
	config_file.addVariable("Look", "ShowAvatars", false);
	config_file.addVariable("Look", "IconTheme", "glass22");
#else
	config_file.addVariable("Look", "ShowAvatars", true);
	config_file.addVariable("Look", "IconTheme", "default");
#endif
	config_file.addVariable("Look", "ShowGroupAll", true);
	config_file.addVariable("Look", "ShowBold", true);
	config_file.addVariable("Look", "ShowDesc", true);
	config_file.addVariable("Look", "ShowInfoPanel", false);
	config_file.addVariable("Look", "ShowMultilineDesc", true);
	config_file.addVariable("Look", "ShowStatusButton", true);
	config_file.addVariable("Look", "Style", "Satin");
	config_file.addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	config_file.addVariable("Look", "UserboxTransparency", false);
	config_file.addVariable("Look", "UserboxAlpha", 0);
	config_file.addVariable("Look", "UserboxBlur", true);
	config_file.addVariable("Look", "UserboxBgColor", w.palette().base().color());
	config_file.addVariable("Look", "UserboxAlternateBgColor", w.palette().alternateBase().color());
	config_file.addVariable("Look", "UserBoxColumnCount", 1);
	config_file.addVariable("Look", "UserboxFgColor", w.palette().text().color());
	QFont userboxfont(qApp->font());
	userboxfont.setPointSize(qApp->font().pointSize() + 1);
	config_file.addVariable("Look", "UserboxFont", userboxfont);
	config_file.addVariable("Look", "UseUserboxBackground", false);
#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the icon notification to animated which
	 * will prevent from blinking the dock icon
	 */
	config_file.addVariable("Look", "NewMessageIcon", 2);
#endif

	config_file.addVariable("Network", "AllowDCC", true);
	config_file.addVariable("Network", "DefaultPort", 0);
	config_file.addVariable("Network", "isDefServers", true);
	config_file.addVariable("Network", "Server", QString());
	config_file.addVariable("Network", "UseProxy", false);

#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the function keys with 'apple' button
	 * as it is the default system configuration */
	config_file.addVariable("ShortCuts", "chat_clear", "Ctrl+F9");
	config_file.addVariable("ShortCuts", "kadu_configure", "Ctrl+F2");
	config_file.addVariable("ShortCuts", "kadu_modulesmanager", "Ctrl+F4");
	config_file.addVariable("ShortCuts", "kadu_showoffline", "Ctrl+F9");
	config_file.addVariable("ShortCuts", "kadu_showonlydesc", "Ctrl+F10");
#else
	config_file.addVariable("ShortCuts", "chat_clear", "F9");
	config_file.addVariable("ShortCuts", "kadu_configure", "F2");
	config_file.addVariable("ShortCuts", "kadu_modulesmanager", "F4");
	config_file.addVariable("ShortCuts", "kadu_showoffline", "F9");
	config_file.addVariable("ShortCuts", "kadu_showonlydesc", "F10");
#endif
	config_file.addVariable("ShortCuts", "chat_bold", "Ctrl+B");
	config_file.addVariable("ShortCuts", "chat_close", "Esc");
	config_file.addVariable("ShortCuts", "chat_italic", "Ctrl+I");
	config_file.addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	config_file.addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	config_file.addVariable("ShortCuts", "kadu_deleteuser", "Del");
	config_file.addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	config_file.addVariable("ShortCuts", "kadu_persinfo", "Ins");
	config_file.addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");
	config_file.addVariable("ShortCuts", "kadu_exit", "Ctrl+Q");

	config_file.addVariable("Chat", "UseDefaultWebBrowser", config_file.readEntry("Chat", "WebBrowser").isEmpty());
	config_file.addVariable("Chat", "UseDefaultEMailClient", config_file.readEntry("Chat", "MailClient").isEmpty());
	config_file.addVariable("Chat", "ContactStateChats", true);
	config_file.addVariable("Chat", "ContactStateWindowTitle", true);
	config_file.addVariable("Chat", "ContactStateWindowTitleSyntax", QString());
	config_file.addVariable("Chat", "ContactStateWindowTitlePosition", 1);

	createAllDefaultToolbars();
}

void Core::createAllDefaultToolbars()
{
	// don't use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	KaduWindow::createDefaultToolbars(toolbarsConfig);
	ChatEditBox::createDefaultToolbars(toolbarsConfig);
	SearchWindow::createDefaultToolbars(toolbarsConfig);

	xml_config_file->sync();
}

void Core::init()
{
	// protocol modules should be loaded before gui
	// it fixes crash on loading pending messages from config, contacts import from 0.6.5, and maybe other issues
	PluginsManager::instance()->activateProtocolPlugins();

	Myself.setAnonymous(false);
	Myself.setDisplay(config_file.readEntry("General", "Nick", tr("Me")));

	connect(StatusContainerManager::instance(), SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));

	new Updates(this);

	setIcon(KaduIcon(QLatin1String("protocols/common/offline")));
	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(statusUpdated()));
	QTimer::singleShot(15000, this, SLOT(deleteOldConfigurationFiles()));

	// TODO: add some life-cycle management
	NotificationManager::instance();

	AccountManager::instance()->ensureLoaded();
	BuddyManager::instance()->ensureLoaded();
	ContactManager::instance()->ensureLoaded();
	// Without that MessageManager is loaded while filtering buddies list for the first time.
	// It has to happen earlier because MessageManager::loaded() might add buddies to the BuddyManager
	// which (the buddies) otherwise will not be taken into account by buddies list before its next update.
	MessageManager::instance()->ensureLoaded();
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

void Core::storeConfiguration()
{
	xml_config_file->makeBackup();
}

char *SystemUserName;
void Core::deleteOldConfigurationFiles()
{
	kdebugf();

	QDir oldConfigs2(profilePath(), "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files);

	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
			QFile::remove(profilePath(oldConfigs2[i]));

	QDir oldBacktraces(profilePath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
			QFile::remove(profilePath(oldBacktraces[i]));

#ifdef Q_OS_WIN
	QString tmp(getenv("TEMP") ? getenv("TEMP") : ".");
	QString mask("kadu-dbg-*.txt");
#else
	QString tmp("/tmp");
	QString mask=QString("kadu-%1-*.dbg").arg(SystemUserName);
#endif

	QDir oldDebugs(tmp, mask, QDir::Name, QDir::Files);
	if (oldDebugs.count() > 5)
		for (unsigned int i = 0, max = oldDebugs.count() - 5; i < max; ++i)
			QFile::remove(tmp + '/' + oldDebugs[i]);

	kdebugf2();
}

void Core::statusUpdated()
{
	if (isClosing())
		return;

	setIcon(StatusContainerManager::instance()->statusIcon());
}

void Core::kaduWindowDestroyed()
{
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
	{
		disconnect(protocol, SIGNAL(connecting(Account)), this, SIGNAL(connecting()));
		disconnect(protocol, SIGNAL(connected(Account)), this, SIGNAL(connected()));
		disconnect(protocol, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
	}
}

void Core::configurationUpdated()
{
#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		       QSettings::NativeFormat);
	if(config_file.readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu",
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

	debug_mask = config_file.readNumEntry("General", "DEBUG_MASK");

	Myself.setDisplay(config_file.readEntry("General", "Nick", tr("Me")));
}

void Core::createGui()
{
	Window = new KaduWindow();
	connect(Window, SIGNAL(destroyed()), this, SLOT(kaduWindowDestroyed()));

	// initialize file transfers
	FileTransferManager::instance();
}

void Core::showMainWindow()
{
	if (ShowMainWindowOnStart)
		Window->show();

	// after first call which has to be placed in main(), this method should always show Window
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

void Core::setIcon(const KaduIcon &icon)
{
	bool blocked = false;
	emit settingMainIconBlocked(blocked);

	if (!blocked)
	{
		QApplication::setWindowIcon(icon.icon());
		foreach (QWidget *window, QApplication::topLevelWidgets())
			if (window->property("ownWindowIcon").toBool() == false)
				window->setWindowIcon(icon.icon());
		if (Window && !Window->isWindow())
			Window->setWindowIcon(icon.icon());

		emit mainIconChanged(icon);
	}
}

void Core::receivedSignal(const QString &signal)
{
	if ("activate" == signal)
		_activateWindow(Window);
	else
		UrlHandlerManager::instance()->openUrl(signal.toUtf8(), true);
}

void Core::quit()
{
	if (!Instance)
		return;

	delete Instance;
	Instance = 0;
}
