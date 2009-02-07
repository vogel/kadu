/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSettings>
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
#include "accounts/account_data.h"
#include "accounts/account_manager.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"

#include "contacts/model/contacts-model.h"

#include "gui/widgets/contact-data-window.h"

#include "gui/widgets/contacts-list-widget.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"

#include "../modules/gadu_protocol/gadu.h"
#include "../modules/gadu_protocol/gadu-contact-account-data.h"
#include "../modules/gadu_protocol/gadu_images_manager.h"

#include "about.h"
#include "chat_edit_box.h"
#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "expimp.h"
#include "groups_manager.h"
#include "hot_key.h"
#include "html_document.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu_parser.h"
#include "main_configuration_window.h"
#include "message_box.h"
#include "modules.h"
#include "misc.h"
#include "pending_msgs.h"
#include "personal_info.h"
#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"
#include "search.h"
#include "status_changer.h"
#include "syntax_editor.h"
#include "tabbar.h"
#include "toolbar.h"
#include "updates.h"
#include "userbox.h"
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

const char *Kadu::SyntaxTextExtended = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
	"#{message} - message content,\n"
	"#{backgroundColor} - background color of message,\n"
	"#{fontColor} - font color of message,\n"
	"#{nickColor} - font color of nick,\n"
	"#{sentDate} - when message was sent,\n"
	"#{receivedDate} - when message was received,\n"
	"#{separator} - separator between messages,\n"
	"<kadu:header>text</kadu:header> - text will not be displayed in 'Remove repeated headers' mode\n"
);

const char *Kadu::SyntaxTextNotify = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
	"#{protocol} - protocol that triggered event,\n"
	"#{event} - name of event,\n"
);

bool Kadu::Closing = false;

void Kadu::keyPressEvent(QKeyEvent *e)
{
//	kdebugf();
	if (e->key() == Qt::Key_Escape)
	{
		if (Docked)
		{
			kdebugm(KDEBUG_INFO, "Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
			if (dontHideOnClose)
				showMinimized();
			else
				hide();
		}
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_deleteuser"))
		deleteUsersActionDescription->createAction(this)->trigger();
	else if (e->key() == Qt::Key_C && e->modifiers() & Qt::ControlModifier)
		InfoPanel->pageAction(QWebPage::Copy)->trigger();

	emit keyPressed(e);

	QWidget::keyPressEvent(e);
//	kdebugf2();
}

void Kadu::closeEvent(QCloseEvent *event)
{
	kdebugf();

	if (!Closing)
	{
		event->ignore();
		close();
	}
	else
		event->accept();

	kdebugf2();
}


void disableNonIdUles(KaduAction *action)
{
	kdebugf();
	foreach(const Contact contact, action->contacts())
		if (contact.accountData(AccountManager::instance()->defaultAccount()) == 0)
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);
	kdebugf2();
}

void disableContainsSelfUles(KaduAction *action)
{
	if (action->contacts().contains(kadu->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
}

void checkNotify(KaduAction *action)
{
	kdebugf();

	if (config_file.readBoolEntry("Notify", "NotifyAboutAll"))
	{
		action->setEnabled(false);
		return;
	}

	foreach(const Contact contact, action->contacts())
		if (contact.accountData(AccountManager::instance()->defaultAccount()) == 0)
		{
			action->setEnabled(false);
			return;
		}
	action->setEnabled(true);

	bool on = true;
	/*
	foreach (const Contact contact, action->contacts())
		//TODO: 0.6.6
		if (!user.notify())
		{
			on = false;
			break;
		}
	*/
	action->setChecked(on);

	kdebugf2();
}

void checkOfflineTo(KaduAction *action)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	bool on = true;
	foreach(const Contact contact, action->contacts())
		if (contact.accountData(account) == 0 || !contact.isOfflineTo(account))
		{
			on = false;
			break;
		}
	action->setChecked(on);
	kdebugf2();
}

void checkHideDescription(KaduAction *action)
{
	Account *account = AccountManager::instance()->defaultAccount();

	foreach(const Contact contact, action->contacts())
		if (contact.accountData(account) == 0)
		{
			action->setEnabled(false);
			return;
		}
	action->setEnabled(true);

	bool on = false;
	/*
	foreach(const Contact contact, action->contacts())
		//TODO: 0.6.6
		if (user.data("HideDescription").toString() == "true")
		{
			on = true;
			break;
		}
	*/
	action->setChecked(on);
}

void disableNotOneUles(KaduAction *action)
{
	kdebugf();

	if (action->contact().isNull())
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduUle(KaduAction *action)
{
	kdebugf();

	Contact contact = action->contact();

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	if (!contact.accountData(AccountManager::instance()->defaultAccount()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduDescription(KaduAction *action)
{
	kdebugf();

	Contact contact = action->contact();
	Account *account = AccountManager::instance()->defaultAccount();

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	if (!contact.accountData(account))
	{
		action->setEnabled(false);
		return;
	}

	if (contact.accountData(account)->status().description().isEmpty())
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduDescriptionUrl(KaduAction *action)
{
	kdebugf();

	Account *account = AccountManager::instance()->defaultAccount();
	Contact contact = action->contact();

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	if (!contact.accountData(account))
	{
		action->setEnabled(false);
		return;
	}

	if (contact.accountData(account)->status().description().isEmpty())
	{
		action->setEnabled(false);
		return;
	}

	if (contact.accountData(account)->status().description().indexOf(HtmlDocument::urlRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoEMail(KaduAction *action)
{
	kdebugf();

	if (action->contacts().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	const ContactList contacts = action->contacts();
	/*
	//TODO: 0.6.6
	if (ules[0].email().isEmpty() || ules[0].email().indexOf(HtmlDocument::mailRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}*/

	action->setEnabled(true);
	kdebugf2();
}

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent)
	: KaduMainWindow(parent),
	InfoPanel(0), MenuBar(0), KaduMenu(0), ContactsMenu(0), HelpMenu(0), RecentChatsMenu(0), GroupBar(0),
	ContactsWidget(0), statusMenu(0), statusButton(), lastPositionBeforeStatusMenuHide(),
	StartTime(QDateTime::currentDateTime()), updateInformationPanelTimer(), NextStatus(),
	selectedUsers(new UserGroup()), ShowMainWindowOnStart(true),
	DoBlink(false), BlinkOn(false),Docked(false), dontHideOnClose(false)
{
	kdebugf();

	GroupManager::instance()->loadConfiguration();
	ContactManager::instance()->loadConfiguration(xml_config_file);

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
	GroupBar = new KaduTabBar(this);
	GroupBar->setGroups(GroupManager::instance()->groups());
	hbox_layout->setStretchFactor(GroupBar, 1);

	StatusChangerManager::initModule();
	connect(status_changer_manager, SIGNAL(statusChanged(Status)), this, SLOT(changeStatus(Status)));

	userStatusChanger = new UserStatusChanger();
	status_changer_manager->registerStatusChanger(userStatusChanger);

#if 0
	splitStatusChanger = new SplitStatusChanger(GG_STATUS_DESCR_MAXSIZE);
	status_changer_manager->registerStatusChanger(splitStatusChanger);
#endif

	// gadu, chat, search
	GaduProtocol::initModule();
	ChatManager::initModule();
	SearchDialog::initModule();

	ContactsWidget = new ContactsListWidget(this);
	ContactsWidget->setModel(new ContactsModel(ContactManager::instance(), this));

	hbox_layout->setStretchFactor(ContactsWidget, 100);
	hbox_layout->addWidget(GroupBar);
	hbox_layout->addWidget(ContactsWidget);
	hbox_layout->setAlignment(GroupBar, Qt::AlignTop);

	connect(ContactsWidget, SIGNAL(contactActivated(Contact)), this, SLOT(sendMessage(Contact)));

// 	connect(Userbox, SIGNAL(mouseButtonClicked(int, Q3ListBoxItem *, const QPoint &)),
// 		this, SLOT(mouseButtonClicked(int, Q3ListBoxItem *)));
	connect(ContactsWidget, SIGNAL(currentContactChanged(Contact)), this, SLOT(currentChanged(Contact)));


	ActionDescription *writeEmailActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEMailActionActivated(QAction *, bool)),
		"WriteEmail", tr("Write email message"), false, "",
		disableNoEMail
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(writeEmailActionDescription);

	ActionDescription *copyDescriptionActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		"CopyDescription", tr("Copy description"), false, "",
		disableNoGaduDescription
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(copyDescriptionActionDescription);

	ActionDescription *openDescriptionLinkActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		"OpenDescriptionLink", tr("Open description link in browser"), false, "",
		disableNoGaduDescriptionUrl
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(openDescriptionLinkActionDescription);

	ActionDescription *copyPersonalInfoActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		"CopyPersonalInfo", tr("Copy personal info")
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(copyPersonalInfoActionDescription);

	ActionDescription *lookupUserInfoActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search in directory"), false, "",
		disableNoGaduUle
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(lookupUserInfoActionDescription);

	ContactsListWidgetMenuManager::instance()->addSeparator();

	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(chat_manager->ignoreUserActionDescription);
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(chat_manager->blockUserActionDescription);

	notifyAboutUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		"NotifyAboutUser", tr("Notify about user"), true, "",
		checkNotify
	);
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(notifyAboutUserActionDescription);

	offlineToUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "offlineToUserAction",
		this, SLOT(offlineToUserActionActivated(QAction *, bool)),
		"Offline", tr("Offline to user"), true, "",
		checkOfflineTo
	);
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(offlineToUserActionDescription);

	hideDescriptionActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "hideDescriptionAction",
		this, SLOT(hideDescriptionActionActivated(QAction *, bool)),
		"ShowDescription_off", tr("Hide description"), true, "",
		checkHideDescription
	);
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(hideDescriptionActionDescription);

	ContactsListWidgetMenuManager::instance()->addManagementSeparator();

	deleteUsersActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "deleteUsersAction",
		this, SLOT(deleteUsersActionActivated(QAction *, bool)),
		"RemoveFromUserlist", tr("Delete")
	);
	deleteUsersActionDescription->setShortcut("kadu_deleteuser");
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(deleteUsersActionDescription);

// 	groups_manager->setTabBar(GroupBar);
	setDocked(Docked, dontHideOnClose);

	loadWindowGeometry(this, "General", "Geometry", 0, 50, 205, 465);

// 	IgnoredManager::loadFromConfiguration();

	/* a newbie? */
// TODO: 0.6.6 some way of setting title needed
//	setWindowTitle(tr("Kadu: %1").arg(Myself.ID("Gadu")));

	pending.loadConfiguration(xml_config_file);

	inactiveUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		"ShowHideInactiveUsers", tr("Hide offline users"),
		true, tr("Show offline users")
	);
	connect(inactiveUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(inactiveUsersActionCreated(KaduAction *)));
	inactiveUsersAction->setShortcut("kadu_showoffline");

	descriptionUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		"ShowOnlyDescriptionUsers", tr("Hide users without description"),
		true, tr("Show users without description")
	);
	connect(descriptionUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(descriptionUsersActionCreated(KaduAction *)));
	descriptionUsersAction->setShortcut("kadu_showonlydesc");

	onlineAndDescriptionUsersAction = new ActionDescription(
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		"ShowOnlineAndDescriptionUsers", tr("Show only online and description users"),
		true, tr("Show all users")
	);
	connect(onlineAndDescriptionUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(onlineAndDescUsersActionCreated(KaduAction *)));

	configurationActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "configurationAction",
		this, SLOT(configurationActionActivated(QAction *, bool)),
		"Configuration", tr("Configuration")
	);
	configurationActionDescription->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	editUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "editUserAction",
		this, SLOT(editUserActionActivated(QAction *, bool)),
		"EditUserInfo", tr("Contact data"), false, QString::null,
		disableNotOneUles
	);
	connect(editUserActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(editUserActionCreated(KaduAction *)));
	editUserActionDescription->setShortcut("kadu_persinfo");
	ContactsListWidgetMenuManager::instance()->addActionDescription(editUserActionDescription);

	addUserActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		"AddUser", tr("Add user")
	);
	addUserActionDescription->setShortcut("kadu_adduser", Qt::ApplicationShortcut);

	openSearchActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(searchInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search user in directory")
	);
	openSearchActionDescription->setShortcut("kadu_searchuser");

	showStatusActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "openStatusAction",
		this, SLOT(showStatusActionActivated(QAction *, bool)),
		"Offline", tr("Change status")
	);
	connect(showStatusActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(showStatusActionCreated(KaduAction *)));

	useProxyActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "useProxyAction",
		this, SLOT(useProxyActionActivated(QAction *, bool)),
		"UseProxy", tr("Use proxy"), true, tr("Don't use proxy")
	);
	connect(useProxyActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(useProxyActionCreated(KaduAction *)));

	/* guess what */
	createMenu();
	createStatusPopupMenu();
	loadToolBarsFromConfig("");

	connect(statusMenu, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));
	connect(dockMenu, SIGNAL(aboutToHide()), this, SLOT(dockMenuAboutToHide()));
	connect(RecentChatsMenu, SIGNAL(aboutToShow()), this, SLOT(createRecentChatsMenu()));

	dockMenu->insertSeparator();
	dockMenu->addAction(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));

	InfoPanel = new KaduTextBrowser(split);
// TODO: 0.6.5
// 	InfoPanel->setFrameStyle(QFrame::NoFrame);
// 	InfoPanel->setMinimumHeight(int(1.5 * QFontMetrics(InfoPanel->QTextEdit::font()).height()));
//	InfoPanel->resize(InfoPanel->size().width(), int(1.5 * QFontMetrics(InfoPanel->font()).height()));
// 	InfoPanel->setTextFormat(Qt::RichText);
// 	InfoPanel->setAlignment(Qt::AlignVCenter/** | Qt::WordBreak | Qt::DontClip*/);

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->QWidget::hide();
	connect(&updateInformationPanelTimer, SIGNAL(timeout()), this, SLOT(updateInformationPanel()));

	statusButton = new QPushButton(icons_manager->loadIcon("Offline"), tr("Offline"), this, "statusButton");
	MainLayout->addWidget(statusButton);
	statusButton->setPopup(statusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusButton->hide();

	QList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
		this, SLOT(accountRegistered(Account *)));
	foreach (Account *account, AccountManager::instance()->accounts())
		accountRegistered(account);

	connect(userlist, SIGNAL(usersDataChanged(QString)), this, SLOT(updateInformationPanelLater()));
	connect(userlist, SIGNAL(protocolUsersDataChanged(QString, QString)), this, SLOT(updateInformationPanelLater()));
	connect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(updateInformationPanelLater()));

	connect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
		this, SLOT(editUserActionSetParams(QString, UserListElement)));

	MainLayout->setResizeMode(QLayout::Minimum);
	setCentralWidget(MainWidget);

#ifdef Q_OS_MAC
	qt_mac_set_dock_menu(dockMenu);
#endif

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chat_manager->loadOpenedWindows();

	configurationUpdated();

	kdebugf2();
}

void Kadu::accountRegistered(Account *account)
{
	Protocol *protocol = account->protocol();

	connect(protocol, SIGNAL(messageReceived(Account *, ContactList, const QString &, time_t)),
		this, SLOT(messageReceived(Account *, ContactList, const QString &, time_t)));
	connect(protocol, SIGNAL(connecting(Account *)), this, SLOT(connecting()));
	connect(protocol, SIGNAL(connected(Account *)), this, SLOT(connected()));
	connect(protocol, SIGNAL(disconnected(Account *)), this, SLOT(disconnected()));
	connect(protocol, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	connect(protocol, SIGNAL(needTokenValue(QPixmap, QString &)),
		this, SLOT(readTokenValue(QPixmap, QString &)));
	connect(protocol, SIGNAL(systemMessageReceived(const QString &)), this, SLOT(systemMessageReceived(const QString &)));
	connect(protocol, SIGNAL(statusChanged(Account *, Status)),
			this, SLOT(statusChanged(Account *, Status)));

	ContactAccountData *contactAccountData = protocol->protocolFactory()->
			newContactAccountData(Myself, account, account->data()->id());
	Myself.addAccountData(contactAccountData);
}

QVBoxLayout * Kadu::mainLayout() const
{
	return MainLayout;
}

void Kadu::writeEMailActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	if (contacts.count() < 1)
		return;

	Contact contact = contacts[0];
	if (!contact.email().isEmpty())
		openMailClient(contact.email());

	kdebugf2();
}

void Kadu::copyDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *data = contact.accountData(account);

	if (!data)
		return;

	QString description = data->status().description();
	if (description.isEmpty())
		return;

	QApplication::clipboard()->setText(description, QClipboard::Selection);
	QApplication::clipboard()->setText(description, QClipboard::Clipboard);

	kdebugf2();
}

void Kadu::openDescriptionLinkActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *data = contact.accountData(account);

	if (!data)
		return;

	QString description = data->status().description();
	if (description.isEmpty())
		return;

	QRegExp url = HtmlDocument::urlRegExp();
	int idx_start = url.search(description);
	if (idx_start >= 0)
		openWebBrowser(description.mid(idx_start, url.matchedLength()));

	kdebugf2();
}

void Kadu::copyPersonalInfoActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	foreach (Contact contact, contacts)
		infoList.append(KaduParser::parse(copyPersonalDataSyntax, contact.prefferedAccount(), contact, false));

	QString info = infoList.join("\n");
	if (info.isEmpty())
		return;

	QApplication::clipboard()->setText(info, QClipboard::Selection);
	QApplication::clipboard()->setText(info, QClipboard::Clipboard);

	kdebugf2();
}

void Kadu::lookupInDirectoryActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	if (contacts.count() != 1)
	{
		searchInDirectoryActionActivated(0, false);
		return;
	}

	Contact contact = contacts[0];
	GaduContactAccountData *cad = dynamic_cast<GaduContactAccountData *>(AccountManager::instance()->defaultAccount());
	if (!cad)
		return;

	SearchDialog *sd = new SearchDialog(kadu, cad->uin());
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void Kadu::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	bool on = true;
	/*
	foreach(const ContactList contact, contacts)
		//TODO: 0.6.6
		if (!user.notify())
		{
			on = false;
			break;
		}

	foreach(const UserListElement &user, users)
		//TODO: 0.6.6
		if (user.notify() == on)
			user.setNotify(!on);
	*/
// TODO: 0.6.6
// 	userlist->writeToConfig();

	foreach(KaduAction *action, notifyAboutUserActionDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(!on);
	}

	kdebugf2();
}

void Kadu::offlineToUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	if (toggled && !config_file.readBoolEntry("General", "PrivateStatus"))
	{
		if (MessageBox::ask("You need to have private status to do it, would you like to set private status now?"))
			changePrivateStatus->setChecked(true);
		else
		{
			sender->setChecked(!toggled);
			return;
		}
	}

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	bool on = true;
	foreach(const Contact contact, contacts)
		if (contact.accountData(account) == 0 || !contact.isOfflineTo(account))
		{
			on = false;
			break;
		}
	/*
	foreach(const Contact contact, contacts)
		if (contact.accountData(account) != 0 || contact.isOfflineTo(account) == on)
			//TODO: 0.6.6
			user.setProtocolData("Gadu", "OfflineTo", !on); // TODO: here boolean
	*/
// TODO: 0.6.6
// 	userlist->writeToConfig();

	foreach (KaduAction *action, offlineToUserActionDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(!on);
	}

	kdebugf2();
}

void Kadu::hideDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	bool on = true;
	/*
	foreach(const UserListElement &user, users)
		//TODO: 0.6.6
		if (user.data("HideDescription").toString() == "true")
		{
			on = false;
			break;
		}

	foreach(const UserListElement &user, users)
		//TODO: 0.6.6
		user.setData("HideDescription", on ? "true" : "false"); // TODO: here string, LOL
	*/
// TODO: 0.6.6
// 	userlist->writeToConfig();

	foreach(KaduAction *action, hideDescriptionActionDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(on);
	}

	kdebugf2();
}

void Kadu::deleteUsersActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	removeUsers(contacts);

	kdebugf2();
}

void Kadu::openRecentChats(QAction *action)
{
	kdebugf();
	chat_manager->openPendingMsgs(chat_manager->closedChatUsers().at(action->data().toInt()), true);
	kdebugf2();
}

void Kadu::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

// 	groups_manager->changeDisplayingOffline(window->userBox(), !toggled);
}

void Kadu::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

// 	groups_manager->changeDisplayingWithoutDescription(window->userBox(), !toggled);
}

void Kadu::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

// 	groups_manager->changeDisplayingOnlineAndDescription(window->userBox(), toggled);
}

void Kadu::inactiveUsersActionCreated(KaduAction *action)
{
	action->setChecked(!config_file.readBoolEntry("General", "ShowOffline"));
}
void Kadu::descriptionUsersActionCreated(KaduAction *action)
{
	action->setChecked(!config_file.readBoolEntry("General", "ShowWithoutDescription"));
}
void Kadu::onlineAndDescUsersActionCreated(KaduAction *action)
{
	action->setChecked(config_file.readBoolEntry("General", "ShowOnlineAndDescription"));
}

void Kadu::configurationActionActivated(QAction *sender, bool toggled)
{
	MainConfigurationWindow::instance()->show();
}

void Kadu::editUserActionSetParams(QString /*protocolName*/, UserListElement user)
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
}

void Kadu::editUserActionCreated(KaduAction *action)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(action->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isAnonymous())
	{
		action->setIcon(icons_manager->loadIcon("AddUser"));
		action->setText(tr("Add user"));
	}
}

void Kadu::editUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	(new ContactDataWindow(UserListElement::fromContact(contact, AccountManager::instance()->defaultAccount()), kadu))->show();

	kdebugf2();
}

void Kadu::addUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	Contact contact = window
		? window->contact()
		: Contact();

	if (contact.isAnonymous() || contact.isNull())
		(new ContactDataWindow(UserListElement::fromContact(contact, AccountManager::instance()->defaultAccount()), kadu))->show();

 	kdebugf2();
}

void Kadu::openChatWith()
{
	kdebugf();

	(new OpenChatWith(this))->show();

	kdebugf2();
}

void Kadu::yourAccounts(QAction *sender, bool toggled)
{
	(new PersonalInfoDialog(kadu))->show();
}

void Kadu::manageIgnored(QAction *sender, bool toggled)
{
	(new Ignored(kadu, "ignored"))->show();
}

void Kadu::showStatusActionActivated(QAction *sender, bool toggled)
{
	statusMenu->exec(QCursor::pos());
}

void Kadu::showStatusActionCreated(KaduAction *action)
{
	Account *gadu = AccountManager::instance()->defaultAccount();

	if (gadu != NULL)
		action->setIcon(gadu->protocol()->statusPixmap());
}

void Kadu::setStatusActionsIcon()
{
	Account *gadu = AccountManager::instance()->defaultAccount();

	if (gadu != NULL)
	{
		QPixmap pixmap = gadu->protocol()->statusPixmap();

		foreach (KaduAction *action, showStatusActionDescription->actions())
			action->setIcon(pixmap);
	}
}

void Kadu::useProxyActionActivated(QAction *sender, bool toggled)
{
	config_file.writeEntry("Network", "UseProxy", toggled);

	setProxyActionsStatus(toggled);
}

void Kadu::useProxyActionCreated(KaduAction *action)
{
	action->setChecked(config_file.readBoolEntry("Network", "UseProxy", false));
}

void Kadu::setProxyActionsStatus(bool checked)
{
	foreach (KaduAction *action, useProxyActionDescription->actions())
		action->setChecked(checked);
}

void Kadu::setProxyActionsStatus()
{
	setProxyActionsStatus(config_file.readBoolEntry("Network", "UseProxy", false));
}

void Kadu::searchInDirectoryActionActivated(QAction *sender, bool toggled)
{
	(new SearchDialog(kadu))->show();
}

void Kadu::addGroupActionActivated(QAction *sender, bool toggled)
{

}

void Kadu::help(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Pomoc_online");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Help_online");
}

void Kadu::bugs(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/B%C5%82%C4%99dy");
	else
		openWebBrowser("http://www.kadu.net/w/English:Bugs");
}

void Kadu::support(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Kadu:Site_support");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Site_support");
}

void Kadu::getInvolved(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(QTextCodec::locale()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Do%C5%82%C4%85cz");
	else
		openWebBrowser("http://www.kadu.net/w/English:GetInvolved");
}

void Kadu::about(QAction *sender, bool toggled)
{
	(new About(kadu))->show();
}

void Kadu::quit()
{
	kdebugf();

	if (measureTime)
	{
		time_t sec;
		int msec;
		getTime(&sec, &msec);
		endingTime = (sec % 1000) * 1000 + msec;
	}
	qApp->quit();
}

void Kadu::importExportUserlist(QAction *sender, bool toggled)
{
	(new UserlistImportExport(kadu))->show();
}

void Kadu::hideKadu(QAction *sender, bool toggled)
{
	if (Docked)
		if (dontHideOnClose)
			showMinimized();
		else
			close();
}

void Kadu::changeAppearance()
{
	kdebugf();

	QApplication::setStyle(config_file.readEntry("Look", "QtStyle"));

	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

	if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
	{
		InfoPanel->show();


		QFont font = config_file.readFontEntry("Look", "PanelFont");

		QString fontFamily = font.family();
		QString fontSize;
		if (font.pointSize() > 0)
			fontSize = QString::number(font.pointSize()) + "pt";
		else
			fontSize = QString::number(font.pixelSize()) + "px";
		QString fontStyle = font.italic() ? "italic" : "normal";
		QString fontWeight = font.bold() ? "bold" : "normal";
		QString textDecoration = font.underline() ? "underline" : "none";
		QString backgroundColor = config_file.readColorEntry("Look","InfoPanelBgColor").name();
		QString fontColor = config_file.readColorEntry("Look","InfoPanelFgColor").name();

		infoPanelStyle = QString(
			"html {"
			"	color: %1;"
			"	font: %2 %3 %4 %5;"
			"	text-decoration: %6;"
			"	margin: 0;"
			"	padding: 0;"
			"	background-color: %7;"
			"}"
			"div {"
			"	color: %1;"
			"	font: %2 %3 %4 %5;"
			"	text-decoration: %6;"
			"	margin: 0;"
			"	padding: 0;"
			"	background-color: %7;"
			"}").arg(fontColor, fontStyle, fontWeight, fontSize, fontFamily, textDecoration, backgroundColor);

 		if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
 			InfoPanel->page()->mainFrame()->setScrollBarPolicy (Qt::Vertical, Qt::ScrollBarAsNeeded);
 		else
 			InfoPanel->page()->mainFrame()->setScrollBarPolicy (Qt::Vertical, Qt::ScrollBarAlwaysOff);
	}
	else
		dynamic_cast<QWidget *>(InfoPanel)->hide();

	kadu->statusButton->setShown(config_file.readBoolEntry("Look", "ShowStatusButton"));

	const UserStatus &stat = AccountManager::instance()->status();

	QPixmap pix = stat.pixmap();
	QIcon icon(pix);
	statusButton->setIcon(icon);

	setStatusActionsIcon();

	setMainWindowIcon(pix);
	emit statusPixmapChanged(icon, stat.toString());
	kdebugf2();
}

void Kadu::removeUsers(ContactList contacts)
{
	kdebugf();
	UserListElements users = UserListElements::fromContactList(contacts, AccountManager::instance()->defaultAccount());
	if (users.count())
	{
		QString altNicks = users.altNicks().join(", ");
		QString tmp;

		for (unsigned int i = 0; i < users.count(); i+=10)
			tmp += (altNicks.section(", ", i, (i + 9)) + ",\n");

		if (MessageBox::ask(tr("Selected users:\n%0will be deleted. Are you sure?").arg(tmp), "Warning", kadu))
		{
			emit removingUsers(users);
			userlist->removeUsers(users);
// TODO: 0.6.6
// 			userlist->writeToConfig();
		}
	}

	kdebugf2();
}

void Kadu::blink()
{
	QIcon icon;

	kdebugf();

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	if (!DoBlink && gadu->isConnected())
		return;
	else if (!DoBlink && !gadu->isConnected())
	{
		icon = QIcon(gadu->statusPixmap(Status::Offline));
		statusButton->setIcon(icon);
		emit statusPixmapChanged(icon, "Offline");
		return;
	}

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
	}

	statusButton->setIcon(icon);
	emit statusPixmapChanged(icon, iconName);

	BlinkOn = !BlinkOn;

	blinktimer->start(1000, TRUE);
}

void Kadu::mouseButtonClicked(int button, Q3ListBoxItem *item)
{
	kdebugmf(KDEBUG_FUNCTION_START, "button=%d\n", button);
	if (!item)
		InfoPanel->setHtml("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	kdebugf2();
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

		if (contacts[0] != myself()) //TODO: elem.hasFeature("SendingMessages")
			chat_manager->sendMessage(contact, contacts);
		else if (contact.mobile().isEmpty() && !contact.email().isEmpty())
			openMailClient(contact.email());

	}
}

void Kadu::changeStatusSlot()
{
	if (0 == AccountManager::instance()->defaultAccount())
		return;

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();

	QAction *action = dynamic_cast<QAction *>(sender());
	if (action)
	{
		foreach (QAction *a, changeStatusActionGroup->actions())
			a->setChecked(a == action);
		slotHandleState(action->data().toInt());
	}
}

void Kadu::changePrivateStatusSlot(bool toggled)
{
	Status status(userStatusChanger->status());
// TODO: 0.6.6
// 	status.setFriendsOnly(toggled);
	setStatus(status);

	config_file.writeEntry("General", "PrivateStatus", toggled);
}

/* when we want to change the status */
// TODO: fix it
void Kadu::slotHandleState(int command)
{
	kdebugf();

	Status status(userStatusChanger->status().type());

	switch (command)
	{
		case 0:
			status.setType(Status::Online);
			setStatus(status);
			break;
		case 1:
			status.setType(Status::Online);
			status.setDescription(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 2:
			status.setType(Status::Busy);
			setStatus(status);
			break;
		case 3:
			status.setType(Status::Busy);
			status.setDescription(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 4:
			status.setType(Status::Invisible);
			setStatus(status);
			break;
		case 5:
			status.setType(Status::Invisible);
			status.setDescription(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
		case 6:
			status.setType(Status::Offline);
			setStatus(status);
			break;
		case 7:
			status.setType(Status::Offline);
			status.setDescription(status.description());
			ChooseDescription::show(status, lastPositionBeforeStatusMenuHide);
			break;
	}

	kdebugf2();
}

void Kadu::changeStatus(Status newStatus)
{
	kdebugf();

	if (NextStatus.isOffline())
	{
		changeStatusToOfflineDesc->setEnabled(false);
		changeStatusToOffline->setChecked(true);
	}

	Account *account = AccountManager::instance()->defaultAccount();
	if (!account)
		return;

	Protocol *gadu = account->protocol();
	if (!gadu)
		return;

// TODO: 0.6.6
// 	if (gadu->nextStatus() == newStatus)
// 		return;

	NextStatus.setStatus(newStatus);
	gadu->setStatus(newStatus);
}

void Kadu::connecting()
{
	kdebugf();

	DoBlink = true;

	if (!blinktimer)
	{
		blinktimer = new QTimer(this, "blinktimer");
		QObject::connect(blinktimer, SIGNAL(timeout()), kadu, SLOT(blink()));
	}

	blinktimer->start(1000, true);
	kdebugf2();
}

// TODO: move back to chatManager
void Kadu::messageReceived(Account *account, ContactList senders, const QString &msg, time_t time)
{
	kdebugf();

	// TODO: workaround
	emit messageReceivedSignal(account, senders, msg, time);

	ChatWidget *chat = chat_manager->findChatWidget(senders);
	if (chat)
		chat->newMessage(account, senders, msg, time);
	else
	{
		if (config_file.readBoolEntry("General","AutoRaise"))
		{
			kadu->showNormal();
			kadu->setFocus();
		}

		if (config_file.readBoolEntry("Chat", "OpenChatOnMessage"))
		{
			// TODO: 0.6.6
			if (config_file.readBoolEntry("Chat", "OpenChatOnMessageWhenOnline") && false /*!Myself.status("Gadu").isOnline()*/)
			{
				pending.addMsg(account, senders, msg, time);
				return;
			}

			// TODO: it is lame
			chat_manager->openChatWidget(account, senders);
			chat = chat_manager->findChatWidget(senders);
			chat->newMessage(account, senders, msg, time);
		}
		else
			pending.addMsg(account, senders, msg, time);
	}

	kdebugf2();
}

void Kadu::connected()
{
	kdebugf();
	DoBlink = false;
	kdebugf2();
}

void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
{
	for (int i = 0, count = pending.count(); i < count; i++)
	{
		PendingMsgs::Element& e = pending[i];
		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
	}
}

// TODO: remove
void Kadu::systemMessageReceived(const QString &msg)
{
//	MessageBox::msg(msg);
}

void Kadu::disconnected()
{
	kdebugmf(KDEBUG_FUNCTION_START, "Disconnection has occured\n");

	chat_manager->refreshTitles();

	DoBlink = false;

	if (blinktimer)
	{
		blinktimer->stop();
		delete blinktimer;
		blinktimer = NULL;
	}

	kdebugf2();
}

bool Kadu::close(bool quit)
{
	if (!quit && Docked && !dontHideOnClose)
	{
		kdebugmf(KDEBUG_INFO, "hiding\n");
		hide();
		return false;
	}
	else
	{
		Closing = true;

		writeToolBarsToConfig("");

		if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
		{
			config_file.writeEntry("General", "UserBoxHeight", ContactsWidget->size().height());
			config_file.writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
		}
		if (config_file.readBoolEntry("Look", "ShowStatusButton"))
			config_file.writeEntry("General", "UserBoxHeight", ContactsWidget->size().height());
 		saveWindowGeometry(this, "General", "Geometry");

		config_file.writeEntry("General", "DefaultDescription", defaultdescriptions.join("<-->"));

// TODO: 0.6.6
// 		if (config_file.readEntry("General", "StartupStatus") == "LastStatus")
// 			config_file.writeEntry("General", "LastStatusIndex", userStatusChanger->status().index());

		if (config_file.readBoolEntry("General", "StartupLastDescription"))
			config_file.writeEntry("General", "LastStatusDescription", userStatusChanger->status().description());

		pending.storeConfiguration(xml_config_file);
// 		IgnoredManager::writeToConfiguration();

		GroupManager::instance()->storeConfiguration();
		ContactManager::instance()->storeConfiguration(xml_config_file);
		AccountManager::instance()->storeConfiguration(xml_config_file);

		Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
		if (gadu->isConnected())
			if (config_file.readBoolEntry("General", "DisconnectWithCurrentDescription"))
				setOffline(gadu->status().description());
			else
				setOffline(config_file.readEntry("General", "DisconnectDescription"));

		xml_config_file->makeBackup();

#if 0
		status_changer_manager->unregisterStatusChanger(splitStatusChanger);
		delete splitStatusChanger;
		splitStatusChanger = 0;
#endif

		status_changer_manager->unregisterStatusChanger(userStatusChanger);
		delete userStatusChanger;
		userStatusChanger = 0;

		ModulesManager::closeModule();

		Updates::closeModule();
		delete defaultFontInfo;
		delete defaultFont;

		disconnect(gadu, SIGNAL(messageReceived(Account *, ContactList, const QString &, time_t)),
				this, SLOT(messageReceived(Account *, ContactList, const QString &, time_t)));
		disconnect(gadu, SIGNAL(connecting()), this, SLOT(connecting()));
		disconnect(gadu, SIGNAL(connected()), this, SLOT(connected()));
		disconnect(gadu, SIGNAL(disconnected()), this, SLOT(disconnected()));
		disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
				this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
		disconnect(gadu, SIGNAL(needTokenValue(QPixmap, QString &)),
				this, SLOT(readTokenValue(QPixmap, QString &)));
		disconnect(gadu, SIGNAL(systemMessageReceived(const QString &)), this, SLOT(systemMessageReceived(const QString &)));

		disconnect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
				this, SLOT(editUserActionSetParams(QString, UserListElement)));

		disconnect(userlist, SIGNAL(usersDataChanged(QString)), this, SLOT(updateInformationPanelLater()));
		disconnect(userlist, SIGNAL(protocolUsersDataChanged(QString, QString)), this, SLOT(updateInformationPanelLater()));
		disconnect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(updateInformationPanelLater()));

		StatusChangerManager::closeModule();

		ChatManager::closeModule();
		SearchDialog::closeModule();

// TODO: 0.6.6
// 		userlist->writeToConfig();//writeToConfig must be before GroupsManager::closeModule, because GM::cM removes all groups from userlist
// 		GroupsManagerOld::closeModule();
		xml_config_file->sync();
		UserList::closeModule();
		EmoticonsManager::closeModule();
		IconsManager::closeModule();

#ifdef Q_OS_MACX
		//na koniec przywracamy domy�ln� ikon�, je�eli tego nie zrobimy, to pozostanie bie��cy status
		setMainWindowIcon(QPixmap(dataPath("kadu.png")));
#endif

		QWidget::close(true);

		kdebugmf(KDEBUG_INFO, "Graceful shutdown...\n");

		qDeleteAll(children());

		delete xml_config_file;
		delete config_file_ptr;

		return true;
	}
}

void Kadu::quitApplication()
{
	kdebugf();
	close(true);
}

Kadu::~Kadu(void)
{
	kdebugf();

	qDeleteAll(KaduActions.values());

	delete selectedUsers;

	kdebugf2();
}

void Kadu::createRecentChatsMenu()
{
	kdebugf();

	RecentChatsMenu->clear();
	QAction *action;
	if (chat_manager->closedChatUsers().isEmpty())
	{
		action = RecentChatsMenu->addAction(tr("No closed chats found"));
		action->setEnabled(false);

		kdebugf2();
		return;
	}

	unsigned int index = 0; // indeks pozycji w popupie

	foreach(const ContactList contacts, chat_manager->closedChatUsers())
	{
		UserListElements users = UserListElements::fromContactList(contacts, AccountManager::instance()->defaultAccount());
		QStringList altnicks = users.altNicks(); // lista nick�w z okna rozmowy
		QString chat_users;

		if (altnicks.count() <= 5)
			chat_users = altnicks.join(", ");
		else // je�eli jest wi�cej ni� pi�ciu u�ytkownik�w...
		{
			for (int i = 0; i < 4; i++) // to i tak dodajemy tylko pierwszych pi�ciu :)
				chat_users.append(*altnicks.at(i) + ", ");
			chat_users.append(*altnicks.at(4) + " [...]");
		}
		action = new QAction(icons_manager->loadIcon("OpenChat"), chat_users, this);
		action->setData(index);
		RecentChatsMenu->addAction(action);

		index++;
	}

	kdebugf2();
}

void Kadu::insertMenuActionDescription(ActionDescription *actionDescription, MenuType type, int pos)
{
	kdebugf();
	if (!actionDescription)
		return;
	KaduAction *action = actionDescription->createAction(this);

	QMenu *menu;

	switch (type)
	{
		case MenuKadu:
			menu = KaduMenu;
			break;
		case MenuContacts:
			menu = ContactsMenu;
			break;
		case MenuHelp:
			menu = HelpMenu;
	}

	QList<QAction *> menuActions = menu->actions();
	if (pos >= menuActions.count() - 1 || pos == -1)
		menu->addAction(action);
	else
		menu->insertAction(menuActions[pos], action);

	MenuActions[actionDescription] = MenuAction(action, type);
}

void Kadu::removeMenuActionDescription(ActionDescription *actionDescription)
{
	if (!actionDescription)
		return;
	KaduAction *action = MenuActions[actionDescription].Action;

	if (!action)
		return;
	switch (MenuActions[actionDescription].Menu)
	{
		case MenuKadu:
			KaduMenu->removeAction(action);
			break;
		case MenuContacts:
			ContactsMenu->removeAction(action);
			break;
		case MenuHelp:
			HelpMenu->removeAction(action);
	}
	MenuActions.remove(actionDescription);

}

void Kadu::createMenu()
{
	kdebugf();
// Kadu Menu
	KaduMenu = new QMenu;
	KaduMenu->setTitle("Kadu");

	RecentChatsMenu = new QMenu;
	RecentChatsMenu->setIcon(icons_manager->loadIcon("OpenChat"));
	RecentChatsMenu->setTitle(tr("Recent chats..."));
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));

	insertMenuActionDescription(configurationActionDescription, MenuKadu);

	ActionDescription *yourAccountsActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "yourAccountsAction",
		this, SLOT(yourAccounts(QAction *, bool)),
		"PersonalInfo", tr("Your accounts")
	);//TODO 0.6.6: implement
	insertMenuActionDescription(yourAccountsActionDescription, MenuKadu);

	KaduMenu->addSeparator();

	KaduMenu->addMenu(RecentChatsMenu);
	KaduMenu->addSeparator();
	ActionDescription *hideKaduActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "hideKaduAction",
		this, SLOT(hideKadu(QAction *, bool)),
		"HideKadu", tr("&Hide")
	);
	insertMenuActionDescription(hideKaduActionDescription, MenuKadu);

	ActionDescription *exitKaduActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(quit()),
		"Exit", tr("&Exit")
	);
	insertMenuActionDescription(exitKaduActionDescription, MenuKadu);

// ContactsMenu
	ContactsMenu = new QMenu;
	ContactsMenu->setTitle(tr("Contacts"));

	insertMenuActionDescription(addUserActionDescription, MenuContacts);
	ActionDescription *addGroupActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "addGroupAction",
		this, SLOT(addGroupActionActivated(QAction *, bool)),
		"", tr("Add Group")
	);//TODO 0.6.6: implement and update icons
	insertMenuActionDescription(addGroupActionDescription, MenuContacts);
	insertMenuActionDescription(openSearchActionDescription, MenuContacts);
	ContactsMenu->addSeparator();
	insertMenuActionDescription(chat_manager->openChatWithActionDescription, MenuContacts);
	ContactsMenu->addSeparator();
	ActionDescription *manageIgnoredActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "manageIgnoredAction",
		this, SLOT(manageIgnored(QAction *, bool)),
		"Ignore", tr("&Ignored users")
	);
	insertMenuActionDescription(manageIgnoredActionDescription, MenuContacts);

	ActionDescription *importExportUserlisActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "importExportUserlisAction",
		this, SLOT(importExportUserlist(QAction *, bool)),
		"ImportExport", tr("I&mport / Export userlist")
	); //TODO 0.6.6: remove
	insertMenuActionDescription(importExportUserlisActionDescription, MenuContacts);

// Help Menu
	HelpMenu = new QMenu;
	HelpMenu->setTitle(tr("Help"));

	ActionDescription *helpActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(help(QAction *, bool)),
		"HelpMenuItem", tr("Getting H&elp")
	);
	insertMenuActionDescription(helpActionDescription, MenuHelp);
	HelpMenu->addSeparator();
	ActionDescription *bugsActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugs(QAction *, bool)),
		"", tr("Submitt Bug Report")
	);
	insertMenuActionDescription(bugsActionDescription, MenuHelp);
	ActionDescription *supportActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "supportAction",
		this, SLOT(support(QAction *, bool)),
		"", tr("Support us")
	);
	insertMenuActionDescription(supportActionDescription, MenuHelp);
	ActionDescription *getInvolvedActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolved(QAction *, bool)),
		"", tr("Get Involved")
	);
	insertMenuActionDescription(getInvolvedActionDescription, MenuHelp);
	HelpMenu->addSeparator();
	ActionDescription *aboutActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(about(QAction *, bool)),
		"AboutMenuItem", tr("A&bout Kadu")
	);
	insertMenuActionDescription(aboutActionDescription, MenuHelp);
/*
TODO 0.6.6:
add new action: "History" in MenuKadu
*/
	menuBar()->addMenu(KaduMenu);
	menuBar()->addMenu(ContactsMenu);
	menuBar()->addMenu(HelpMenu);

	kdebugf2();
}

void Kadu::statusMenuAboutToHide()
{
	lastPositionBeforeStatusMenuHide = statusMenu->pos();
}

void Kadu::dockMenuAboutToHide()
{
	lastPositionBeforeStatusMenuHide = dockMenu->pos();
}

void Kadu::createStatusPopupMenu()
{
	kdebugf();

	QPixmap pix;
	QIcon icon;

	statusMenu = new QMenu(this);
	dockMenu = new QMenu(this);

	changeStatusActionGroup = new QActionGroup(this);
	changeStatusActionGroup->setExclusive(false); // HACK

	// TODO: 0.6.6

	changeStatusToOnline = new QAction(/*icons_manager->loadIcon(s.pixmapName(Online, false, false)), */tr("Online"), this);
	changeStatusToOnline->setCheckable(true);
	changeStatusToOnline->setData(0);
	connect(changeStatusToOnline, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToOnlineDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Online, true, false)), */tr("Online (d.)"), this);
	changeStatusToOnlineDesc->setCheckable(true);
	changeStatusToOnlineDesc->setData(1);
	connect(changeStatusToOnlineDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToBusy = new QAction(/*icons_manager->loadIcon(s.pixmapName(Busy, false, false)), */tr("Busy"), this);
	changeStatusToBusy->setCheckable(true);
	changeStatusToBusy->setData(2);
	connect(changeStatusToBusy, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToBusyDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Busy, true, false)), */tr("Busy (d.)"), this);
	changeStatusToBusyDesc->setCheckable(true);
	changeStatusToBusyDesc->setData(3);
	connect(changeStatusToBusyDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToInvisible = new QAction(/*icons_manager->loadIcon(s.pixmapName(Invisible, false, false)), */tr("Invisible"), this);
	changeStatusToInvisible->setCheckable(true);
	changeStatusToInvisible->setData(4);
	connect(changeStatusToInvisible, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToInvisibleDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Invisible, true, false)), */tr("Invisible (d.)"), this);
	changeStatusToInvisibleDesc->setCheckable(true);
	changeStatusToInvisibleDesc->setData(5);
	connect(changeStatusToInvisibleDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToOffline = new QAction(/*icons_manager->loadIcon(s.pixmapName(Offline, false, false)), */tr("Offline"), this);
	changeStatusToOffline->setCheckable(true);
	changeStatusToOffline->setData(6);
	connect(changeStatusToOffline, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changeStatusToOfflineDesc = new QAction(/*icons_manager->loadIcon(s.pixmapName(Offline, true, false)), */tr("Offline (d.)"), this);
	changeStatusToOfflineDesc->setCheckable(true);
	changeStatusToOfflineDesc->setData(7);
	connect(changeStatusToOfflineDesc, SIGNAL(triggered()), this, SLOT(changeStatusSlot()));

	changePrivateStatus = new QAction(tr("Private"), this);
	changePrivateStatus->setCheckable(true);
	connect(changePrivateStatus, SIGNAL(toggled(bool)), this, SLOT(changePrivateStatusSlot(bool)));

	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");
	changePrivateStatus->setChecked(privateStatus);

	changeStatusActionGroup->addAction(changeStatusToOnline);
	changeStatusActionGroup->addAction(changeStatusToOnlineDesc);
	changeStatusActionGroup->addAction(changeStatusToBusy);
	changeStatusActionGroup->addAction(changeStatusToBusyDesc);
	changeStatusActionGroup->addAction(changeStatusToInvisible);
	changeStatusActionGroup->addAction(changeStatusToInvisibleDesc);
	changeStatusActionGroup->addAction(changeStatusToOffline);
	changeStatusActionGroup->addAction(changeStatusToOfflineDesc);

	statusMenu->addAction(changeStatusToOnline);
	statusMenu->addAction(changeStatusToOnlineDesc);
	statusMenu->addAction(changeStatusToBusy);
	statusMenu->addAction(changeStatusToBusyDesc);
	statusMenu->addAction(changeStatusToInvisible);
	statusMenu->addAction(changeStatusToInvisibleDesc);
	statusMenu->addAction(changeStatusToOffline);
	statusMenu->addAction(changeStatusToOfflineDesc);
	statusMenu->addSeparator();
	statusMenu->addAction(changePrivateStatus);

	dockMenu->addAction(changeStatusToOnline);
	dockMenu->addAction(changeStatusToOnlineDesc);
	dockMenu->addAction(changeStatusToBusy);
	dockMenu->addAction(changeStatusToBusyDesc);
	dockMenu->addAction(changeStatusToInvisible);
	dockMenu->addAction(changeStatusToInvisibleDesc);
	dockMenu->addAction(changeStatusToOffline);
	dockMenu->addAction(changeStatusToOfflineDesc);
	dockMenu->addSeparator();
	dockMenu->addAction(changePrivateStatus);

	kdebugf2();
}

void Kadu::showdesc(bool show)
{
	if (show)
		InfoPanel->show();
	else
		InfoPanel->QWidget::hide();
}

void Kadu::updateInformationPanelLater()
{
	updateInformationPanelTimer.start(0, true);
}

void Kadu::updateInformationPanel()
{
// 	if (ContactsWidget->cur ->currentUserExists())
// 		updateInformationPanel(Userbox->currentContact());
}

void Kadu::updateInformationPanel(Contact contact)
{
	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		return;
// 	if (Userbox->currentUserExists() && contact == Userbox->currentContact())
	if (false)
	{
		kdebugmf(KDEBUG_INFO, "%s\n", qPrintable(contact.display()));
		QString text = QString(
			"<html>"
			"	<head>"
			"		<style type='text/css'>") +
			infoPanelStyle +
			"		</style>"
			"	</head>"
			"	<body>";
		HtmlDocument doc;
		doc.parseHtml(KaduParser::parse(InfoPanelSyntax, contact.prefferedAccount(), contact));
		doc.convertUrlsToHtml();
		doc.convertMailToHtml();
		if((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") != EMOTS_NONE && config_file.readBoolEntry("General", "ShowEmotPanel"))
			emoticons->expandEmoticons(doc, config_file.readColorEntry("Look", "InfoPanelBgColor"), (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));

		text += doc.generateHtml();
		text += "</body></html>";
		InfoPanel->setHtml(text);

		kdebugf2();
	}
}

void Kadu::currentChanged(Contact contact)
{
	updateInformationPanel(contact);
}

KaduTabBar* Kadu::groupBar() const
{
	return GroupBar;
}

ContactList Kadu::contacts()
{
	return ContactsWidget->selectedContacts();
}

void Kadu::setDocked(bool docked, bool dontHideOnClose1)
{
	Docked = docked;
	dontHideOnClose = dontHideOnClose1;
	qApp->setQuitOnLastWindowClosed(!Docked || dontHideOnClose);

// TODO: 0.6.6
// 	if (config_file.readBoolEntry("General", "ShowAnonymousWithMsgs") || !Docked || dontHideOnClose)
// 	{
// 	Userbox->removeNegativeFilter(anonymousUsers);
// 	Userbox->applyNegativeFilter(anonymousUsersWithoutMessages);
// 	}
// 	else
// 	{
// 		Userbox->removeNegativeFilter(anonymousUsersWithoutMessages);
// 		Userbox->applyNegativeFilter(anonymousUsers);
// 	}
}

bool Kadu::docked() const
{
	return Docked;
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
	if (changePrivateStatus->isChecked() == privateStatus)
		return;

	Status status = userStatusChanger->status();
// TODO: 0.6.6
// 	status.setFriendsOnly(privateStatus);
	userStatusChanger->userStatusSet(status);

	changePrivateStatus->setChecked(privateStatus);
}

void Kadu::configurationUpdated()
{
	refreshPrivateStatusFromConfigFile();

	changeAppearance();

	if (config_file.readBoolEntry("Look", "UseUserboxBackground", true))
	{
		QString type = config_file.readEntry("Look", "UserboxBackgroundDisplayStyle");
		ContactsWidget->setBackground(config_file.readEntry("Look", "UserboxBackground"),
			type == "Centered" ? ContactsListWidget::BackgroundCentered
			: type == "Tiled" ? ContactsListWidget::BackgroundTiled
			: type == "Stretched" ? ContactsListWidget::BackgroundStretched
			: type == "TiledAndCentered" ? ContactsListWidget::BackgroundTiledAndCentered
			: ContactsListWidget::BackgroundNone);
	}
	else
		ContactsWidget->setBackground();

// 	groups_manager->refreshTabBar();

//	Myself.setAltNick(config_file.readEntry("General", "Nick"));

	kadu->setDocked(kadu->Docked, kadu->dontHideOnClose);

	InfoPanelSyntax = SyntaxList::readSyntax("infopanel", config_file.readEntry("Look", "InfoPanelSyntaxFile"),
		"<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"file:///@{ManageUsersWindowIcon}\"></td><td> "
		"<div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i]</div></td></tr></table> <hr> <b>%s</b> [<br>%d]");
	InfoPanel->setHtml("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	updateInformationPanel();

	setProxyActionsStatus();

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
		int statusIndex = config_file.readNumEntry("General", "LastStatusIndex", UserStatus::index(Offline, false));
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
	userStatusChanger->userStatusSet(status);

	kdebugf2();
}

void Kadu::startupProcedure()
{
	kdebugf();

	if (ShowMainWindowOnStart)
		show();

	Updates::initModule();

	xml_config_file->makeBackup();

	status_changer_manager->enable();
	setDefaultStatus();

	kdebugf2();
}

void Kadu::setShowMainWindowOnStart(bool show)
{
	ShowMainWindowOnStart = show;
}

void Kadu::statusChanged(Account *account, Status status)
{
	DoBlink = false;

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
	}

	showStatusOnMenu(index);
}

void Kadu::showStatusOnMenu(int statusNr)
{
	kdebugf();

	QList<QAction*> statusActions = changeStatusActionGroup->actions();
	for (int i = 0; i < 8; ++i)
		statusActions[i]->setChecked(i == statusNr);

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
// 	changePrivateStatus->setChecked(gadu->status().isFriendsOnly());
// TODO: 0.6.6

	statusButton->setText(qApp->translate("@default", Status::name(gadu->status()).ascii()));
	changeStatusToOfflineDesc->setEnabled(statusNr != 6);
	changeStatusToOffline->setEnabled(statusNr != 7);

	QPixmap pix = gadu->statusPixmap();
	QIcon icon(pix);
	QString iconName = Status::name(gadu->status());

	statusButton->setIcon(icon);
	setMainWindowIcon(pix);

	foreach(KaduAction *action, showStatusActionDescription->actions())
		action->setIcon(icon);

	emit statusPixmapChanged(icon, iconName);
}

void Kadu::readTokenValue(QPixmap tokenImage, QString &tokenValue)
{
	TokenDialog *td = new TokenDialog(tokenImage, 0);

	if (td->exec() == QDialog::Accepted)
		td->getValue(tokenValue);
	else
		tokenValue.truncate(0);

	delete td;
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

void Kadu::setMainWindowIcon(const QPixmap &icon)
{
	bool blocked = false;
	emit settingMainIconBlocked(blocked);
	if (!blocked)
	{
		setWindowIcon(icon);
		QApplication::setWindowIcon(icon);
	}
}

const QDateTime &Kadu::startTime() const
{
	return StartTime;
}

void Kadu::customEvent(QEvent *e)
{
	Account *defaultAccount = AccountManager::instance()->defaultAccount();

	if (int(e->type()) == 4321)
		show();
//		QTimer::singleShot(0, this, SLOT(show()));
	else if (int(e->type()) == 5432)
	{
		OpenGGChatEvent *ev = static_cast<OpenGGChatEvent *>(e);
		if (ev->number() > 0)
		{
			Contact contact = userlist->byID("Gadu", QString::number(ev->number())).toContact(defaultAccount);
			ContactList contacts;
			contacts << contact;
			chat_manager->openChatWidget(defaultAccount, contacts, true);
		}
	}
	else
		QWidget::customEvent(e);
}

void Kadu::setStatus(const Status &status)
{
	userStatusChanger->userStatusSet(status);
}

void Kadu::setOnline(const QString &description)
{
	userStatusChanger->userStatusSet(Status(Status::Online, description));
}

void Kadu::setBusy(const QString &description)
{
	userStatusChanger->userStatusSet(Status(Status::Busy, description));
}

void Kadu::setInvisible(const QString &description)
{
	userStatusChanger->userStatusSet(Status(Status::Invisible, description));
}

void Kadu::setOffline(const QString &description)
{
	userStatusChanger->userStatusSet(Status(Status::Offline, description));
}

void Kadu::createDefaultConfiguration()
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
	config_file.addVariable("General", "Language",  QString(QTextCodec::locale()).mid(0,2));
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
	config_file.addVariable("Look", "ChatFont", *defaultFont);
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
	config_file.addVariable("Look", "DescriptionColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "DisplayGroupTabs", true);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "IconsPaths", "");
	config_file.addVariable("Look", "IconTheme", "default");
	config_file.addVariable("Look", "InfoPanelBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "InfoPanelFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "InfoPanelSyntaxFile", "default");
	config_file.addVariable("Look", "NiceDateFormat", true);
	config_file.addVariable("Look", "NoHeaderInterval", 30);
	config_file.addVariable("Look", "NoHeaderRepeat", false);
	config_file.addVariable("Look", "NoServerTime", true);
	config_file.addVariable("Look", "NoServerTimeDiff", 60);
	config_file.addVariable("Look", "PanelFont", *defaultFont);
	config_file.addVariable("Look", "PanelVerticalScrollBar", false);
	config_file.addVariable("Look", "ParagraphSeparator", 4);
	config_file.addVariable("Look", "ShowBold", true);
	config_file.addVariable("Look", "ShowDesc", true);
	config_file.addVariable("Look", "ShowInfoPanel", true);
	config_file.addVariable("Look", "ShowMultilineDesc", true);
	config_file.addVariable("Look", "ShowStatusButton", true);
	config_file.addVariable("Look", "Style", "kadu");
	config_file.addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	config_file.addVariable("Look", "UserboxBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "UserBoxColumnCount", 1);
	config_file.addVariable("Look", "UserboxFgColor", w.paletteForegroundColor());
	QFont userboxfont(*defaultFont);
	userboxfont.setPointSize(defaultFont->pointSize()+1);
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

void Kadu::createAllDefaultToolbars()
{
	// dont use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	Kadu::createDefaultToolbars(toolbarsConfig);
	ChatEditBox::createDefaultToolbars(toolbarsConfig);
	SearchDialog::createDefaultToolbars(toolbarsConfig);

	xml_config_file->sync();
}

void Kadu::createDefaultToolbars(QDomElement toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "topDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");

	addToolButton(toolbarConfig, "inactiveUsersAction");
	addToolButton(toolbarConfig, "descriptionUsersAction");
	addToolButton(toolbarConfig, "configurationAction");
	addToolButton(toolbarConfig, "editUserAction");
	addToolButton(toolbarConfig, "openSearchAction");
	addToolButton(toolbarConfig, "addUserAction");
}

void Kadu::addAction(const QString &actionName, bool showLabel)
{
	addToolButton(findExistingToolbar(""), actionName, showLabel);
	kadu->refreshToolBars("");
}
