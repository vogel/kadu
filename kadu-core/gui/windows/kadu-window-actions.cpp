/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QLocale>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-kadu-data.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"
#include "contacts/account-data/contact-account-data.h"
#include "contacts/filter/has-description-contact-filter.h"
#include "contacts/filter/offline-contact-filter.h"
#include "contacts/filter/online-and-description-contact-filter.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/contacts-list-view.h"
#include "gui/widgets/contacts-list-view-menu-manager.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/contact-data-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/merge-contacts-window.h"
#include "gui/windows/message-box.h"
#include "gui/windows/search-window.h"
#include "gui/windows/your-accounts.h"
#include "misc/misc.h"
#include "parser/parser.h"

#include "about.h"
#include "debug.h"
#include "expimp.h"
#include "html_document.h"
#include "ignore.h"
#include "modules.h"
#include "status_changer.h"

#include "kadu-window-actions.h"

void disableNonIdUles(Action *action)
{
	kdebugf();
	foreach (const Contact contact, action->contacts())
		if (contact.accountData(AccountManager::instance()->defaultAccount()) == 0)
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);
	kdebugf2();
}

void disableContainsSelfUles(Action *action)
{
	if (action->contacts().contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
}

void checkOfflineTo(Action *action)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	bool on = true;
	foreach (const Contact contact, action->contacts())
		if (contact.accountData(account) == 0 || !contact.isOfflineTo(account))
		{
			on = false;
			break;
		}
	action->setChecked(on);
	kdebugf2();
}

void checkHideDescription(Action *action)
{
	Account *account = AccountManager::instance()->defaultAccount();

	foreach (const Contact contact, action->contacts())
		if (contact.accountData(account) == 0)
		{
			action->setEnabled(false);
			return;
		}
	action->setEnabled(true);

	bool on = false;
	foreach (const Contact contact, action->contacts())
	{
		ContactKaduData *ckd = contact.moduleData<ContactKaduData>(true);
		if (!ckd)
			continue;

		if (ckd->hideDescription())
		{
			on = true;
			break;
		}
	}

	action->setChecked(on);
}

void disableNotOneUles(Action *action)
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

void disableNoGaduUle(Action *action)
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

void disableNoGaduDescription(Action *action)
{
	kdebugf();

	Contact contact = action->contact();
	Account *account = contact.prefferedAccount();

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

void disableNoGaduDescriptionUrl(Action *action)
{
	kdebugf();

	Contact contact = action->contact();
	Account *account = contact.prefferedAccount();

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

void disableNoEMail(Action *action)
{
	kdebugf();

	if (action->contacts().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	const Contact contact = action->contact();

	if (contact.email().isEmpty() || contact.email().indexOf(HtmlDocument::mailRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

KaduWindowActions::KaduWindowActions(QObject *parent) : QObject(parent)
{
	Configuration = new ActionDescription(this,
		ActionDescription::TypeGlobal, "configurationAction",
		this, SLOT(configurationActionActivated(QAction *, bool)),
		"Configuration", tr("Configure Kadu...")
	);
	Configuration->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	ShowYourAccounts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "yourAccountsAction",
		this, SLOT(yourAccountsActionActivated(QAction *, bool)),
		"PersonalInfo", tr("Your accounts...")
	);

	ManageModules = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "manageModulesAction",
		ModulesManager::instance(), SLOT(showDialog(QAction *, bool)),
		"ManageModules", tr("Plugins...")
	);
	ManageModules->setShortcut("kadu_modulesmanager", Qt::ApplicationShortcut);

	ExitKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(exitKaduActionActivated(QAction *, bool)),
		"Exit", tr("&Exit")
	);

	AddUser = new ActionDescription(this,
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		"AddUser", tr("Add Buddy...")
	);
	AddUser->setShortcut("kadu_adduser", Qt::ApplicationShortcut);
	ContactsListViewMenuManager::instance()->insertManagementActionDescription(0, AddUser);

	MergeContact = new ActionDescription(this,
		ActionDescription::TypeUser, "mergeContactAction",
		this, SLOT(mergeContactActionActivated(QAction *, bool)),
		"MergeContact", tr("Merge Buddy...")
	);
	ContactsListViewMenuManager::instance()->insertManagementActionDescription(1, MergeContact);
	
	ContactsListViewMenuManager::instance()->insertManagementActionDescription(2, 0);

	AddGroup = new ActionDescription(this,
		ActionDescription::TypeGlobal, "addGroupAction",
		this, SLOT(addGroupActionActivated(QAction *, bool)),
		//TODO 0.6.6 proper icon
		"AddUser", tr("Add Group...")
	);

	OpenSearch = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(openSearchActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search for Buddies...")
	);

	ManageIgnored = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "manageIgnoredAction",
		this, SLOT(manageIgnoredActionActivated(QAction *, bool)),
		"Ignore", tr("Ignored Buddies...")
	);

	ImportExportContacts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "importExportUserlisAction",
		this, SLOT(importExportContactsActionActivated(QAction *, bool)),
		"ImportExport", tr("I&mport / Export userlist")
	); //TODO 0.6.6: remove

	Help = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(helpActionActivated(QAction *, bool)),
		"HelpMenuItem", tr("Getting H&elp")
	);

	Bugs = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugsActionActivated(QAction *, bool)),
		"", tr("Submitt Bug Report")
	);

	Support = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "supportAction",
		this, SLOT(supportActionActivated(QAction *, bool)),
		"", tr("Support us")
	);

	GetInvolved = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolvedActionActivated(QAction *, bool)),
		"", tr("Get Involved")
	);

	About = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(aboutActionActivated(QAction *, bool)),
		"AboutMenuItem", tr("A&bout Kadu")
	);

	CopyDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		"CopyDescription", tr("Copy description"), false, "",
		disableNoGaduDescription
	);
	ContactsListViewMenuManager::instance()->addListActionDescription(CopyDescription);

	CopyPersonalInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		"CopyPersonalInfo", tr("Copy personal info")
	);
	ContactsListViewMenuManager::instance()->addListActionDescription(CopyPersonalInfo);

	OpenDescriptionLink = new ActionDescription(this,
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		"OpenDescriptionLink", tr("Open description link in browser"), false, "",
		disableNoGaduDescriptionUrl
	);
	ContactsListViewMenuManager::instance()->addListActionDescription(OpenDescriptionLink);

	WriteEmail = new ActionDescription(this,
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEmailActionActivated(QAction *, bool)),
		"WriteEmail", tr("Write email message"), false, "",
		disableNoEMail
	);
	ContactsListViewMenuManager::instance()->addListActionDescription(WriteEmail);

	LookupUserInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search in directory"), false, "",
		disableNoGaduUle
	);

	ContactsListViewMenuManager::instance()->insertManagementActionDescription(3, ChatWidgetManager::instance()->actions()->ignoreUser());
	ContactsListViewMenuManager::instance()->insertManagementActionDescription(4, ChatWidgetManager::instance()->actions()->blockUser());

	OfflineToUser = new ActionDescription(this,
		ActionDescription::TypeUser, "offlineToUserAction",
		this, SLOT(offlineToUserActionActivated(QAction *, bool)),
		"Offline", tr("Offline to user"), true, "",
		checkOfflineTo
	);
	ContactsListViewMenuManager::instance()->addManagementActionDescription(OfflineToUser);

	HideDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "hideDescriptionAction",
		this, SLOT(hideDescriptionActionActivated(QAction *, bool)),
		"ShowDescription_off", tr("Hide description"), true, "",
		checkHideDescription
	);
	ContactsListViewMenuManager::instance()->addManagementActionDescription(HideDescription);
	ContactsListViewMenuManager::instance()->addManagementSeparator();

	DeleteUsers = new ActionDescription(this,
		ActionDescription::TypeUser, "deleteUsersAction",
		this, SLOT(deleteUsersActionActivated(QAction *, bool)),
		"RemoveFromUserlist", tr("Delete")
	);
	DeleteUsers->setShortcut("kadu_deleteuser");
	ContactsListViewMenuManager::instance()->addManagementActionDescription(DeleteUsers);

	InactiveUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		"ShowHideInactiveUsers", tr("Hide offline users"),
		true, tr("Show offline users")
	);
	connect(InactiveUsers, SIGNAL(actionCreated(Action *)), this, SLOT(inactiveUsersActionCreated(Action *)));
	InactiveUsers->setShortcut("kadu_showoffline");

	DescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		"ShowOnlyDescriptionUsers", tr("Hide users without description"),
		true, tr("Show users without description")
	);
	connect(DescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(descriptionUsersActionCreated(Action *)));
	DescriptionUsers->setShortcut("kadu_showonlydesc");

	OnlineAndDescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		"ShowOnlineAndDescriptionUsers", tr("Show only online and description users"),
		true, tr("Show all users")
	);
	connect(OnlineAndDescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(onlineAndDescUsersActionCreated(Action *)));

	ContactsListViewMenuManager::instance()->addSeparator();
	ContactsListViewMenuManager::instance()->addSeparator();

	EditUser = new ActionDescription(this,
		ActionDescription::TypeUser, "editUserAction",
		this, SLOT(editUserActionActivated(QAction *, bool)),
		"EditUserInfo", tr("Buddy Properties"), false, QString::null,
		disableNotOneUles
	);
	connect(EditUser, SIGNAL(actionCreated(Action *)), this, SLOT(editUserActionCreated(Action *)));
	EditUser->setShortcut("kadu_persinfo");
	ContactsListViewMenuManager::instance()->addActionDescription(EditUser);

	ShowStatus = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openStatusAction",
		this, SLOT(showStatusActionActivated(QAction *, bool)),
		"Offline", tr("Change status")
	);
	connect(ShowStatus, SIGNAL(actionCreated(Action *)), this, SLOT(showStatusActionCreated(Action *)));

	UseProxy = new ActionDescription(this,
		ActionDescription::TypeGlobal, "useProxyAction",
		this, SLOT(useProxyActionActivated(QAction *, bool)),
		"UseProxy", tr("Use proxy"), true, tr("Don't use proxy")
	);
	connect(UseProxy, SIGNAL(actionCreated(Action *)), this, SLOT(useProxyActionCreated(Action *)));

	connect(StatusChangerManager::instance(), SIGNAL(statusChanged(Status)), this, SLOT(statusChanged(Status)));
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::statusChanged(Status status)
{
	Account *account = AccountManager::instance()->defaultAccount();
	if (!account)
		return;

	QIcon icon = account->statusPixmap(status);
	foreach (Action *action, ShowStatus->actions())
		action->setIcon(icon);
}

void KaduWindowActions::inactiveUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->contactsListView())
		return;

	bool enabled = !config_file.readBoolEntry("General", "ShowOffline");
	OfflineContactFilter *ofcf = new OfflineContactFilter(action);
	ofcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(ofcf));
	action->setChecked(enabled);

	window->contactsListView()->addFilter(ofcf);
}

void KaduWindowActions::descriptionUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->contactsListView())
		return;

	bool enabled = !config_file.readBoolEntry("General", "ShowWithoutDescription");
	HasDescriptionContactFilter *hdcf = new HasDescriptionContactFilter(action);
	hdcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(hdcf));
	action->setChecked(enabled);

	window->contactsListView()->addFilter(hdcf);
}

void KaduWindowActions::onlineAndDescUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->contactsListView())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowOnlineAndDescription");
	OnlineAndDescriptionContactFilter *oadcf = new OnlineAndDescriptionContactFilter(action);
	oadcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(oadcf));
	action->setChecked(enabled);

	window->contactsListView()->addFilter(oadcf);
}

void KaduWindowActions::editUserActionCreated(Action *action)
{
	MainWindow *window = dynamic_cast<MainWindow *>(action->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isAnonymous())
	{
		action->setIcon(IconsManager::instance()->loadIcon("AddUser"));
		action->setText(tr("Add user"));
	}
}

void KaduWindowActions::showStatusActionCreated(Action *action)
{
	Account *account = AccountManager::instance()->defaultAccount();

	if (account)
		action->setIcon(account->protocol()->statusPixmap());
}

void KaduWindowActions::useProxyActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Network", "UseProxy", false));
}

void KaduWindowActions::configurationActionActivated(QAction *sender, bool toggled)
{
	MainConfigurationWindow::instance()->show();
}

void KaduWindowActions::yourAccountsActionActivated(QAction *sender, bool toggled)
{
	(new YourAccounts())->show();
}

void KaduWindowActions::exitKaduActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	// TODO: 0.6.6
	//if (measureTime)
	//{
	//	time_t sec;
	//	int msec;
	//	getTime(&sec, &msec);
	//	endingTime = (sec % 1000) * 1000 + msec;
	//}
	qApp->quit();
}

void KaduWindowActions::addUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(window);

	if (contact.isAnonymous())
		addBuddyWindow->setContact(contact);

	addBuddyWindow->show();

 	kdebugf2();
}

void KaduWindowActions::mergeContactActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (!contact.isNull())
	{
		MergeContactsWindow *mergeContactsWindow = new MergeContactsWindow(contact, window);
		mergeContactsWindow->show();
	}

	kdebugf2();
}

void KaduWindowActions::addGroupActionActivated(QAction *sender, bool toggled)
{
	bool ok;
	QString newGroupName = QInputDialog::getText(dynamic_cast<QWidget *>(sender->parent()), tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString::null, &ok);

	if (ok && !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
}

void KaduWindowActions::openSearchActionActivated(QAction *sender, bool toggled)
{
	(new SearchWindow(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::manageIgnoredActionActivated(QAction *sender, bool toggled)
{
	(new Ignored(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::importExportContactsActionActivated(QAction *sender, bool toggled)
{
	(new UserlistImportExport(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::helpActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Pomoc_online");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Help_online");
}

void KaduWindowActions::bugsActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/B%C5%82%C4%99dy");
	else
		openWebBrowser("http://www.kadu.net/w/English:Bugs");
}

void KaduWindowActions::supportActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Kadu:Site_support");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Site_support");
}

void KaduWindowActions::getInvolvedActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Do%C5%82%C4%85cz");
	else
		openWebBrowser("http://www.kadu.net/w/English:GetInvolved");
}

void KaduWindowActions::aboutActionActivated(QAction *sender, bool toggled)
{
	(new ::About(Core::instance()->kaduWindow()))->show();
}

void KaduWindowActions::writeEmailActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	if (!contact.email().isEmpty())
		openMailClient(contact.email());

	kdebugf2();
}

void KaduWindowActions::copyDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	Account *account = contact.prefferedAccount();
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

void KaduWindowActions::openDescriptionLinkActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	Account *account = contact.prefferedAccount();
	ContactAccountData *data = contact.accountData(account);

	if (!data)
		return;

	QString description = data->status().description();
	if (description.isEmpty())
		return;

	QRegExp url = HtmlDocument::urlRegExp();
	int idx_start = url.indexIn(description);
	if (idx_start >= 0)
		openWebBrowser(description.mid(idx_start, url.matchedLength()));

	kdebugf2();
}

void KaduWindowActions::copyPersonalInfoActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();

	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	foreach (Contact contact, contacts)
		infoList.append(Parser::parse(copyPersonalDataSyntax, contact.prefferedAccount(), contact, false));

	QString info = infoList.join("\n");
	if (info.isEmpty())
		return;

	QApplication::clipboard()->setText(info, QClipboard::Selection);
	QApplication::clipboard()->setText(info, QClipboard::Clipboard);

	kdebugf2();
}

void KaduWindowActions::lookupInDirectoryActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(), contact);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void KaduWindowActions::offlineToUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	if (toggled && !config_file.readBoolEntry("General", "PrivateStatus"))
	{
// TODO: 0.6.6
// 		if (MessageBox::ask("You need to have private status to do it, would you like to set private status now?"))
// 			changePrivateStatus->setChecked(true);
// 		else
// 		{
// 			sender->setChecked(!toggled);
// 			return;
// 		}
	}

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	bool on = true;
	foreach (const Contact contact, contacts)
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

	foreach (Action *action, OfflineToUser->actions())
		if (action->contacts() == contacts)
			action->setChecked(!on);

	kdebugf2();
}

void KaduWindowActions::hideDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();

	foreach (const Contact &contact, contacts)
	{
		if (contact.isNull() || contact.isAnonymous())
			continue;

		ContactKaduData *ckd = contact.moduleData<ContactKaduData>(true);
		if (!ckd)
			continue;

		if (ckd->hideDescription() != toggled)
		{
			ckd->setHideDescription(toggled);
			ckd->storeConfiguration();
			delete ckd;
		}
	}

	foreach (Action *action, HideDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(toggled);
	}

	kdebugf2();
}

void KaduWindowActions::deleteUsersActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	if (contacts.isEmpty())
		return;

	QStringList displays;
	foreach (Contact contact, contacts)
		displays.append(contact.display());
	if (MessageBox::ask(tr("Selected users:\n%0 will be deleted. Are you sure?").arg(displays.join(", ")), "Warning", Core::instance()->kaduWindow()))
	{
		foreach (Contact contact, contacts)
			ContactManager::instance()->removeContact(contact);
		ContactManager::instance()->store();
	}

	kdebugf2();
}

void KaduWindowActions::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<OfflineContactFilter *>())
	{
		OfflineContactFilter *ofcf = v.value<OfflineContactFilter *>();
		ofcf->setEnabled(toggled);
	}
}

void KaduWindowActions::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<HasDescriptionContactFilter *>())
	{
		HasDescriptionContactFilter *hdcf = v.value<HasDescriptionContactFilter *>();
		hdcf->setEnabled(toggled);
	}
}

void KaduWindowActions::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<OnlineAndDescriptionContactFilter *>())
	{
		OnlineAndDescriptionContactFilter *oadcf = v.value<OnlineAndDescriptionContactFilter *>();
		oadcf->setEnabled(toggled);
	}
}

void KaduWindowActions::editUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	if (contact.isAnonymous())
	{
		AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(window);
		addBuddyWindow->setContact(contact);
		addBuddyWindow->show();
	}
	else
		(new ContactDataWindow(contact, window))->show();

	kdebugf2();
}

void KaduWindowActions::showStatusActionActivated(QAction *sender, bool toggled)
{ // TODO: 0.6.6
// 	QMenu *menu = new QMenu();
// 	StatusMenu *status = new StatusMenu(menu);
// 	status->addToMenu(menu);
// 	menu->exec(QCursor::pos());
// 	delete menu;
}

void KaduWindowActions::useProxyActionActivated(QAction *sender, bool toggled)
{
	config_file.writeEntry("Network", "UseProxy", toggled);

	foreach (Action *action, UseProxy->actions())
		action->setChecked(toggled);
}

// void Kadu::setProxyActionsStatus() TODO: 0.6.6
// {
// 	setProxyActionsStatus(config_file.readBoolEntry("Network", "UseProxy", false));
// }
