/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSettings>
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtWebKit/QWebFrame>

#ifndef _MSC_VER
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "chat/chat_manager.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-kadu-data.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"

#include "contacts/model/contacts-model.h"

#include "contacts/model/filter/group-contact-filter.h"
#include "contacts/model/filter/has-description-contact-filter.h"
#include "contacts/model/filter/online-contact-filter.h"
#include "contacts/model/filter/online-and-description-contact-filter.h"
#include "contacts/model/filter/offline-contact-filter.h"
#include "contacts/model/filter/anonymous-contact-filter.h"
#include "contacts/model/filter/anonymous-without-messages-contact-filter.h"

#include "core/core.h"

#include "file-transfer/file-transfer-manager.h"

#include "gui/widgets/chat_edit_box.h"
#include "gui/widgets/contacts-list-widget.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/widgets/group-tab-bar.h"

#include "gui/windows/contact-data-window.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"

#include "../modules/gadu_protocol/gadu-contact-account-data.h"
#include "../modules/gadu_protocol/gadu-protocol.h"

#include "about.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "expimp.h"
#include "hot_key.h"
#include "html_document.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu_parser.h"
#include "main_configuration_window.h"
#include "message_box.h"
#include "modules.h"
#include "misc/misc.h"
#include "personal_info.h"
#include "search.h"
#include "status_changer.h"
#include "syntax_editor.h"
#include "toolbar.h"
#include "updates.h"
#include "xml_config_file.h"

#include "kadu.h"

#ifdef Q_OS_MAC
extern void qt_mac_set_dock_menu(QMenu *); //there's no header for it
#endif

//look for comment in config_file.h
ConfigFile *config_file_ptr;

Kadu *kadu;

static QTimer *blinktimer;
QMenu *dockMenu;

const char *Kadu::SyntaxText = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size\n"
);

const char *Kadu::SyntaxTextNotify = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
	"#{protocol} - protocol that triggered event,\n"
	"#{event} - name of event,\n"
);

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent)
	: KaduMainWindow(parent)
{/*
	kdebugf();

	Core *core = Core::instance();
	connect(core, SIGNAL(connecting()), this, SLOT(connecting()));
	connect(core, SIGNAL(connected()), this, SLOT(connected()));
	connect(core, SIGNAL(disconnected()), this, SLOT(disconnected()));

	kadu = this;
	blinktimer = 0;

	createDefaultConfiguration();

#ifdef Q_OS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	MainWidget = new QWidget;
	MainLayout = new QVBoxLayout(MainWidget);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	QSplitter *split = new QSplitter(Qt::Vertical, this);
	MainLayout->addWidget(split);

	QWidget* hbox = new QWidget(split);
	QHBoxLayout *hbox_layout = new QHBoxLayout(hbox);
	hbox_layout->setMargin(0);
	hbox_layout->setSpacing(0);

	// groupbar
	GroupBar = new GroupTabBar(this);
	hbox_layout->setStretchFactor(GroupBar, 1);*/

#if 0
	splitStatusChanger = new SplitStatusChanger(GG_STATUS_DESCR_MAXSIZE);
	status_changer_manager->registerStatusChanger(splitStatusChanger);
#endif

	// gadu, chat, search
/*

	ContactsWidget = new ContactsListWidget(this);
	ContactsWidget->setModel(new ContactsModel(ContactManager::instance(), this));
	ContactsWidget->addFilter(GroupBar->filter());

	hbox_layout->setStretchFactor(ContactsWidget, 100);
	hbox_layout->addWidget(GroupBar);
	hbox_layout->addWidget(ContactsWidget);
	hbox_layout->setAlignment(GroupBar, Qt::AlignTop);

	connect(ContactsWidget, SIGNAL(contactActivated(Contact)), this, SLOT(sendMessage(Contact)));
	connect(ContactsWidget, SIGNAL(currentContactChanged(Contact)), this, SLOT(currentChanged(Contact)));
*/

// 	groups_manager->setTabBar(GroupBar);

//	loadWindowGeometry(this, "General", "Geometry", 0, 50, 205, 465);

// 	IgnoredManager::loadFromConfiguration();

	/* a newbie? */
// TODO: 0.6.6 some way of setting title needed
//	setWindowTitle(tr("Kadu: %1").arg(Myself.ID("Gadu")));

/*

	createStatusPopupMenu();

	connect(statusMenu, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));
	connect(dockMenu, SIGNAL(aboutToHide()), this, SLOT(dockMenuAboutToHide()));
	connect(RecentChatsMenu, SIGNAL(aboutToShow()), this, SLOT(createRecentChatsMenu()));

	dockMenu->addSeparator();
	dockMenu->addAction(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));

	statusButton = new QPushButton(icons_manager->loadIcon("Offline"), tr("Offline"), this);
	MainLayout->addWidget(statusButton);
	statusButton->setMenu(statusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusButton->hide();

	QList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setCentralWidget(MainWidget);

#ifdef Q_OS_MAC
	qt_mac_set_dock_menu(dockMenu);
#endif

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chat_manager->loadOpenedWindows();

	configurationUpdated();

	kdebugf2();*/
}

/*
void Kadu::editUserActionSetParams(QString / *protocolName* /, UserListElement user)
{
	kdebugf();
 	foreach (KaduAction *action, editUserActionDescription->actions())
	{
		KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(action->parent());
		if (!window)
			continue;

		Contact contact = window->contact();

		if (!contact.isNull())
		{
			if (contact.isAnonymous())
			{
				action->setIcon(icons_manager->loadIcon("AddUser"));
				action->setText(tr("Add user"));
			}
			else
			{
				action->setIcon(icons_manager->loadIcon("EditUserInfo"));
				action->setText(tr("Contact data"));
			}
		}
	}
	kdebugf2();
}*/

void Kadu::setStatusActionsIcon()
{
// 	Account *gadu = AccountManager::instance()->defaultAccount();
// 
// 	if (gadu != NULL)
// 	{
// 		QPixmap pixmap = gadu->protocol()->statusPixmap();
// 
// 		foreach (KaduAction *action, ShowStatus->actions())
// 			action->setIcon(pixmap);
// 	}
}

void Kadu::changeAppearance()
{
	kdebugf();

	QApplication::setStyle(config_file.readEntry("Look", "QtStyle"));

// 	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

// 	kadu->statusButton->setShown(config_file.readBoolEntry("Look", "ShowStatusButton"));

	const Status &stat = AccountManager::instance()->status();

	Account *account = AccountManager::instance()->defaultAccount();
	QPixmap pix;
	if (account)
		pix = account->statusPixmap(stat);

	QIcon icon(pix);
// 	statusButton->setIcon(icon);

	setStatusActionsIcon();

	Core::instance()->setIcon(pix);

	emit statusPixmapChanged(icon, Status::name(stat));
	kdebugf2();
}

// TODO: 0.6.6 remove blink from here
void Kadu::blink()
{
	QIcon icon;

	kdebugf();
/*
	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	if (!DoBlink && gadu->isConnected())
		return;
	else if (!DoBlink && !gadu->isConnected())
	{
		icon = QIcon(gadu->statusPixmap(Status::Offline));
// 		statusButton->setIcon(icon);
		emit statusPixmapChanged(icon, "Offline");
		return;
	}*/
/*
	QString iconName;
	if (BlinkOn)
	{
		icon = QIcon(gadu->statusPixmap(Status::Offline));
		iconName = "Offline";
	}
	else
	{
		const Status &stat = gadu->nextStatus();
		icon = QIcon(gadu->statusPixmap(stat));
		iconName = Status::name(stat);
	}*/

// 	statusButton->setIcon(icon);
// 	emit statusPixmapChanged(icon, iconName);
/*
	BlinkOn = !BlinkOn;*/
/*
	blinktimer->setSingleShot(true);
	blinktimer->start(1000);*/
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(Contact contact)
{
	ContactsListWidget *widget = dynamic_cast<ContactsListWidget *>(sender());
	if (!widget)
		return;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactList contacts = widget->selectedContacts();

	if (!contacts.isEmpty())
	{
		Contact contact = contacts[0];

		if (contacts[0] != Core::instance()->myself()) //TODO: elem.hasFeature("SendingMessages")
			chat_manager->sendMessage(contact, contacts);
		else if (contact.mobile().isEmpty() && !contact.email().isEmpty())
			openMailClient(contact.email());

	}
}

void Kadu::connecting()
{
	kdebugf();
/*
	DoBlink = true;

	if (!blinktimer)
	{
		blinktimer = new QTimer(this);
		QObject::connect(blinktimer, SIGNAL(timeout()), kadu, SLOT(blink()));
	}

	blinktimer->setSingleShot(true);
	blinktimer->start(1000);*/
	kdebugf2();
}

void Kadu::connected()
{
	kdebugf();
// 	DoBlink = false;
	kdebugf2();
}

void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
{
// 	for (int i = 0, count = pending.count(); i < count; i++)
// 	{
// 		PendingMsgs::Element& e = pending[i];
//	TODO: 0.6.6 or sth?
// 		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
// 	}
}

void Kadu::disconnected()
{
	kdebugmf(KDEBUG_FUNCTION_START, "Disconnection has occured\n");

	chat_manager->refreshTitles();
/*
	DoBlink = false;

	if (blinktimer)
	{
		blinktimer->stop();
		delete blinktimer;
		blinktimer = NULL;
	}*/

	kdebugf2();
}

bool Kadu::close(bool quit)
{
	if (!quit/* && Docked*/)
	{
		kdebugmf(KDEBUG_INFO, "hiding\n");
		hide();
		return false;
	}
	else
	{

#if 0
		status_changer_manager->unregisterStatusChanger(splitStatusChanger);
		delete splitStatusChanger;
		splitStatusChanger = 0;
#endif

		ModulesManager::closeModule();

		Updates::closeModule();

// TODO: 0.6.6
// 		disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
// 				this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));

		ChatManager::closeModule();
		SearchDialog::closeModule();

		EmoticonsManager::closeModule();
		IconsManager::closeModule();

#ifdef Q_OS_MACX
		//na koniec przywracamy domy�ln� ikon�, je�eli tego nie zrobimy, to pozostanie bie��cy status
		setMainWindowIcon(QPixmap(dataPath("kadu.png")));
#endif

		QWidget::close();

		kdebugmf(KDEBUG_INFO, "Graceful shutdown...\n");

		qDeleteAll(children());

		kdebugmf(KDEBUG_INFO, "Sync configuration...\n");
		xml_config_file->sync();

		delete xml_config_file;
		delete config_file_ptr;

		return true;
	}
}

Kadu::~Kadu(void)
{
	kdebugf();

	qDeleteAll(KaduActions.values());

	kdebugf2();
}

//
ContactList Kadu::contacts()
{
	return ContactList();
// 	return ContactsWidget->selectedContacts();
}

void Kadu::show()
{
	QWidget::show();

	// no columns now ...
	// TODO: remove after 0.6
	if (config_file.readBoolEntry("Look", "MultiColumnUserbox", false))
	{
// 		int columns = Userbox->visibleWidth() / config_file.readUnsignedNumEntry("Look", "MultiColumnUserboxWidth", Userbox->visibleWidth());
// 		if (columns < 1)
// 			columns = 1;
// 		config_file.writeEntry("Look", "UserBoxColumnCount", columns);
// 		KaduListBoxPixmap::setColumnCount(columns);
// 		Userbox->refresh();
	}
	config_file.removeVariable("Look", "MultiColumnUserbox");
	config_file.removeVariable("Look", "MultiColumnUserboxWidth");

	emit shown();
}

void Kadu::hide()
{
	emit hiding();
	QWidget::hide();
}

void Kadu::refreshPrivateStatusFromConfigFile()
{
	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");

	// je�li stan nie uleg� zmianie to nic nie robimy
// 	if (changePrivateStatus->isChecked() == privateStatus)
// 		return;

// 	Status status = userStatusChanger->status();
// TODO: 0.6.6
// 	status.setFriendsOnly(privateStatus);
// 	userStatusChanger->userStatusSet(status);

// 	changePrivateStatus->setChecked(privateStatus);
}

void Kadu::configurationUpdated()
{
	refreshPrivateStatusFromConfigFile();

	changeAppearance();

// 	groups_manager->refreshTabBar();

// 	setProxyActionsStatus();

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

void Kadu::setDefaultStatus()
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

	if (startupStatus == "LastStatus")
	{
		int statusIndex = config_file.readNumEntry("General", "LastStatusIndex", 6);
		switch (statusIndex)
		{
			case 0:
			case 1:
				type = Status::Online;
				break;
			case 2:
			case 3:
				type = Status::Busy;
				break;
			case 4:
			case 5:
				type = Status::Invisible;
				break;
			case 6:
			case 7:
				type = Status::Offline;
				break;
		}

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

// TODO: 0.6.6
// 	status.setFriendsOnly(config_file.readBoolEntry("General", "PrivateStatus"));
	Core::instance()->setStatus(status);

	kdebugf2();
}

void Kadu::startupProcedure()
{
	kdebugf();

	status_changer_manager->enable();
	setDefaultStatus();

	kdebugf2();
}

void Kadu::statusChanged(Account *account, Status status)
{
// 	DoBlink = false;
/*
	bool hasDescription = !status.description().isEmpty();
	int index;

	switch (status.type())
	{
		case Status::Online:
			index = hasDescription ? 1 : 0;
			break;
		case Status::Busy:
			index = hasDescription ? 3 : 2;
			break;
		case Status::Invisible:
			index = hasDescription ? 5 : 4;
			break;
		default:
			index = hasDescription ? 7 : 6;
	}*/

// 	showStatusOnMenu(index);
}

char *SystemUserName;
void Kadu::deleteOldConfigFiles()
{
	kdebugf();

	QDir oldConfigs2(ggPath(), "kadu.conf.xml.backup.*", QDir::Name, QDir::Files);
//	kdebugm(KDEBUG_INFO, "%d\n", oldConfigs2.count());
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldConfigs2[i]));
			QFile::remove(ggPath(oldConfigs2[i]));
		}
//	kdebugm(KDEBUG_INFO, "configs2 deleted\n");

	QDir oldBacktraces(ggPath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldBacktraces[i]));
			QFile::remove(ggPath(oldBacktraces[i]));
		}
//	kdebugm(KDEBUG_INFO, "bts deleted\n");

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
		{
			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldDebugs[i]));
			QFile::remove(tmp + "/" + oldDebugs[i]);
		}
//	kdebugm(KDEBUG_INFO, "debugs deleted\n");
	kdebugf2();
}

const QDateTime &Kadu::startTime() const
{
	return QDateTime();
// 	return StartTime;
}

void Kadu::customEvent(QEvent *e)
{
// TODO: 0.6.6
// 	Account *defaultAccount = AccountManager::instance()->defaultAccount();
// 
// 	if (int(e->type()) == 4321)
// 		show();
//		QTimer::singleShot(0, this, SLOT(show()));
// 	else if (int(e->type()) == 5432)
// 	{
// 		OpenGGChatEvent *ev = static_cast<OpenGGChatEvent *>(e);
// 		if (ev->number() > 0)
// 		{
// 			Contact contact = userlist->byID("Gadu", QString::number(ev->number())).toContact(defaultAccount);
// 			ContactList contacts;
// 			contacts << contact;
// 			chat_manager->openChatWidget(defaultAccount, contacts, true);
// 		}
// 	}
// 	else
// 		QWidget::customEvent(e);
}
