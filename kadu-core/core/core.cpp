/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtGui/QApplication>
#include <QtCore/QSettings>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "contacts/contact-manager.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "misc/misc.h"
#include "notify/notification-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/services/chat-service.h"
#include "status/status-container-manager.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "debug.h"
#include "emoticons.h"
#include "icons-manager.h"
#include "modules.h"
#include "status_changer.h"
#include "updates.h"

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

Core::Core() : Myself(Buddy::create()), Window(0), ShowMainWindowOnStart(true)
{
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));
	createDefaultConfiguration();
	configurationUpdated();
}

Core::~Core()
{
	ConfigurationManager::instance()->store();
// 	delete Configuration;
// 	Configuration = 0;

	storeConfiguration();

	StatusChangerManager::instance()->unregisterStatusChanger(StatusChanger);
	delete StatusChanger;
	StatusChanger = 0;

	ModulesManager::instance()->unloadAllModules();
	Updates::closeModule();

#ifdef Q_OS_MACX
	setIcon(QPixmap(dataPath("kadu.png")));
#endif // Q_OS_MACX

	delete Window;
	Window = 0;

	triggerAllAccountsUnregistered();

	xml_config_file->sync();

	delete xml_config_file;
	delete config_file_ptr;

	xml_config_file = 0;
	config_file_ptr = 0;
}

void Core::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Chat", "ActivateWithNewMessages", false);
	config_file.addVariable("Chat", "AutoSend", true);
	config_file.addVariable("Chat", "BlinkChatTitle", true);
	config_file.addVariable("Chat", "ChatCloseTimer", true);
	config_file.addVariable("Chat", "ChatCloseTimerPeriod", 2);
	config_file.addVariable("Chat", "ChatPrune", false);
	config_file.addVariable("Chat", "ChatPruneLen", 20);
	config_file.addVariable("Chat", "ConfirmChatClear", true);
	config_file.addVariable("Chat", "EmoticonsPaths", "");
	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	config_file.addVariable("Chat", "EmoticonsTheme", "penguins");
	config_file.addVariable("Chat", "FoldLink", true);
	config_file.addVariable("Chat", "LinkFoldTreshold", 50);
	config_file.addVariable("Chat", "IgnoreAnonymousRichtext", true);
	config_file.addVariable("Chat", "IgnoreAnonymousUsers", false);
	config_file.addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	config_file.addVariable("Chat", "LastImagePath", QString(getenv("HOME")) + '/');
	config_file.addVariable("Chat", "MaxImageRequests", 5);
	config_file.addVariable("Chat", "MaxImageSize", 255);
	config_file.addVariable("Chat", "MessageAcks", false);
	config_file.addVariable("Chat", "NewMessagesInChatTitle", false);
	config_file.addVariable("Chat", "OpenChatOnMessage", true);
	config_file.addVariable("Chat", "OpenChatOnMessageWhenOnline", false);
	config_file.addVariable("Chat", "SaveOpenedWindows", true);
	config_file.addVariable("Chat", "ReceiveMessages", true);
	config_file.addVariable("Chat", "RecentChatsTimeout", 180);
	config_file.addVariable("Chat", "ReceiveImagesDuringInvisibility", true);
	config_file.addVariable("Chat", "RememberPosition", true);
	config_file.addVariable("Chat", "ShowEditWindowLabel", true);

	config_file.addVariable("General", "AllowExecutingFromParser", false);
	config_file.addVariable("General", "AutoRaise", false);
	config_file.addVariable("General", "CheckUpdates", true);
	config_file.addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	config_file.addVariable("General", "DescriptionHeight", 60);
	config_file.addVariable("General", "DisconnectWithCurrentDescription", true);
	config_file.addVariable("General", "HideBaseModules", true);
	config_file.addVariable("General", "Language",  QString(qApp->keyboardInputLocale().name()).mid(0,2));
	config_file.addVariable("General", "Nick", tr("Me"));
	config_file.addVariable("General", "NumberOfDescriptions", 20);
	config_file.addVariable("General", "ParseStatus", false);
	config_file.addVariable("General", "PrivateStatus", false);
	config_file.addVariable("General", "SaveStdErr", false);
	config_file.addVariable("General", "ShowBlocked", true);
	config_file.addVariable("General", "ShowBlocking", true);
	config_file.addVariable("General", "ShowEmotPanel", true);
	config_file.addVariable("General", "ShowOffline", true);
	config_file.addVariable("General", "ShowOnlineAndDescription", false);
	config_file.addVariable("General", "ShowWithoutDescription", true);
	config_file.addVariable("General", "StartDelay", 0);
	config_file.addVariable("General", "StartupLastDescription", true);
	config_file.addVariable("General", "StartupStatus", "LastStatus");
	config_file.addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", true);
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "WindowActivationMethod", 0);

	config_file.addVariable("Look", "AlignUserboxIconsTop", true);
	config_file.addVariable("Look", "ChatContents", "");
	config_file.addVariable("Look", "ChatFont", qApp->font());
	config_file.addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ConferenceContents", "");
	config_file.addVariable("Look", "ConferencePrefix", "");
	config_file.addVariable("Look", "DescriptionColor", w.palette().foreground().color());
	config_file.addVariable("Look", "DisplayGroupTabs", true);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "IconsPaths", "");
	config_file.addVariable("Look", "IconTheme", "default");
	config_file.addVariable("Look", "InfoPanelBgColor", w.palette().background().color());
	config_file.addVariable("Look", "InfoPanelFgColor", w.palette().foreground().color());
	config_file.addVariable("Look", "InfoPanelSyntaxFile", "default");
	config_file.addVariable("Look", "NiceDateFormat", true);
	config_file.addVariable("Look", "NoHeaderInterval", 30);
	config_file.addVariable("Look", "NoHeaderRepeat", false);
	config_file.addVariable("Look", "NoServerTime", true);
	config_file.addVariable("Look", "NoServerTimeDiff", 60);
	config_file.addVariable("Look", "PanelFont", qApp->font());
	config_file.addVariable("Look", "PanelVerticalScrollBar", false);
	config_file.addVariable("Look", "ParagraphSeparator", 4);
	config_file.addVariable("Look", "ShowGroupAll", true);
	config_file.addVariable("Look", "ShowBold", true);
	config_file.addVariable("Look", "ShowDesc", true);
	config_file.addVariable("Look", "ShowInfoPanel", true);
	config_file.addVariable("Look", "ShowMultilineDesc", true);
	config_file.addVariable("Look", "ShowStatusButton", true);
	config_file.addVariable("Look", "Style", "kadu");
	config_file.addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	config_file.addVariable("Look", "UserboxBgColor", w.palette().background().color());
	config_file.addVariable("Look", "UserBoxColumnCount", 1);
	config_file.addVariable("Look", "UserboxFgColor", w.palette().foreground().color());
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
	config_file.addVariable("Network", "Server", "");
	config_file.addVariable("Network", "TimeoutInMs", 5000);
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
	config_file.addVariable("ShortCuts", "chat_newline", "Return");
	config_file.addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	config_file.addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	config_file.addVariable("ShortCuts", "kadu_deleteuser", "Del");
	config_file.addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	config_file.addVariable("ShortCuts", "kadu_persinfo", "Ins");
	config_file.addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");

	createAllDefaultToolbars();
}

void Core::createAllDefaultToolbars()
{
	// dont use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	KaduWindow::createDefaultToolbars(toolbarsConfig);
	ChatEditBox::createDefaultToolbars(toolbarsConfig);

	xml_config_file->sync();
}

void Core::init()
{
	// protocol modules should be loaded before gui
	// it fixes crash on loading pending messages from config, contacts import from 0.6.5, and maybe other issues
	ModulesManager::instance()->loadProtocolModules();

	Myself.setAnonymous(false);
	Myself.setDisplay(config_file.readEntry("General", "Nick", tr("Me")));

	connect(StatusContainerManager::instance(), SIGNAL(statusChanged()), this, SLOT(statusChanged()));
	// TODO 0.6.6:
	StatusChanger = new UserStatusChanger();
	StatusChangerManager::instance()->registerStatusChanger(StatusChanger);
	StatusChangerManager::instance()->enable();

	Updates::initModule();

#ifdef Q_OS_MACX
	setIcon(IconsManager::instance()->loadPixmap("BigOffline"));
#else
	setIcon(IconsManager::instance()->loadPixmap("Offline"));
#endif
	QTimer::singleShot(15000, this, SLOT(deleteOldConfigurationFiles()));

	NotificationManager::instance(); // TODO: 0.6.6

	AccountManager::instance()->ensureLoaded();
	BuddyManager::instance()->ensureLoaded();
	ContactManager::instance()->ensureLoaded();
}

void Core::storeConfiguration()
{
	xml_config_file->makeBackup();
}

char *SystemUserName;
void Core::deleteOldConfigurationFiles()
{
	kdebugf();

	QDir oldConfigs2(ggPath(), "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files);

	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
			QFile::remove(ggPath(oldConfigs2[i]));

	QDir oldBacktraces(ggPath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
			QFile::remove(ggPath(oldBacktraces[i]));

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
			QFile::remove(tmp + "/" + oldDebugs[i]);

	kdebugf2();
}

void Core::statusChanged()
{
	kdebugf();

	setIcon(StatusContainerManager::instance()->statusPixmap());
}

void Core::kaduWindowDestroyed()
{
	Window = 0;
}

void Core::accountAdded(Account account)
{
	printf("account: %s\n", qPrintable(account.uuid().toString()));
	printf("contact: %s\n", qPrintable(account.accountContact().uuid().toString()));
	printf("buddy: %s\n", qPrintable(Myself.uuid().toString()));
	account.accountContact().setOwnerBuddy(Myself);
}

void Core::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(messageReceived(const Message &)),
			this, SIGNAL(messageReceived(const Message &)));
		connect(chatService, SIGNAL(messageSent(const Message &)),
			this, SIGNAL(messageSent(const Message &)));
	}

	connect(protocol, SIGNAL(connecting(Account)), this, SIGNAL(connecting()));
	connect(protocol, SIGNAL(connected(Account)), this, SIGNAL(connected()));
	connect(protocol, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
}

void Core::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (protocol)
	{
		ChatService *chatService = protocol->chatService();
		if (chatService)
		{
			disconnect(chatService, SIGNAL(messageReceived(const Message &)),
				this, SIGNAL(messageReceived(const Message &)));
			disconnect(chatService, SIGNAL(messageSent(const Message &)),
				this, SIGNAL(messageSent(const Message &)));
		}

		disconnect(protocol, SIGNAL(connecting(Account)), this, SIGNAL(connecting()));
		disconnect(protocol, SIGNAL(connected(Account)), this, SIGNAL(connected()));
		disconnect(protocol, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
	}

	// TODO: 0.6.6
	//Myself.removeContact(account);
}

void Core::configurationUpdated()
{
	QApplication::setStyle(config_file.readEntry("Look", "QtStyle"));

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
}

QString Core::readToken(const QPixmap &tokenPixmap)
{
/*	TokenDialog *td = new TokenDialog(tokenPixmap, 0);
	QString result;

	if (td->exec() == QDialog::Accepted)
		result = td->getValue();
	delete td;*/
	return "";
}

Status Core::status()
{
	return StatusChanger->status();
}

void Core::createGui()
{
	Window = new KaduWindow(0);
	Window->setWindowIcon(QApplication::windowIcon());
	connect(Window, SIGNAL(destroyed(QObject *)), this, SLOT(kaduWindowDestroyed()));

	// initialize file transfers
	FileTransferManager::instance();

	if (ShowMainWindowOnStart)
		Window->show();
}

void Core::setShowMainWindowOnStart(bool show)
{
	ShowMainWindowOnStart = show;
}

KaduWindow * Core::kaduWindow()
{
	return Window;
}

void Core::setIcon(const QPixmap &pixmap)
{
	bool blocked = false;
	emit settingMainIconBlocked(blocked);

	if (!blocked)
	{
		QIcon icon(pixmap);
		if (Window)
			Window->setWindowIcon(icon);
		QApplication::setWindowIcon(icon);
		emit mainIconChanged(icon);
	}
}
//TODO 0.6.6:
void Core::setStatus(const Status &status)
{
	StatusChanger->userStatusSet(status);
}

void Core::setOnline(const QString &description)
{
	StatusChanger->userStatusSet(Status("Online", description));
}

void Core::setAway(const QString &description)
{
	StatusChanger->userStatusSet(Status("Away", description));
}

void Core::setInvisible(const QString &description)
{
	StatusChanger->userStatusSet(Status("Invisible", description));
}

void Core::setOffline(const QString &description)
{
	StatusChanger->userStatusSet(Status("Offline", description));
}

void Core::quit()
{
	if (!Instance)
		return;

	delete Instance;
	Instance = 0;
}
