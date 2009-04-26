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
#include "configuration/configuration-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"
#include "gui/widgets/chat_edit_box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "misc/misc.h"
#include "misc/token-dialog.h"
#include "notify/notification-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/services/chat-service.h"

#include "../modules/gadu_protocol/gadu-protocol.h"

#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "icons_manager.h"
#include "modules.h"
#include "pending_msgs.h"
#include "search.h"
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

		NotificationManager::instance(); // TODO: 0.6.6
	}

	return Instance;
}

Core::Core() : Myself(Contact::TypeNull), Window(0), ShowMainWindowOnStart(true)
{
	QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));
}

Core::~Core()
{
	Configuration->store();
	delete Configuration;
	Configuration = 0;

	storeConfiguration();

	status_changer_manager->unregisterStatusChanger(StatusChanger);
	delete StatusChanger;
	StatusChanger = 0;

	StatusChangerManager::closeModule();
	ModulesManager::closeModule();
	Updates::closeModule();
	SearchDialog::closeModule();
	EmoticonsManager::closeModule();
	IconsManager::closeModule();

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
	SearchDialog::createDefaultToolbars(toolbarsConfig);

	xml_config_file->sync();
}

void Core::init()
{
	Configuration = new ConfigurationManager();
	Configuration->load();

	Myself = Contact();
	Myself.setDisplay(config_file.readEntry("General", "Nick"));

	triggerAllAccountsRegistered();

	StatusChangerManager::initModule();
	connect(status_changer_manager, SIGNAL(statusChanged(Status)), this, SLOT(changeStatus(Status)));

	StatusChanger = new UserStatusChanger();
	status_changer_manager->registerStatusChanger(StatusChanger);
	status_changer_manager->enable();

	Updates::initModule();
	GaduProtocol::initModule();
	SearchDialog::initModule();

#ifdef Q_OS_MACX
	setIcon(icons_manager->loadPixmap("BigOffline"));
#else
	setIcon(icons_manager->loadPixmap("Offline"));
#endif

	loadDefaultStatus();

	QTimer::singleShot(15000, this, SLOT(deleteOldConfigFiles()));
}

void Core::loadDefaultStatus()
{
	kdebugf();

	QString description;
	QString startupStatus = config_file.readEntry("General", "StartupStatus");
	Status status;

	if (config_file.readBoolEntry("General", "StartupLastDescription"))
		description = config_file.readEntry("General", "LastStatusDescription");
	else
		description = config_file.readEntry("General", "StartupDescription");

	bool offlineToInvisible = false;
	Status::StatusType type;

	if (startupStatus.isEmpty() || startupStatus == "LastStatus")
	{
		int typeIndex = config_file.readNumEntry("General", "LastStatusType", -1);
		if (typeIndex == -1)
		{
			typeIndex = config_file.readNumEntry("General", "LastStatusIndex", 6) / 2;
			config_file.removeVariable("General", "LastStatusIndex");
		}

		type = (Status::StatusType)typeIndex;
		offlineToInvisible = config_file.readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline");
	}
	else if (startupStatus == "Online")
		type = Status::Online;
	else if (startupStatus == "Busy")
		type = Status::Busy;
	else if (startupStatus == "Invisible")
		type = Status::Invisible;
	else if (startupStatus == "Offline")
		type = Status::Offline;

	if ((Status::Offline == type) && offlineToInvisible)
		type = Status::Invisible;

	status.setType(type);
	status.setDescription(description);

	Account *account = AccountManager::instance()->defaultAccount();
	if (account)
		account->protocol()->setPrivateMode(config_file.readBoolEntry("General", "PrivateStatus"));
	setStatus(status);

	kdebugf2();
}

void Core::storeConfiguration()
{
	if (config_file.readEntry("General", "StartupStatus") == "LastStatus")
		config_file.writeEntry("General", "LastStatusType", (int)StatusChanger->status().type());

	if (config_file.readBoolEntry("General", "StartupLastDescription"))
		config_file.writeEntry("General", "LastStatusDescription", StatusChanger->status().description());

	pending.storeConfiguration(xml_config_file);
// 		IgnoredManager::writeToConfiguration();

// 	GroupManager::instance()->store();
// 	ContactManager::instance()->store();
// 	AccountManager::instance()->store();

	if (AccountManager::instance()->defaultAccount() && AccountManager::instance()->defaultAccount()->protocol())
	{
		Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
		if (gadu->isConnected())
			if (config_file.readBoolEntry("General", "DisconnectWithCurrentDescription"))
				setOffline(gadu->status().description());
			else
				setOffline(config_file.readEntry("General", "DisconnectDescription"));
	}

	xml_config_file->makeBackup();
}

char *SystemUserName;
void Core::deleteOldConfigurationFiles()
{
	kdebugf();

	QDir oldConfigs2(ggPath(), "kadu.conf.xml.backup.*", QDir::Name, QDir::Files);

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

//TODO 0.6.6:
void Core::changeStatus(Status newStatus)
{
	kdebugf();

	foreach (Account *account, AccountManager::instance()->accounts())
	{
		Protocol *protocol = account->protocol();
		if (!protocol)
			return;

		if (protocol->nextStatus() == newStatus)
			return;

		protocol->setStatus(newStatus);
	}

	//update icon:
	Account *account = AccountManager::instance()->defaultAccount();
	if (!account)
		return;

	Protocol *protocol = account->protocol();
	if (!protocol)
		return;

	setIcon(protocol->statusPixmap(newStatus));
}

void Core::kaduWindowDestroyed()
{
	Window = 0;
}

void Core::accountRegistered(Account *account)
{
	Protocol *protocol = account->protocol();

	ChatService *chatService = protocol->chatService();
	if (chatService)
		connect(chatService, SIGNAL(messageReceived(Chat *, Contact , const QString &)),
			this, SIGNAL(messageReceived(Chat *, Contact , const QString &)));

	connect(protocol, SIGNAL(connecting(Account *)), this, SIGNAL(connecting()));
	connect(protocol, SIGNAL(connected(Account *)), this, SIGNAL(connected()));
	connect(protocol, SIGNAL(disconnected(Account *)), this, SIGNAL(disconnected()));
	connect(protocol, SIGNAL(statusChanged(Account *, Status)),
			this, SLOT(statusChanged(Account *, Status)));

	ContactAccountData *contactAccountData = protocol->protocolFactory()->
			newContactAccountData(Myself, account, account->id());
	Myself.addAccountData(contactAccountData);

	protocol->setStatus(status_changer_manager->status());
}

void Core::accountUnregistered(Account *account)
{
	Protocol *protocol = account->protocol();

	ChatService *chatService = protocol->chatService();
	if (chatService)
		disconnect(chatService, SIGNAL(messageReceived(Chat *, Contact , const QString &)),
			this, SIGNAL(messageReceived(Chat *, Contact , const QString &)));

	disconnect(protocol, SIGNAL(connecting(Account *)), this, SIGNAL(connecting()));
	disconnect(protocol, SIGNAL(connected(Account *)), this, SIGNAL(connected()));
	disconnect(protocol, SIGNAL(disconnected(Account *)), this, SIGNAL(disconnected()));
	disconnect(protocol, SIGNAL(statusChanged(Account *, Status)),
			this, SLOT(statusChanged(Account *, Status)));

	Myself.removeAccountData(account);
}

void Core::configurationUpdated()
{
	QApplication::setStyle(config_file.readEntry("Look", "QtStyle"));

	Account *account = AccountManager::instance()->defaultAccount();
	if (account)
	{
		setIcon(account->protocol()->statusPixmap());
		account->protocol()->setPrivateMode(config_file.readBoolEntry("General", "PrivateStatus"));
	}
#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		       QSettings::NativeFormat);
	if(config_file.readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu",
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

#ifdef DEBUG_ENABLED
	debug_mask = config_file.readNumEntry("General", "DEBUG_MASK");
	gg_debug_level = debug_mask | ~255;
#endif
}

QString Core::readToken(const QPixmap &tokenPixmap)
{
	TokenDialog *td = new TokenDialog(tokenPixmap, 0);
	QString result;

	if (td->exec() == QDialog::Accepted)
		result = td->getValue();
	delete td;

	return result;
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

void Core::setIcon(const QPixmap &icon)
{
	bool blocked = false;
	emit settingMainIconBlocked(blocked);

	if (!blocked)
	{
		if (Window)
			Window->setWindowIcon(icon);
		QApplication::setWindowIcon(icon);
	}
}

void Core::setStatus(const Status &status)
{
	StatusChanger->userStatusSet(status);
}

void Core::setOnline(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Online, description));
}

void Core::setBusy(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Busy, description));
}

void Core::setInvisible(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Invisible, description));
}

void Core::setOffline(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Offline, description));
}

void Core::quit()
{
	if (!Instance)
		return;

	delete Instance;
	Instance = 0;
}
