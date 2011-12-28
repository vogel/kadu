/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-kadu-data.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/actions/change-status-action.h"
#include "gui/actions/chat/add-conference-action.h"
#include "gui/actions/default-proxy-action.h"
#include "gui/actions/delete-talkable-action.h"
#include "gui/actions/edit-talkable-action.h"
#include "gui/status-icon.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/status-menu.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/buddy-delete-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/merge-buddies-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/modules-window.h"
#include "gui/windows/multilogon-window.h"
#include "gui/windows/search-window.h"
#include "gui/windows/your-accounts.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "os/generic/url-opener.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "talkable/filter/blocked-talkable-filter.h"
#include "talkable/filter/hide-offline-talkable-filter.h"
#include "talkable/filter/hide-offline-without-description-talkable-filter.h"
#include "talkable/filter/hide-without-description-talkable-filter.h"
#include "talkable/model/talkable-model.h"
#include "talkable/model/talkable-proxy-model.h"
#include "url-handlers/url-handler-manager.h"
#include "kadu-application.h"

#include "about.h"
#include "debug.h"

#include "kadu-window-actions.h"

void hideNoMultilogonAccounts(Action *action)
{
	bool hasMultilogonAccount = false;
	foreach (const Account &account, AccountManager::instance()->items())
		if (account.protocolHandler() && account.protocolHandler()->multilogonService())
		{
			hasMultilogonAccount = true;
			break;
		}

	action->setVisible(hasMultilogonAccount);
}

void hideNoSearchServiceAccounts(Action *action)
{
	bool hasSearchServiceAccount = false;
	foreach (const Account &account, AccountManager::instance()->items())
		if (account.protocolHandler() && account.protocolHandler()->searchService())
		{
			hasSearchServiceAccount = true;
			break;
		}

	action->setVisible(hasSearchServiceAccount);
}

void disableNoSearchService(Action *action)
{
	const Contact &contact = action->context()->contacts().toContact();
	action->setEnabled(contact
			&& contact.contactAccount().protocolHandler()
			&& contact.contactAccount().protocolHandler()->searchService());
}

void disableNoContact(Action *action)
{
	action->setEnabled(action->context()->contacts().toContact());
}

void disableNoDescription(Action *action)
{
	action->setEnabled(!action->context()->contacts().toContact().currentStatus().description().isEmpty());
}

void disableNoDescriptionUrl(Action *action)
{
	action->setEnabled(action->context()->contacts().toContact().currentStatus().description().indexOf(UrlHandlerManager::instance()->urlRegExp()) >= 0);
}

void disableNoEMail(Action *action)
{
	const Buddy &buddy = action->context()->buddies().toBuddy();
	bool hasMail = !buddy.email().isEmpty() && buddy.email().indexOf(UrlHandlerManager::instance()->mailRegExp()) == 0;

	action->setEnabled(hasMail);
}

void disableIfContactSelected(Action *action)
{
	if (action && action->context())
		action->setEnabled(!action->context()->roles().contains(ContactRole) && !action->context()->buddies().isEmpty());

	if (action->context()->buddies().contains(Core::instance()->myself()))
		action->setEnabled(false);
	else
		action->setEnabled(true);
}

void disableMerge(Action *action)
{
	disableIfContactSelected(action);

	if (1 != action->context()->buddies().size())
		action->setEnabled(false);
}

KaduWindowActions::KaduWindowActions(QObject *parent) : QObject(parent)
{
	Actions::instance()->blockSignals();

	Configuration = new ActionDescription(this,
		ActionDescription::TypeGlobal, "configurationAction",
		this, SLOT(configurationActionActivated(QAction *, bool)),
		KaduIcon("preferences-other"), tr("Preferences...")
	);
	Configuration->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	ShowYourAccounts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "yourAccountsAction",
		this, SLOT(yourAccountsActionActivated(QAction *, bool)),
		KaduIcon("x-office-address-book"), tr("Your Accounts...")
	);

	ShowMultilogons = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showMultilogonsAction",
		this, SLOT(showMultilogonsActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/multilogon"), tr("Multilogons..."), false,
		hideNoMultilogonAccounts
	);
	connect(ShowMultilogons, SIGNAL(actionCreated(Action *)), this, SLOT(showMultilogonsActionCreated(Action *)));

	ManageModules = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "manageModulesAction",
		this, SLOT(manageModulesActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/plugins"), tr("Plugins...")
	);
	ManageModules->setShortcut("kadu_modulesmanager", Qt::ApplicationShortcut);

	ExitKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(exitKaduActionActivated(QAction *, bool)),
		KaduIcon("application-exit"), tr("&Quit")
	);
	ExitKadu->setShortcut("kadu_exit", Qt::ApplicationShortcut);

	AddUser = new ActionDescription(this,
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		KaduIcon("contact-new"), tr("Add Buddy...")
	);
	AddUser->setShortcut("kadu_adduser", Qt::ApplicationShortcut);

	AddConference = new AddConferenceAction(this);

	AddGroup= new ActionDescription(this,
		ActionDescription::TypeGlobal, "addGroupAction",
		this, SLOT(addGroupActionActivated(QAction *, bool)),
		KaduIcon("group-new"), tr("Add Group...")
	);

	OpenSearch = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(openSearchActionActivated(QAction *, bool)),
		KaduIcon("edit-find"), tr("Search for Buddy..."), false,
		hideNoSearchServiceAccounts
	);
	connect(OpenSearch, SIGNAL(actionCreated(Action*)), this, SLOT(openSearchActionCreated(Action*)));

	Help = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(helpActionActivated(QAction *, bool)),
		KaduIcon("help-contents"), tr("Getting H&elp...")
	);

	Bugs = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugsActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/report-a-bug"), tr("Report a Bug...")
	);

	GetInvolved = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolvedActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/get-involved"), tr("Get Involved...")
	);

	Translate = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "translateAction",
		this, SLOT(translateActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/translate-kadu"), tr("Translate Kadu...")
	);

	About = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(aboutActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/about-kadu"), tr("A&bout Kadu")
	);

	ShowInfoPanel = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showInfoPanelAction",
		this, SLOT(showInfoPanelActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-information-panel"), tr("Show Information Panel"), true
	);
	connect(ShowInfoPanel, SIGNAL(actionCreated(Action *)), this, SLOT(showInfoPanelActionCreated(Action *)));

	ShowBlockedBuddies = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showIgnoredAction",
		this, SLOT(showBlockedActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-blocked-buddies"), tr("Show Blocked Buddies"), true
	);
	connect(ShowBlockedBuddies, SIGNAL(actionCreated(Action *)), this, SLOT(showBlockedActionCreated(Action *)));

	ShowMyself = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "showMyselfAction",
		this, SLOT(showMyselfActionActivated(QAction *, bool)),
		KaduIcon(), tr("Show Myself Buddy"), true
	);
	connect(ShowMyself, SIGNAL(actionCreated(Action *)), this, SLOT(showMyselfActionCreated(Action *)));

	CopyDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		KaduIcon("edit-copy"), tr("Copy Description"), false,
		disableNoDescription
	);
	TalkableMenuManager::instance()->addListActionDescription(CopyDescription, TalkableMenuItem::CategoryActions, 10);

	CopyPersonalInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/copy-personal-info"), tr("Copy Personal Info")
	);
	TalkableMenuManager::instance()->addListActionDescription(CopyPersonalInfo, TalkableMenuItem::CategoryActions, 20);

	OpenDescriptionLink = new ActionDescription(this,
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		KaduIcon("go-jump"), tr("Open Description Link in Browser..."), false,
		disableNoDescriptionUrl
	);
	TalkableMenuManager::instance()->addListActionDescription(OpenDescriptionLink, TalkableMenuItem::CategoryActions, 30);

	WriteEmail = new ActionDescription(this,
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEmailActionActivated(QAction *, bool)),
		KaduIcon("mail-message-new"), tr("Send E-Mail"), false,
		disableNoEMail
	);
	TalkableMenuManager::instance()->addActionDescription(WriteEmail, TalkableMenuItem::CategoryActions, 200);
	connect(WriteEmail, SIGNAL(actionCreated(Action *)), this, SLOT(writeEmailActionCreated(Action *)));

	LookupUserInfo = new ActionDescription(this,
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		KaduIcon("edit-find"), tr("Search in Directory"), false,
		disableNoSearchService
	);

	InactiveUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-offline-buddies"), tr("Show Offline Buddies"),
		true
	);
	connect(InactiveUsers, SIGNAL(actionCreated(Action *)), this, SLOT(inactiveUsersActionCreated(Action *)));
	InactiveUsers->setShortcut("kadu_showoffline");

	DescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/only-show-with-description"), tr("Only Show Buddies with Description"),
		true
	);
	connect(DescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(descriptionUsersActionCreated(Action *)));
	DescriptionUsers->setShortcut("kadu_showonlydesc");

	ShowDescriptions = new ActionDescription(this,
		ActionDescription::TypeUserList, "descriptionsAction",
		this, SLOT(showDescriptionsActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-descriptions"), tr("Show Descriptions"),
		true
	);
	connect(ShowDescriptions, SIGNAL(actionCreated(Action *)), this, SLOT(showDescriptionsActionCreated(Action *)));

	OnlineAndDescriptionUsers = new ActionDescription(this,
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/only-show-online-and-with-description"), tr("Only Show Online Buddies and Buddies with Description"),
		true
	);
	connect(OnlineAndDescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(onlineAndDescUsersActionCreated(Action *)));

	EditTalkable = new EditTalkableAction(this);
	TalkableMenuManager::instance()->addActionDescription(EditTalkable, TalkableMenuItem::CategoryView, 0);

	MergeContact = new ActionDescription(this,
		ActionDescription::TypeUser, "mergeContactAction",
		this, SLOT(mergeContactActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/merge-buddies"), tr("Merge Buddies..."), false,
		disableMerge
	);
	TalkableMenuManager::instance()->addActionDescription(MergeContact, TalkableMenuItem::CategoryManagement, 100);

	TalkableMenuManager::instance()->addActionDescription(ChatWidgetManager::instance()->actions()->blockUser(), TalkableMenuItem::CategoryManagement, 500);

	DeleteTalkable = new DeleteTalkableAction(this);
	TalkableMenuManager::instance()->addActionDescription(DeleteTalkable, TalkableMenuItem::CategoryManagement, 1000);

	// The last ActionDescription will send actionLoaded() signal.
	// TODO It will not reflect all action types (see MainWindow::actionLoadedOrUnloaded() method)
	// but will work good since KaduActions is created very early. Of course we still need a better mechanism for that.
	Actions::instance()->unblockSignals();

	ChangeStatus = new ChangeStatusAction(this);
	DefaultProxy = new DefaultProxyAction(this);
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::showMultilogonsActionCreated(Action *action)
{
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)), action, SLOT(checkState()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)), action, SLOT(checkState()));
}

void KaduWindowActions::openSearchActionCreated(Action *action)
{
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)), action, SLOT(checkState()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)), action, SLOT(checkState()));
}

void KaduWindowActions::inactiveUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowOffline");
	HideOfflineTalkableFilter *filter = new HideOfflineTalkableFilter(action);
	filter->setEnabled(!enabled);

	action->setData(QVariant::fromValue(filter));
	action->setChecked(enabled);

	window->talkableProxyModel()->addFilter(filter);
}

void KaduWindowActions::descriptionUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = !config_file.readBoolEntry("General", "ShowWithoutDescription");
	HideWithoutDescriptionTalkableFilter *filter = new HideWithoutDescriptionTalkableFilter(action);
	filter->setEnabled(enabled);

	action->setData(QVariant::fromValue(filter));
	action->setChecked(enabled);

	window->talkableProxyModel()->addFilter(filter);
}

void KaduWindowActions::showDescriptionsActionCreated(Action *action)
{
	bool enabled = config_file.readBoolEntry("Look", "ShowDesc");
	action->setChecked(enabled);
}

void KaduWindowActions::onlineAndDescUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowOnlineAndDescription");
	HideOfflineWithoutDescriptionTalkableFilter *filter = new HideOfflineWithoutDescriptionTalkableFilter(action);
	filter->setEnabled(enabled);

	action->setData(QVariant::fromValue(filter));
	action->setChecked(enabled);

	window->talkableProxyModel()->addFilter(filter);
}

void KaduWindowActions::showInfoPanelActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Look", "ShowInfoPanel"));
}

void KaduWindowActions::showBlockedActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowBlocked");
	BlockedTalkableFilter *blockedTalkableFilter = new BlockedTalkableFilter(action);
	blockedTalkableFilter->setEnabled(!enabled);

	action->setData(QVariant::fromValue(blockedTalkableFilter));
	action->setChecked(enabled);

	window->talkableProxyModel()->addFilter(blockedTalkableFilter);
}

void KaduWindowActions::showMyselfActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = config_file.readBoolEntry("General", "ShowMyself", false);
	TalkableModel *model = qobject_cast<TalkableModel *>(window->talkableProxyModel()->sourceModel());
	if (model)
	{
		model->setIncludeMyself(enabled);
		action->setChecked(enabled);
	}
}

void KaduWindowActions::writeEmailActionCreated(Action *action)
{
	const Buddy &buddy = action->context()->buddies().toBuddy();
	if (buddy)
		connect(buddy, SIGNAL(updated()), action, SLOT(checkState()));
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

void KaduWindowActions::showMultilogonsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	MultilogonWindow::instance()->show();
}

void KaduWindowActions::manageModulesActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	ModulesWindow::show();
}

void KaduWindowActions::exitKaduActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	kdebugf();

	Core::instance()->application()->quit();
}

void KaduWindowActions::addUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Buddy &buddy = action->context()->buddies().toBuddy();

	if (buddy.isAnonymous())
		(new AddBuddyWindow(action->parentWidget(), buddy, true))->show();
	else
		(new AddBuddyWindow(action->parentWidget()))->show();

	kdebugf2();
}

void KaduWindowActions::mergeContactActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Buddy &buddy = action->context()->buddies().toBuddy();
	if (buddy)
	{
		MergeBuddiesWindow *mergeBuddiesWindow = new MergeBuddiesWindow(buddy);
		mergeBuddiesWindow->show();
	}

	kdebugf2();
}

void KaduWindowActions::addGroupActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	bool ok;
	QString newGroupName = QInputDialog::getText(sender->parentWidget(), tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString(), &ok);

	if (ok && !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
}

void KaduWindowActions::openSearchActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	(new SearchWindow(sender->parentWidget()))->show();
}

void KaduWindowActions::helpActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.im/w/Pomoc");
	else
		UrlOpener::openUrl("http://www.kadu.im/w/English:Kadu:Help");
}

void KaduWindowActions::bugsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.im/w/B%C5%82%C4%99dy");
	else
		UrlOpener::openUrl("http://www.kadu.im/w/English:Bugs");
}

void KaduWindowActions::getInvolvedActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.im/w/Do%C5%82%C4%85cz");
	else
		UrlOpener::openUrl("http://www.kadu.im/w/English:GetInvolved");
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

	UrlOpener::openUrl("http://www.transifex.net/projects/p/kadu/");
}

void KaduWindowActions::showInfoPanelActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	Core::instance()->kaduWindow()->infoPanel()->setVisible(toggled);

	config_file.writeEntry("Look", "ShowInfoPanel", toggled);
}

void KaduWindowActions::showBlockedActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<BlockedTalkableFilter *>())
	{
		BlockedTalkableFilter *blockedTalkableFilter = v.value<BlockedTalkableFilter *>();
		blockedTalkableFilter->setEnabled(!toggled);
		config_file.writeEntry("General", "ShowBlocked", toggled);
	}
}

void KaduWindowActions::showMyselfActionActivated(QAction *sender, bool toggled)
{
	MainWindow *window = qobject_cast<MainWindow *>(sender->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	TalkableModel *model = qobject_cast<TalkableModel *>(window->talkableProxyModel()->sourceModel());
	if (model)
	{
		model->setIncludeMyself(toggled);
		config_file.writeEntry("General", "ShowMyself", toggled);
	}
}

void KaduWindowActions::writeEmailActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Buddy &buddy = action->context()->buddies().toBuddy();
	if (!buddy)
		return;

	if (!buddy.email().isEmpty())
		UrlOpener::openEmail(buddy.email().toUtf8());

	kdebugf2();
}

void KaduWindowActions::copyDescriptionActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Contact &contact = action->context()->contacts().toContact();
	if (!contact)
		return;

	const QString &description = contact.currentStatus().description();
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

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Contact &contact = action->context()->contacts().toContact();
	if (!contact)
		return;

	const QString &description = contact.currentStatus().description();
	if (description.isEmpty())
		return;

	QRegExp url = UrlHandlerManager::instance()->urlRegExp();
	int idx_start = url.indexIn(description);
	if (idx_start >= 0)
		UrlOpener::openUrl(description.mid(idx_start, url.matchedLength()).toUtf8());

	kdebugf2();
}

void KaduWindowActions::copyPersonalInfoActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	ContactSet contacts = action->context()->contacts();

	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	foreach (Contact contact, contacts)
		infoList.append(Parser::parse(copyPersonalDataSyntax, Talkable(contact), false));

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

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Buddy &buddy = action->context()->buddies().toBuddy();
	if (!buddy)
	{
		(new SearchWindow(Core::instance()->kaduWindow()))->show();
		return;
	}

	SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(), buddy);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void KaduWindowActions::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<HideOfflineTalkableFilter *>())
	{
		HideOfflineTalkableFilter *filter = v.value<HideOfflineTalkableFilter *>();
		filter->setEnabled(!toggled);
		config_file.writeEntry("General", "ShowOffline", toggled);
	}
}

void KaduWindowActions::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<HideWithoutDescriptionTalkableFilter *>())
	{
		HideWithoutDescriptionTalkableFilter *filter = v.value<HideWithoutDescriptionTalkableFilter *>();
		filter->setEnabled(toggled);
	}
}

void KaduWindowActions::showDescriptionsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	config_file.writeEntry("Look", "ShowDesc", toggled);
	ConfigurationAwareObject::notifyAll();
}

void KaduWindowActions::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	config_file.writeEntry("General", "ShowOnlineAndDescription", toggled);

	QVariant v = sender->data();
	if (v.canConvert<HideOfflineWithoutDescriptionTalkableFilter *>())
	{
		HideOfflineWithoutDescriptionTalkableFilter *filter = v.value<HideOfflineWithoutDescriptionTalkableFilter *>();
		filter->setEnabled(toggled);
	}
}

void KaduWindowActions::configurationUpdated()
{
	ActionContext *context = Core::instance()->kaduWindow()->actionContext();

	if (ShowInfoPanel->action(context)->isChecked() != config_file.readBoolEntry("Look", "ShowInfoPanel"))
		ShowInfoPanel->action(context)->trigger();

	if (InactiveUsers->action(context)->isChecked() != config_file.readBoolEntry("General", "ShowOffline"))
		InactiveUsers->action(context)->trigger();

	if (ShowBlockedBuddies->action(context)->isChecked() != config_file.readBoolEntry("General", "ShowBlocked"))
		ShowBlockedBuddies->action(context)->trigger();

	if (ShowMyself->action(context)->isChecked() != config_file.readBoolEntry("General", "ShowMyself"))
		ShowMyself->action(context)->trigger();
}
