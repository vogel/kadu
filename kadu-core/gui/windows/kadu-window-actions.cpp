/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "actions/add-group-action.h"
#include "actions/add-user-action.h"
#include "actions/exit-action.h"
#include "actions/open-forum-action.h"
#include "actions/open-get-involved-action.h"
#include "actions/open-redmine-action.h"
#include "actions/open-search-action.h"
#include "actions/open-translate-action.h"
#include "actions/show-about-window-action.h"
#include "actions/show-configuration-window-action.h"
#include "actions/show-multilogons-action.h"
#include "actions/show-your-accounts-action.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "core/injected-factory.h"
#include "core/myself.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/actions/change-status-action.h"
#include "gui/actions/chat/add-conference-action.h"
#include "gui/actions/chat/add-room-chat-action.h"
#include "gui/actions/default-proxy-action.h"
#include "gui/actions/delete-talkable-action.h"
#include "gui/actions/edit-talkable-action.h"
#include "gui/actions/recent-chats-action.h"
#include "gui/actions/talkable-tree-view/collapse-action.h"
#include "gui/actions/talkable-tree-view/expand-action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/status-icon.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/chat-widget/actions/chat-widget-actions.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/dialog/merge-buddies-dialog-widget.h"
#include "gui/widgets/status-menu.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/buddy-delete-window.h"
#include "gui/windows/kadu-dialog.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/group-edit-window.h"
#include "gui/windows/main-configuration-window-service.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/multilogon-window-service.h"
#include "gui/windows/search-window.h"
#include "gui/windows/your-accounts-window-service.h"
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

#include "about.h"
#include "debug.h"

#include "kadu-window-actions.h"

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

void disableNoDescriptionUrl(UrlHandlerManager *urlHandlerManager, Action *action)
{
	action->setEnabled(action->context()->contacts().toContact().currentStatus().description().indexOf(urlHandlerManager->urlRegExp()) >= 0);
}

void disableNoEMail(UrlHandlerManager *urlHandlerManager, Action *action)
{
	const Buddy &buddy = action->context()->buddies().toBuddy();
	bool hasMail = !buddy.email().isEmpty() && buddy.email().indexOf(urlHandlerManager->mailRegExp()) == 0;

	action->setEnabled(hasMail);
}

void disableIfContactSelected(Myself *myself, Action *action)
{
	if (!action || action->context())
		return;

	action->setEnabled(!action->context()->roles().contains(ContactRole) && !action->context()->buddies().isEmpty());

	if (action->context()->buddies().contains(myself->buddy()))
		action->setEnabled(false);
	else
		action->setEnabled(true);
}

void disableMerge(Myself *myself, Action *action)
{
	if (action->context()->buddies().isAnyTemporary())
	{
		action->setEnabled(false);
		return;
	}

	if (action->context()->buddies().contains(myself->buddy()))
		action->setEnabled(false);
	else
		action->setEnabled(true);

	if (1 != action->context()->buddies().size())
		action->setEnabled(false);
}

KaduWindowActions::KaduWindowActions(QObject *parent) : QObject(parent)
{
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void KaduWindowActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void KaduWindowActions::setAddGroupAction(AddGroupAction *addGroupAction)
{
	m_addGroupAction = addGroupAction;
}

void KaduWindowActions::setAddUserAction(AddUserAction *addUserAction)
{
	m_addUserAction = addUserAction;
}

void KaduWindowActions::setApplication(Application *application)
{
	m_application = application;
}

void KaduWindowActions::setChatWidgetActions(ChatWidgetActions *chatWidgetActions)
{
	m_chatWidgetActions = chatWidgetActions;
}

void KaduWindowActions::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void KaduWindowActions::setExitAction(ExitAction *exitAction)
{
	m_exitAction = exitAction;
}

void KaduWindowActions::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void KaduWindowActions::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void KaduWindowActions::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void KaduWindowActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void KaduWindowActions::setMultilogonWindowService(MultilogonWindowService *multilogonWindowService)
{
	m_multilogonWindowService = multilogonWindowService;
}

void KaduWindowActions::setMyself(Myself *myself)
{
	m_myself = myself;
}

void KaduWindowActions::setOpenForumAction(OpenForumAction *openForumAction)
{
	m_openForumAction = openForumAction;
}

void KaduWindowActions::setOpenGetInvolvedAction(OpenGetInvolvedAction *openGetInvolvedAction)
{
	m_openGetInvolvedAction = openGetInvolvedAction;
}

void KaduWindowActions::setOpenRedmineAction(OpenRedmineAction *openRedmineAction)
{
	m_openRedmineAction = openRedmineAction;
}

void KaduWindowActions::setOpenSearchAction(OpenSearchAction *openSearchAction)
{
	m_openSearchAction = openSearchAction;
}

void KaduWindowActions::setOpenTranslateAction(OpenTranslateAction *openTranslateAction)
{
	m_openTranslateAction = openTranslateAction;
}

void KaduWindowActions::setParser(Parser *parser)
{
	m_parser = parser;
}

void KaduWindowActions::setShowAboutWindowAction(ShowAboutWindowAction *showAboutWindowAction)
{
	m_showAboutWindowAction = showAboutWindowAction;
}

void KaduWindowActions::setShowConfigurationWindowAction(ShowConfigurationWindowAction *showConfigurationWindowAction)
{
	m_showConfigurationWindowAction = showConfigurationWindowAction;
}

void KaduWindowActions::setShowMultilogonsAction(ShowMultilogonsAction *showMultilogonsAction)
{
	m_showMultilogonsAction = showMultilogonsAction;
}

void KaduWindowActions::setShowYourAccountsAction(ShowYourAccountsAction *showYourAccountsAction)
{
	m_showYourAccountsAction = showYourAccountsAction;
}

void KaduWindowActions::setUrlHandlerManager(UrlHandlerManager *urlHandlerManager)
{
	m_urlHandlerManager = urlHandlerManager;
}

void KaduWindowActions::setUrlOpener(UrlOpener *urlOpener)
{
	m_urlOpener = urlOpener;
}

void KaduWindowActions::setYourAccountsWindowService(YourAccountsWindowService *yourAccountsWindowService)
{
	m_yourAccountsWindowService = yourAccountsWindowService;
}

void KaduWindowActions::init()
{
	m_actions->blockSignals();

	m_showConfigurationWindowAction->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	RecentChats = m_injectedFactory->makeInjected<RecentChatsAction>(this);
	m_actions->insert(RecentChats);

	m_exitAction->setShortcut("kadu_exit", Qt::ApplicationShortcut);
	m_addUserAction->setShortcut("kadu_adduser", Qt::ApplicationShortcut);

	AddConference = m_injectedFactory->makeInjected<AddConferenceAction>(this);
	m_actions->insert(AddConference);

	AddRoomChat = m_injectedFactory->makeInjected<AddRoomChatAction>(this);
	m_actions->insert(AddRoomChat);

	ShowInfoPanel = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeMainMenu, "showInfoPanelAction",
		this, SLOT(showInfoPanelActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-information-panel"), tr("Show Information Panel"), true
	);
	connect(ShowInfoPanel, SIGNAL(actionCreated(Action *)), this, SLOT(showInfoPanelActionCreated(Action *)));

	ShowBlockedBuddies = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeMainMenu, "showIgnoredAction",
		this, SLOT(showBlockedActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-blocked-buddies"), tr("Show Blocked Buddies"), true
	);
	connect(ShowBlockedBuddies, SIGNAL(actionCreated(Action *)), this, SLOT(showBlockedActionCreated(Action *)));

	ShowMyself = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeMainMenu, "showMyselfAction",
		this, SLOT(showMyselfActionActivated(QAction *, bool)),
		KaduIcon(), tr("Show Myself Buddy"), true
	);
	connect(ShowMyself, SIGNAL(actionCreated(Action *)), this, SLOT(showMyselfActionCreated(Action *)));

	auto expandAction = m_injectedFactory->makeInjected<ExpandAction>(this);
	m_actions->insert(expandAction);

	auto collapseAction = m_injectedFactory->makeInjected<CollapseAction>(this);
	m_actions->insert(collapseAction);

	m_menuInventory
		->menu("buddy-list")
		->addAction(expandAction, KaduMenu::SectionActionsGui, 2);
	m_menuInventory
		->menu("buddy-list")
		->addAction(collapseAction, KaduMenu::SectionActionsGui, 1);

	CopyDescription = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		KaduIcon("edit-copy"), tr("Copy Description"), false,
		disableNoDescription
	);
	m_menuInventory
		->menu("buddy-list")
		->addAction(CopyDescription, KaduMenu::SectionActions, 10);

	CopyPersonalInfo = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/copy-personal-info"), tr("Copy Personal Info")
	);
	m_menuInventory
		->menu("buddy-list")
		->addAction(CopyPersonalInfo, KaduMenu::SectionActions, 20);

	OpenDescriptionLink = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		KaduIcon("go-jump"), tr("Open Description Link in Browser"), false,
		[this](Action *action){ return disableNoDescriptionUrl(m_urlHandlerManager, action); }
	);
	m_menuInventory
		->menu("buddy-list")
		->addAction(OpenDescriptionLink, KaduMenu::SectionActions, 30);

	WriteEmail = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEmailActionActivated(QAction *, bool)),
		KaduIcon("mail-message-new"), tr("Send E-Mail"), false,
		[this](Action *action){ return disableNoEMail(m_urlHandlerManager, action); }
	);
	connect(WriteEmail, SIGNAL(actionCreated(Action *)), this, SLOT(writeEmailActionCreated(Action *)));

	m_menuInventory
		->menu("buddy-list")
		->addAction(WriteEmail, KaduMenu::SectionSend, 200);

	LookupUserInfo = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		KaduIcon("edit-find"), tr("Search in Directory"), false,
		disableNoSearchService
	);

	InactiveUsers = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-offline-buddies"), tr("Show Offline Buddies"),
		true
	);
	connect(InactiveUsers, SIGNAL(actionCreated(Action *)), this, SLOT(inactiveUsersActionCreated(Action *)));
	InactiveUsers->setShortcut("kadu_showoffline");

	DescriptionUsers = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/only-show-with-description"), tr("Only Show Buddies with Description"),
		true
	);
	connect(DescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(descriptionUsersActionCreated(Action *)));
	DescriptionUsers->setShortcut("kadu_showonlydesc");

	ShowDescriptions = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUserList, "descriptionsAction",
		this, SLOT(showDescriptionsActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/show-descriptions"), tr("Show Descriptions"),
		true
	);
	connect(ShowDescriptions, SIGNAL(actionCreated(Action *)), this, SLOT(showDescriptionsActionCreated(Action *)));

	OnlineAndDescriptionUsers = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/only-show-online-and-with-description"), tr("Only Show Online Buddies and Buddies with Description"),
		true
	);
	connect(OnlineAndDescriptionUsers, SIGNAL(actionCreated(Action *)), this, SLOT(onlineAndDescUsersActionCreated(Action *)));

	EditTalkable = m_injectedFactory->makeInjected<EditTalkableAction>(this);
	m_actions->insert(EditTalkable);

	m_menuInventory
		->menu("buddy-list")
		->addAction(EditTalkable, KaduMenu::SectionView);

	MergeContact = m_injectedFactory->makeInjected<ActionDescription>(this,
		ActionDescription::TypeUser, "mergeContactAction",
		this, SLOT(mergeContactActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/merge-buddies"), tr("Merge Buddies..."), false,
		[this](Action *action){ return disableMerge(m_myself, action); }
	);

	m_menuInventory
		->menu("buddy-list")
		->addAction(MergeContact, KaduMenu::SectionManagement, 100);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_chatWidgetActions->blockUser(), KaduMenu::SectionManagement, 500);

	DeleteTalkable = m_injectedFactory->makeInjected<DeleteTalkableAction>(this);
	m_actions->insert(DeleteTalkable);

	m_menuInventory
		->menu("buddy-list")
		->addAction(DeleteTalkable, KaduMenu::SectionManagement, 1000);

	// The last ActionDescription will send actionLoaded() signal.
	// TODO It will not reflect all action types (see MainWindow::actionLoadedOrUnloaded() method)
	// but will work good since KaduActions is created very early. Of course we still need a better mechanism for that.
	m_actions->unblockSignals();

	ChangeStatus = m_injectedFactory->makeInjected<ChangeStatusAction>(this);
	m_actions->insert(ChangeStatus);

	DefaultProxy = m_injectedFactory->makeInjected<DefaultProxyAction>(this);
	m_actions->insert(DefaultProxy);
}

void KaduWindowActions::inactiveUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = m_configuration->deprecatedApi()->readBoolEntry("General", "ShowOffline");
	auto filter = m_injectedFactory->makeInjected<HideOfflineTalkableFilter>(action);
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

	bool enabled = !m_configuration->deprecatedApi()->readBoolEntry("General", "ShowWithoutDescription");
	auto filter = m_injectedFactory->makeInjected<HideWithoutDescriptionTalkableFilter>(action);
	filter->setEnabled(enabled);

	action->setData(QVariant::fromValue(filter));
	action->setChecked(enabled);

	window->talkableProxyModel()->addFilter(filter);
}

void KaduWindowActions::showDescriptionsActionCreated(Action *action)
{
	bool enabled = m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowDesc");
	action->setChecked(enabled);
}

void KaduWindowActions::onlineAndDescUsersActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = m_configuration->deprecatedApi()->readBoolEntry("General", "ShowOnlineAndDescription");
	auto filter = m_injectedFactory->makeInjected<HideOfflineWithoutDescriptionTalkableFilter>(action);
	filter->setEnabled(enabled);

	action->setData(QVariant::fromValue(filter));
	action->setChecked(enabled);

	window->talkableProxyModel()->addFilter(filter);
}

void KaduWindowActions::showInfoPanelActionCreated(Action *action)
{
	action->setChecked(m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowInfoPanel"));
}

void KaduWindowActions::showBlockedActionCreated(Action *action)
{
	MainWindow *window = qobject_cast<MainWindow *>(action->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	bool enabled = m_configuration->deprecatedApi()->readBoolEntry("General", "ShowBlocked");
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

	auto enabled = m_configuration->deprecatedApi()->readBoolEntry("General", "ShowMyself", false);
	auto model = qobject_cast<TalkableModel *>(window->talkableProxyModel()->sourceModel());
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

void KaduWindowActions::mergeContactActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Buddy &buddy = action->context()->buddies().toBuddy();
	if (!buddy)
		return;

	MergeBuddiesDialogWidget *mergeWidget = m_injectedFactory->makeInjected<MergeBuddiesDialogWidget>(buddy,
			tr("Choose which buddy would you like to merge with <i>%1</i>")
			.arg(buddy.display()), sender->parentWidget());
	KaduDialog *window = new KaduDialog(mergeWidget, sender->parentWidget());
	window->setAcceptButtonText(tr("Merge"));
	window->exec();

	kdebugf2();
}

void KaduWindowActions::showInfoPanelActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	m_kaduWindowService->kaduWindow()->infoPanel()->setVisible(toggled);

	m_configuration->deprecatedApi()->writeEntry("Look", "ShowInfoPanel", toggled);
}

void KaduWindowActions::showBlockedActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<BlockedTalkableFilter *>())
	{
		BlockedTalkableFilter *blockedTalkableFilter = v.value<BlockedTalkableFilter *>();
		blockedTalkableFilter->setEnabled(!toggled);
		m_configuration->deprecatedApi()->writeEntry("General", "ShowBlocked", toggled);
	}
}

void KaduWindowActions::showMyselfActionActivated(QAction *sender, bool toggled)
{
	auto window = qobject_cast<MainWindow *>(sender->parentWidget());
	if (!window)
		return;
	if (!window->talkableProxyModel())
		return;

	auto model = qobject_cast<TalkableModel *>(window->talkableProxyModel()->sourceModel());
	if (model)
	{
		model->setIncludeMyself(toggled);
		m_configuration->deprecatedApi()->writeEntry("General", "ShowMyself", toggled);
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
		m_urlOpener->openEmail(buddy.email().toUtf8());

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

	QRegExp url = m_urlHandlerManager->urlRegExp();
	int idx_start = url.indexIn(description);
	if (idx_start >= 0)
		m_urlOpener->openUrl(description.mid(idx_start, url.matchedLength()).toUtf8());

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
	QString defaultSyntax = m_parser->escape(tr("Contact:")) + " %a[ (%u)]\n["
			+ m_parser->escape(tr("First name:")) + " %f\n]["
			+ m_parser->escape(tr("Last name:")) + " %r\n]["
			+ m_parser->escape(tr("Mobile:")) + " %m\n]";
	QString copyPersonalDataSyntax = m_configuration->deprecatedApi()->readEntry("General", "CopyPersonalDataSyntax", defaultSyntax);
	foreach (Contact contact, contacts)
		infoList.append(m_parser->parse(copyPersonalDataSyntax, Talkable(contact), ParserEscape::NoEscape));

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
		(m_injectedFactory->makeInjected<SearchWindow>(m_kaduWindowService->kaduWindow()))->show();
		return;
	}

	auto sd = m_injectedFactory->makeInjected<SearchWindow>(m_kaduWindowService->kaduWindow(), buddy);
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
		m_configuration->deprecatedApi()->writeEntry("General", "ShowOffline", toggled);
	}
}

void KaduWindowActions::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	QVariant v = sender->data();
	if (v.canConvert<HideWithoutDescriptionTalkableFilter *>())
	{
		auto filter = v.value<HideWithoutDescriptionTalkableFilter *>();
		filter->setEnabled(toggled);
	}
}

void KaduWindowActions::showDescriptionsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	m_configuration->deprecatedApi()->writeEntry("Look", "ShowDesc", toggled);
	ConfigurationAwareObject::notifyAll();
}

void KaduWindowActions::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	m_configuration->deprecatedApi()->writeEntry("General", "ShowOnlineAndDescription", toggled);

	QVariant v = sender->data();
	if (v.canConvert<HideOfflineWithoutDescriptionTalkableFilter *>())
	{
		auto filter = v.value<HideOfflineWithoutDescriptionTalkableFilter *>();
		filter->setEnabled(toggled);
	}
}

void KaduWindowActions::configurationUpdated()
{
	ActionContext *context = m_kaduWindowService->kaduWindow()->actionContext();

	if (ShowInfoPanel->action(context)->isChecked() != m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowInfoPanel"))
		ShowInfoPanel->action(context)->trigger();

	if (InactiveUsers->action(context)->isChecked() != m_configuration->deprecatedApi()->readBoolEntry("General", "ShowOffline"))
		InactiveUsers->action(context)->trigger();

	if (ShowBlockedBuddies->action(context)->isChecked() != m_configuration->deprecatedApi()->readBoolEntry("General", "ShowBlocked"))
		ShowBlockedBuddies->action(context)->trigger();

	if (ShowMyself->action(context)->isChecked() != m_configuration->deprecatedApi()->readBoolEntry("General", "ShowMyself"))
		ShowMyself->action(context)->trigger();
}

#include "moc_kadu-window-actions.cpp"
