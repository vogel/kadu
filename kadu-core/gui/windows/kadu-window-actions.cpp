/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QLocale>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-kadu-data.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "buddies/filter/has-description-buddy-filter.h"
#include "buddies/filter/ignored-buddy-filter.h"
#include "buddies/filter/offline-buddy-filter.h"
#include "buddies/filter/online-and-description-buddy-filter.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/buddy-data-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/merge-buddies-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/search-window.h"
#include "gui/windows/your-accounts.h"
#include "misc/misc.h"
#include "parser/parser.h"
#include "status/status-changer-manager.h"
#include "status/status-container-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "about.h"
#include "debug.h"
#include "ignore.h"
#include "modules.h"

#include "kadu-window-actions.h"

void disableNonIdUles(Action *action)
{
	kdebugf();
	foreach (const Contact &contact, action->contacts())
		if (contact.isNull())
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);
	kdebugf2();
}

void disableContainsSelfUles(Action *action)
{
	if (action->buddies().contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
}

void checkOfflineTo(Action *action)
{
	kdebugf();
	bool on = true;
	foreach (const Buddy buddy, action->buddies())
		if (!buddy.isOfflineTo())
		{
			on = false;
			break;
		}
	action->setChecked(on);
	kdebugf2();
}

void checkHideDescription(Action *action)
{
	action->setEnabled(true);

	bool on = false;
	foreach (const Buddy buddy, action->buddies())
	{
		BuddyKaduData *ckd = 0;
		if (buddy.data())
			ckd = buddy.data()->moduleStorableData<BuddyKaduData>("kadu", false);
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

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduDescription(Action *action)
{
	kdebugf();

	Contact contact = action->contact();

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	if (contact.currentStatus().description().isEmpty())
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

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	Status status = contact.currentStatus();
	if (status.description().isEmpty())
	{
		action->setEnabled(false);
		return;
	}

	if (status.description().indexOf(UrlHandlerManager::instance()->urlRegExp()) < 0)
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

	const Buddy buddy = action->contact().ownerBuddy();

	if (buddy.email().isEmpty() || buddy.email().indexOf(UrlHandlerManager::instance()->mailRegExp()) < 0)
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
		"16x16/preferences-other.png", "16x16/preferences-other.png", tr("Preferences...")
	);
	Configuration->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	ShowYourAccounts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "yourAccountsAction",
		this, SLOT(yourAccountsActionActivated(QAction *, bool)),
		"16x16/x-office-address-book.png", "16x16/x-office-address-book.png", tr("Manage Accounts...")
	);

	ManageModules = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "manageModulesAction",
		ModulesManager::instance(), SLOT(showWindow(QAction *, bool)),
		"kadu_icons/kadu-modmanager.png", "kadu_icons/kadu-modmanager.png", tr("Plugins...")
	);
	ManageModules->setShortcut("kadu_modulesmanager", Qt::ApplicationShortcut);

	ExitKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(exitKaduActionActivated(QAction *, bool)),
		"16x16/application-exit.png", "16x16/application-exit.png", tr("&Quit")
	);
	ExitKadu->setShortcut("kadu_exit", Qt::ApplicationShortcut);

	AddUser = new ActionDescription(this,
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		"16x16/contact-new.png", "16x16/contact-new.png", tr("Add Buddy...")
	);
	AddUser->setShortcut("kadu_adduser", Qt::ApplicationShortcut);

	AddGroup= new ActionDescription(this,
		ActionDescription::TypeGlobal, "addGroupAction",
		this, SLOT(addGroupActionActivated(QAction *, bool)),
		//TODO 0.6.6 proper icon
		"16x16/contact-new.png", "16x16/contact-new.png", tr("Add Group...")
	);

	OpenSearch = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(openSearchActionActivated(QAction *, bool)),
		"16x16/edit-find.png", "16x16/edit-find.png", tr("Search for Buddy...")
	);

	ManageIgnored = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "manageIgnoredAction",
		this, SLOT(manageIgnoredActionActivated(QAction *, bool)),
		"kadu_icons/kadu-manageignored.png", "kadu_icons/kadu-manageignored.png", tr("Ignored Buddies...")
	);

	Help = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(helpActionActivated(QAction *, bool)),
		"16x16/help-contents.png", "16x16/help-contents.png", tr("Getting H&elp")
	);

	Bugs = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugsActionActivated(QAction *, bool)),
		"", "", tr("Report a Bug...")
	);

	Support = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "supportAction",
		this, SLOT(supportActionActivated(QAction *, bool)),
		"", "", tr("Support Us")
	);

	GetInvolved = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolvedActionActivated(QAction *, bool)),
		"", "", tr("Get Involved")
	);

	About = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(aboutActionActivated(QAction *, bool)),
		"16x16/help-about.png", "16x16/help-about.png", tr("A&bout Kadu")
	);

	Translate = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "translateAction",
		this, SLOT(translateActionActivated(QAction *, bool)),
		"", "", tr("Translate Kadu...")
	);
// TODO 0.6.6: icon
	ShowInfoPanel = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showInfoPanelAction",
		this, SLOT(showInfoPanelActionActivated(QAction *, bool)),
		"", "", tr("Show Information Panel"), true
	);
	connect(ShowInfoPanel, SIGNAL(actionCreated(Action *)), this, SLOT(showInfoPanelActionCreated(Action *)));
// TODO 0.6.6: icon
	ShowIgnoredBuddies = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showIgnoredAction",
		this, SLOT(showIgnoredActionActivated(QAction *, bool)),
		"", "", tr("Show Ignored Buddies"), true
	);
	connect(ShowIgnoredBuddies, SIGNAL(actionCreated(Action *)), this, SLOT(showIgnoredActionCreated(Action *)));


	CopyDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		"16x16/edit-copy.png", "16x16/edit-copy.png", tr("Copy Description"), false, "",
		disableNoGaduDescription
	);
	BuddiesListViewMenuManager::instance()->addListActionDescription(CopyDescription);

	CopyPersonalInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		"kadu_icons/kadu-copypersonal.png", "kadu_icons/kadu-copypersonal.png", tr("Copy Personal Info")
	);
	BuddiesListViewMenuManager::instance()->addListActionDescription(CopyPersonalInfo);

	OpenDescriptionLink = new ActionDescription(this,
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		"16x16/go-jump.png", "16x16/go-jump.png", tr("Open Description Link in Browser..."), false, "",
		disableNoGaduDescriptionUrl
	);
	BuddiesListViewMenuManager::instance()->addListActionDescription(OpenDescriptionLink);

	WriteEmail = new ActionDescription(this,
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEmailActionActivated(QAction *, bool)),
		"16x16/mail-message-new.png", "16x16/mail-message-new.png", tr("Send E-Mail"), false, "",
		disableNoEMail
	);
	BuddiesListViewMenuManager::instance()->addActionDescription(WriteEmail);

	LookupUserInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		"16x16/edit-find.png", "16x16/edit-find.png", tr("Search in Directory"), false, "",
		disableNoGaduUle
	);

	OfflineToUser = new ActionDescription(this,
		ActionDescription::TypeUser, "offlineToUserAction",
		this, SLOT(offlineToUserActionActivated(QAction *, bool)),
		"protocols/gadu-gadu/16x16/offline.png", "protocols/gadu-gadu/16x16/offline.png", tr("Offline to User"), true, "",
		checkOfflineTo
	);

	HideDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "hideDescriptionAction",
		this, SLOT(hideDescriptionActionActivated(QAction *, bool)),
		"kadu_icons/kadu-descriptions_on.png", "kadu_icons/kadu-descriptions_off.png", tr("Hide Description"), true, "",
		checkHideDescription
	);

	InactiveUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		"protocols/gadu-gadu/16x16/offline.png", "protocols/gadu-gadu/16x16/offline.png", tr("Show Offline Users"),
		true//, tr("Show offline users")
	);
	connect(InactiveUsers, SIGNAL(actionCreated(Action *)), this, SLOT(inactiveUsersActionCreated(Action *)));
	InactiveUsers->setShortcut("kadu_showoffline");

	DescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		"kadu_icons/kadu-showdescriptionusers_off.png", "kadu_icons/kadu-showdescriptionusers.png", tr("Hide Users Without Description"),
		true, tr("Show Users Without Description")
	);
	connect(DescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(descriptionUsersActionCreated(Action *)));
	DescriptionUsers->setShortcut("kadu_showonlydesc");

	OnlineAndDescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		"kadu_icons/kadu-onoff_onlineandd_off.png", "kadu_icons/kadu-onoff_onlineandd.png", tr("Show Only Online and Description Users"),
		true, tr("Show all users")
	);
	connect(OnlineAndDescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(onlineAndDescUsersActionCreated(Action *)));

	BuddiesListViewMenuManager::instance()->addSeparator();

	EditUser = new ActionDescription(this,
		ActionDescription::TypeUser, "editUserAction",
		this, SLOT(editUserActionActivated(QAction *, bool)),
		"16x16/x-office-address-book", "16x16/x-office-address-book", tr("View Buddy Properties"), false, QString::null,
		disableNotOneUles
	);
	connect(EditUser, SIGNAL(actionCreated(Action *)), this, SLOT(editUserActionCreated(Action *)));
	EditUser->setShortcut("kadu_persinfo");
	BuddiesListViewMenuManager::instance()->addActionDescription(EditUser);

	BuddiesListViewMenuManager::instance()->addSeparator();

	MergeContact = new ActionDescription(this,
		ActionDescription::TypeUser, "mergeContactAction",
		this, SLOT(mergeContactActionActivated(QAction *, bool)),
		"", "", tr("Merge Buddies...")
	);
	BuddiesListViewMenuManager::instance()->addActionDescription(MergeContact);

	BuddiesListViewMenuManager::instance()->addActionDescription(ChatWidgetManager::instance()->actions()->ignoreUser());
	BuddiesListViewMenuManager::instance()->addActionDescription(ChatWidgetManager::instance()->actions()->blockUser());

	DeleteUsers = new ActionDescription(this,
		ActionDescription::TypeUser, "deleteUsersAction",
		this, SLOT(deleteUsersActionActivated(QAction *, bool)),
		"16x16/edit-delete.png", "16x16/edit-delete.png", tr("Delete Buddy...")
	);
	DeleteUsers->setShortcut("kadu_deleteuser");
	BuddiesListViewMenuManager::instance()->addActionDescription(DeleteUsers);

	ShowStatus = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openStatusAction",
		this, SLOT(showStatusActionActivated(QAction *, bool)),
		"protocols/gadu-gadu/16x16/offline.png", "protocols/gadu-gadu/16x16/offline.png", tr("Change Status")
	);
	connect(ShowStatus, SIGNAL(actionCreated(Action *)), this, SLOT(showStatusActionCreated(Action *)));

	UseProxy = new ActionDescription(this,
		ActionDescription::TypeGlobal, "useProxyAction",
		this, SLOT(useProxyActionActivated(QAction *, bool)),
		"kadu_icons/kadu-proxy.png", "kadu_icons/kadu-proxy_off.png", tr("Use Proxy"), true, tr("Don't Use Proxy")
	);
	connect(UseProxy, SIGNAL(actionCreated(Action *)), this, SLOT(useProxyActionCreated(Action *)));

	connect(StatusChangerManager::instance(), SIGNAL(statusChanged(StatusContainer *, Status)), this, SLOT(statusChanged(StatusContainer *, Status)));
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::statusChanged(StatusContainer *container, Status status)
{
	if (!container)
		return;

	// TODO: 0.6.6, this really SUXX
	QIcon icon = container->statusIcon(status).pixmap(16, 16);
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

	bool enabled = config_file.readBoolEntry("General", "ShowOffline");
	OfflineBuddyFilter *ofcf = new OfflineBuddyFilter(action);
	ofcf->setEnabled(!enabled);

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
	HasDescriptionBuddyFilter *hdcf = new HasDescriptionBuddyFilter(action);
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
	OnlineAndDescriptionBuddyFilter *oadcf = new OnlineAndDescriptionBuddyFilter(action);
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

	Buddy buddy = window->contact().ownerBuddy();
	if (buddy.isAnonymous())
	{
		action->setIcon(IconsManager::instance()->iconByPath("16x16/contact-new.png"));
		action->setText(tr("Add user"));
	}
}

void KaduWindowActions::showStatusActionCreated(Action *action)
{
	Account account = AccountManager::instance()->defaultAccount();

	if (account.protocolHandler())
		action->setIcon(account.protocolHandler()->statusIcon().pixmap(16,16));
}

void KaduWindowActions::useProxyActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Network", "UseProxy", false));
}

void KaduWindowActions::showInfoPanelActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Look", "ShowInfoPanel"));
}

void KaduWindowActions::showIgnoredActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->contactsListView())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowBlocked");
	IgnoredBuddyFilter *ibf = new IgnoredBuddyFilter(action);
	ibf->setEnabled(!enabled);

	action->setData(QVariant::fromValue(ibf));
	action->setChecked(enabled);

	window->contactsListView()->addFilter(ibf);
}

void KaduWindowActions::configurationActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	MainConfigurationWindow::instance()->show();
}

void KaduWindowActions::yourAccountsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	YourAccounts::instance()->show();
}

void KaduWindowActions::exitKaduActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

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
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Buddy buddy = window->contact().ownerBuddy();
	AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(window);

	if (buddy.isAnonymous())
		addBuddyWindow->setBuddy(buddy);

	addBuddyWindow->show();

 	kdebugf2();
}

void KaduWindowActions::mergeContactActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Buddy buddy = window->contact().ownerBuddy();
	if (!buddy.isNull())
	{
		MergeBuddiesWindow *mergeBuddiesWindow = new MergeBuddiesWindow(buddy, window);
		mergeBuddiesWindow->show();
	}

	kdebugf2();
}

void KaduWindowActions::addGroupActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	bool ok;
	QString newGroupName = QInputDialog::getText(dynamic_cast<QWidget *>(sender->parent()), tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString::null, &ok);

	if (ok && !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
}

void KaduWindowActions::openSearchActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	(new SearchWindow(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::manageIgnoredActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	(new Ignored(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::helpActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Pomoc_online");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Help_online");
}

void KaduWindowActions::bugsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/B%C5%82%C4%99dy");
	else
		openWebBrowser("http://www.kadu.net/w/English:Bugs");
}

void KaduWindowActions::supportActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Kadu:Site_support");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Site_support");
}

void KaduWindowActions::getInvolvedActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Do%C5%82%C4%85cz");
	else
		openWebBrowser("http://www.kadu.net/w/English:GetInvolved");
}

void KaduWindowActions::aboutActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	(new ::About(Core::instance()->kaduWindow()))->show();
}

void KaduWindowActions::translateActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	openWebBrowser("http://www.kadu.net/forum/viewforum.php?f=19");
}

void KaduWindowActions::showInfoPanelActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	Core::instance()->kaduWindow()->infoPanel()->setVisible(toggled);

	config_file.writeEntry("Look", "ShowInfoPanel", toggled);
}

void KaduWindowActions::showIgnoredActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<IgnoredBuddyFilter *>())
	{
		IgnoredBuddyFilter *ibf = v.value<IgnoredBuddyFilter *>();
		ibf->setEnabled(!toggled);
		config_file.writeEntry("General", "ShowBlocked", toggled);
	}

}

void KaduWindowActions::writeEmailActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Buddy buddy = window->contact().ownerBuddy();
	if (buddy.isNull())
		return;

	if (!buddy.email().isEmpty())
		openMailClient(buddy.email());

	kdebugf2();
}

void KaduWindowActions::copyDescriptionActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact data = window->contact();

	if (data.isNull())
		return;

	QString description = data.currentStatus().description();
	if (description.isEmpty())
		return;

	QApplication::clipboard()->setText(description, QClipboard::Selection);
	QApplication::clipboard()->setText(description, QClipboard::Clipboard);

	kdebugf2();
}

void KaduWindowActions::openDescriptionLinkActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Contact data = window->contact();

	if (data.isNull())
		return;

	QString description = data.currentStatus().description();
	if (description.isEmpty())
		return;

	QRegExp url = UrlHandlerManager::instance()->urlRegExp();
	int idx_start = url.indexIn(description);
	if (idx_start >= 0)
		openWebBrowser(description.mid(idx_start, url.matchedLength()));

	kdebugf2();
}

void KaduWindowActions::copyPersonalInfoActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();

	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	foreach (Contact contact, contacts)
		infoList.append(Parser::parse(copyPersonalDataSyntax, contact, false));

	QString info = infoList.join("\n");
	if (info.isEmpty())
		return;

	QApplication::clipboard()->setText(info, QClipboard::Selection);
	QApplication::clipboard()->setText(info, QClipboard::Clipboard);

	kdebugf2();
}

void KaduWindowActions::lookupInDirectoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Buddy buddy = window->contact().ownerBuddy();
	if (buddy.isNull())
		return;

	SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(), buddy);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void KaduWindowActions::offlineToUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	if (toggled && !config_file.readBoolEntry("General", "PrivateStatus"))
	{
// TODO: 0.6.6
// 		if (MessageDialog::ask("You need to have private status to do it, would you like to set private status now?"))
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

	BuddySet buddies = window->buddies();
	bool on = true;
	foreach (const Buddy buddy, buddies)
		if (!buddy.isOfflineTo())
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
		if (action->buddies() == buddies)
			action->setChecked(!on);

	kdebugf2();
}

void KaduWindowActions::hideDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	BuddySet buddies = window->buddies();

	foreach (const Buddy &buddy, buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		BuddyKaduData *bkd = 0;
		if (buddy.data())
			bkd = buddy.data()->moduleStorableData<BuddyKaduData>("kadu", true);
		if (!bkd)
			continue;

		if (bkd->hideDescription() != toggled)
		{
			bkd->setHideDescription(toggled);
			bkd->store();
		}
	}

	foreach (Action *action, HideDescription->actions())
		if (action->buddies() == buddies)
			action->setChecked(toggled);

	kdebugf2();
}

void KaduWindowActions::deleteUsersActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	deleteUserActionActivated(window, toggled);
}

void KaduWindowActions::deleteUserActionActivated(MainWindow* window, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	printf("Delete user action\n");

	if (!window)
	{
		printf("on empty window..\n");
		return;
	}

	BuddySet buddies = window->buddies();
	if (buddies.isEmpty())
		return;

	QStringList displays;
	foreach (Buddy buddy, buddies)
		displays.append(buddy.display());
	if (MessageDialog::ask(tr("Selected users:\n%0 will be deleted. Are you sure?").arg(displays.join(", ")), "32x32/dialog-warning.png", Core::instance()->kaduWindow()))
	{
		foreach (Buddy buddy, buddies)
			BuddyManager::instance()->removeItem(buddy);
		BuddyManager::instance()->store();
	}
}


void KaduWindowActions::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<OfflineBuddyFilter *>())
	{
		OfflineBuddyFilter *ofcf = v.value<OfflineBuddyFilter *>();
		ofcf->setEnabled(!toggled);
		config_file.writeEntry("General", "ShowOffline", toggled);
	}
}

void KaduWindowActions::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<HasDescriptionBuddyFilter *>())
	{
		HasDescriptionBuddyFilter *hdcf = v.value<HasDescriptionBuddyFilter *>();
		hdcf->setEnabled(toggled);
	}
}

void KaduWindowActions::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<OnlineAndDescriptionBuddyFilter *>())
	{
		OnlineAndDescriptionBuddyFilter *oadcf = v.value<OnlineAndDescriptionBuddyFilter *>();
		oadcf->setEnabled(toggled);
	}
}

void KaduWindowActions::editUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Buddy buddy = window->contact().ownerBuddy();
	if (buddy.isNull())
		buddy = BuddyManager::instance()->byContact(window->contact(), ActionCreateAndAdd);

	if (buddy.isAnonymous())
	{
		AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(window);
		addBuddyWindow->setBuddy(buddy);
		addBuddyWindow->show();
	}
	else
		(new BuddyDataWindow(buddy, window))->show();

	kdebugf2();
}

void KaduWindowActions::showStatusActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	StatusContainer *container = window->statusContainer();
	if (!container)
		container = StatusContainerManager::instance();

	QMenu *menu = new QMenu();
	new StatusMenu(container, menu);
	menu->exec(QCursor::pos());
	delete menu;
}

void KaduWindowActions::useProxyActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	config_file.writeEntry("Network", "UseProxy", toggled);

	foreach (Action *action, UseProxy->actions())
		action->setChecked(toggled);
}

void KaduWindowActions::configurationUpdated()
{
	if (ShowInfoPanel->action(Core::instance()->kaduWindow())->isChecked() != config_file.readBoolEntry("Look", "ShowInfoPanel"))
		ShowInfoPanel->action(Core::instance()->kaduWindow())->trigger();

	if (InactiveUsers->action(Core::instance()->kaduWindow())->isChecked() != config_file.readBoolEntry("General", "ShowOffline"))
		InactiveUsers->action(Core::instance()->kaduWindow())->trigger();

	if (ShowIgnoredBuddies->action(Core::instance()->kaduWindow())->isChecked() != config_file.readBoolEntry("General", "ShowBlocked"))
		ShowIgnoredBuddies->action(Core::instance()->kaduWindow())->trigger();

}

// void Kadu::setProxyActionsStatus() TODO: 0.6.6
// {
// 	setProxyActionsStatus(config_file.readBoolEntry("Network", "UseProxy", false));
// }
