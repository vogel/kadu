/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "groups_manager.h"
#include "kadu.h"
#include "misc.h"
#include "tabbar.h"
#include "userbox.h"
#include "userlist.h"

void GroupsManager::initModule()
{
	kdebugf();
	groups_manager = new GroupsManager();
	usersWithDescription = new UsersWithDescription();
	onlineUsers = new OnlineUsers();
	blockedUsers = new BlockedUsers();
	blockingUsers = new BlockingUsers();
	anonymousUsers = new AnonymousUsers();
	kdebugf2();
}

void GroupsManager::closeModule()
{
	kdebugf();
	delete groups_manager;
	groups_manager = NULL;

	UserBox *userbox = kadu->userbox();
	userbox->removeNegativeFilter(blockingUsers);
	userbox->removeNegativeFilter(blockedUsers);
	userbox->removeFilter(onlineUsers);
	userbox->removeFilter(usersWithDescription);
	userbox->removeNegativeFilter(anonymousUsers);

	delete usersWithDescription;
	usersWithDescription = NULL;

	delete onlineUsers;
	onlineUsers = NULL;

	delete blockedUsers;
	blockedUsers = NULL;

	delete blockingUsers;
	blockingUsers = NULL;

	delete anonymousUsers;
	anonymousUsers = NULL;
	kdebugf2();
}

void GroupsManager::setTabBar(KaduTabBar *bar)
{
	kdebugf();
	GroupBar = bar;

	bar->setShape(QTabBar::RoundedBelow);
	bar->addTab(new QTab(tr("All")));
	bar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(), QFont::Bold));
	connect(bar, SIGNAL(selected(int)), this, SLOT(tabSelected(int)));
	connect(userlist, SIGNAL(modified()), this, SLOT(refreshTabBarLater()));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTabBar()));
	lastId = -1;
	refreshTabBar();

	int configTab = config_file.readNumEntry( "Look", "CurrentGroupTab" );
	if (configTab >= 0 && configTab < GroupBar -> count())
		((QTabBar*) GroupBar) -> setCurrentTab(configTab);

	//to be removed in 0.6
	config_file.addVariable("General", "ShowOffline", config_file.readBoolEntry("General", "ShowHideInactive", true));
	config_file.addVariable("General", "ShowWithoutDescription", !config_file.readBoolEntry("General", "ShowOnlyDescriptionUsers", false));
	config_file.addVariable("ShortCuts", "kadu_showoffline", config_file.readEntry("ShortCuts", "kadu_showinactive", "F9"));
	//end

	ConfigDialog::addCheckBox("General", "grid-advanced", QT_TRANSLATE_NOOP("@default", "Show offline contacts"), "ShowOffline", true, QT_TRANSLATE_NOOP("@default", "Display contacts who are offline"), 0, Advanced);
	ConfigDialog::addCheckBox("General", "grid-advanced", QT_TRANSLATE_NOOP("@default", "Show contacts without description"), "ShowWithoutDescription", true, QT_TRANSLATE_NOOP("@default", "Display contacts that don't have a description"), 0, Advanced);
	ConfigDialog::addCheckBox("General", "grid-expert", QT_TRANSLATE_NOOP("@default", "Show contacts that you are blocking"), "ShowBlocked", true, 0, 0, Expert);
	ConfigDialog::addCheckBox("General", "grid-expert", QT_TRANSLATE_NOOP("@default", "Show contacts that are blocking you"), "ShowBlocking", true, 0, 0, Expert);

	//najpierw ustawiamy odwrotnie, a pó¼niej robimy x=!x;
	showBlocked = !config_file.readBoolEntry("General", "ShowBlocked");
	showBlocking = !config_file.readBoolEntry("General", "ShowBlocking");
	showOffline = !config_file.readBoolEntry("General", "ShowOffline");
	showWithoutDescription = !config_file.readBoolEntry("General", "ShowWithoutDescription");
	changeDisplayingBlocking();
	changeDisplayingBlocked();
	changeDisplayingOffline();
	changeDisplayingWithoutDescription();
	kdebugf2();
}

void GroupsManager::tabSelected(int id)
{
	if (lastId != id) // od¶wie¿amy UserBoksa dopiero gdy grupa naprawdê siê zmieni...
	{
		lastId = id;
		if (id == 0)
			setActiveGroup(QString::null);
		else
			setActiveGroup(GroupBar->tab(id)->text());
	}
}

void GroupsManager::setActiveGroup(const QString& name)
{
	kdebugf();
	if (name == currentGroup)
		return;
	if (GroupBar->tab(GroupBar->currentTab())->text() != name)
	{
		for (int i = 0, cnt = GroupBar->count(); i < cnt; ++i)
		{
			QTab *tab = GroupBar->tabAt(i);
			if (tab->text() == name)
			{
				GroupBar->setCurrentTab(tab);
				kdebugf2();
				return;
			}
		}
	}
	if (!currentGroup.isEmpty())
		kadu->userbox()->removeFilter(group(currentGroup));
	kadu->userbox()->applyFilter(group(name));
	currentGroup = name;
	kdebugf2();
}

QString GroupsManager::currentGroupName() const
{
	if (currentGroup.isEmpty())
		return tr("All");
	else
		return currentGroup;
}

void GroupsManager::refreshTabBar()
{
	kdebugf();
	if (!GroupBar)
	{
		kdebugf2();
		return;
	}
	if (!config_file.readBoolEntry("Look", "DisplayGroupTabs"))
	{
		if (!currentGroup.isEmpty())
			setActiveGroup(QString::null);
		GroupBar->hide();
		kdebugf2();
		return;
	}

	/* budujemy listê grup */
	QStringList group_list;
	CONST_FOREACH(group, Groups)
		if ((*group)->count() != 0)
			group_list.append(group.key());

	kdebugm(KDEBUG_INFO, "%i groups found: %s\n", group_list.count(), group_list.join(",").local8Bit().data());

	/* usuwamy wszystkie niepotrzebne zakladki - od tylu,
	   bo indeksy sie przesuwaja po usunieciu */
	for (int i = GroupBar->count() - 1; i >= 1; --i)
		if (!group_list.contains(GroupBar->tabAt(i)->text()))
			GroupBar->removeTab(GroupBar->tabAt(i));

	if (group_list.isEmpty())
	{
		GroupBar->hide();
		setActiveGroup(QString::null);
		kdebugf2();
		return;
	}

	/* dodajemy nowe zakladki */
	CONST_FOREACH(group, group_list)
	{
		bool createNewTab = true;
		for (int j = 0, count = GroupBar->count(); j < count; ++j)
			if (GroupBar->tabAt(j)->text() == *group)
				createNewTab = false;
		if (createNewTab)
			GroupBar->addTab(new QTab(*group));
	}
	kdebugm(KDEBUG_INFO, "%i group tabs\n", GroupBar->count());
	GroupBar->show();

	/* odswiezamy - dziala tylko jesli jest widoczny */
	GroupBar->update();
	kdebugf2();
}

void GroupsManager::onApplyTabGeneral()
{
	kdebugf();
	if (config_file.readBoolEntry("General", "ShowBlocking") != showBlocking)
		changeDisplayingBlocking();
	if (config_file.readBoolEntry("General", "ShowBlocked") != showBlocked)
		changeDisplayingBlocked();
	if (config_file.readBoolEntry("General", "ShowOffline") != showOffline)
		changeDisplayingOffline();
	if (config_file.readBoolEntry("General", "ShowWithoutDescription") != showWithoutDescription)
		changeDisplayingWithoutDescription();
	kdebugf2();
}

void GroupsManager::changeDisplayingBlocking()
{
	kdebugf();
	showBlocking = !showBlocking;
	if (showBlocking)
		kadu->userbox()->removeNegativeFilter(blockingUsers);
	else
		kadu->userbox()->applyNegativeFilter(blockingUsers);
	config_file.writeEntry("General", "ShowBlocking", showBlocking);
	kdebugf2();
}

void GroupsManager::changeDisplayingBlocked()
{
	kdebugf();
	showBlocked = !showBlocked;
	if (showBlocked)
		kadu->userbox()->removeNegativeFilter(blockedUsers);
	else
		kadu->userbox()->applyNegativeFilter(blockedUsers);
	config_file.writeEntry("General", "ShowBlocked", showBlocked);
	kdebugf2();
}

void GroupsManager::changeDisplayingOffline()
{
	kdebugf();
	showOffline = !showOffline;
	if (showOffline)
		kadu->userbox()->removeFilter(onlineUsers);
	else
		kadu->userbox()->applyFilter(onlineUsers);
	config_file.writeEntry("General", "ShowOffline", showOffline);
	kdebugf2();
}

void GroupsManager::changeDisplayingWithoutDescription()
{
	kdebugf();
	showWithoutDescription = !showWithoutDescription;
	if (showWithoutDescription)
		kadu->userbox()->removeFilter(usersWithDescription);
	else
		kadu->userbox()->applyFilter(usersWithDescription);
	config_file.writeEntry("General", "ShowWithoutDescription", showWithoutDescription);
	kdebugf2();
}

GroupsManager::GroupsManager() : QObject(0, "groups_manager"), GroupBar(NULL)
{
	kdebugf();
	CONST_FOREACH(user, *userlist)
	{
//		kdebugm(KDEBUG_INFO, "%s\n", (*user).altNick().local8Bit().data());
		QStringList groups_ = (*user).data("Groups").toStringList();
		CONST_FOREACH(g, groups_)
			addGroup(*g)->addUser(*user);
	}
	connect (userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			 this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToMainUserlist(UserListElement, bool, bool)));
	ConfigDialog::registerSlotOnApplyTab("General", this, SLOT(onApplyTabGeneral()));

	kdebugf2();
}

GroupsManager::~GroupsManager()
{
	kdebugf();
	ConfigDialog::removeControl("General", "Show offline contacts");
	ConfigDialog::removeControl("General", "Show contacts without description");
	ConfigDialog::removeControl("General", "Show contacts that you are blocking");
	ConfigDialog::removeControl("General", "Show contacts that are blocking you");

	ConfigDialog::unregisterSlotOnApplyTab("General", this, SLOT(onApplyTabGeneral()));
	if (GroupBar)
		config_file.writeEntry("Look", "CurrentGroupTab", GroupBar->currentTab());
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToMainUserlist(UserListElement, bool, bool)));
	disconnect (userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			 this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));
	while (!Groups.isEmpty())
		removeGroup(Groups.begin().key());
	kdebugf2();
}

void GroupsManager::userAddedToMainUserlist(UserListElement elem, bool massively, bool last)
{
	kdebugf();
	QStringList groups = elem.data("Groups").toStringList();
	CONST_FOREACH(group, groups)
		addGroup(*group)->addUser(elem, true, false);
	kdebugf2();
}

UserGroup *GroupsManager::group(const QString &name) const
{
	if (name == tr("All") || name.isEmpty())
		return userlist;
	else
	{
		if (!Groups.contains(name))
		{
			kdebugm(KDEBUG_PANIC, "group %s does not exist!\n", name.local8Bit().data());
			return NULL;
		}
		else
			return Groups[name];
	}
}

QStringList GroupsManager::groups() const
{
	return QStringList(Groups.keys());
}

bool GroupsManager::groupExists(const QString &name)
{
	return Groups.contains(name);
}

UserGroup *GroupsManager::addGroup(const QString &name)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s'\n", name.local8Bit().data());
	if (Groups.contains(name))
	{
//		kdebugmf(KDEBUG_FUNCTION_END, "stop: group already exists\n");
		return Groups[name];
	}
	int cnt = userlist->count() / 2;
	if (cnt < 30)
		cnt = 31;
	UserGroup *group = new UserGroup(cnt, name.local8Bit());
	Groups.insert(name, group);

	connect(group, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));
	connect(group, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemoved(UserListElement, bool, bool)));
	refreshTabBarLater();
	kdebugf2();
	return group;
}

void GroupsManager::removeGroup(const QString &name)
{
	kdebugf();
	if (!Groups.contains(name))
	{
		kdebugf2();
		return;
	}
	UserGroup *group = this->Groups[name];

	for (int i = 1, cnt = group->count(); i <= cnt; ++i)
		group->removeUser(*(group->constBegin()), true, i == cnt);

	disconnect(group, SIGNAL(userAdded(UserListElement, bool, bool)),
				this, SLOT(userAdded(UserListElement, bool, bool)));
	disconnect(group, SIGNAL(userRemoved(UserListElement, bool, bool)),
				this, SLOT(userRemoved(UserListElement, bool, bool)));

	Groups.remove(name);
	group->deleteLater();
	refreshTabBarLater();
	kdebugf2();
}

void GroupsManager::userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last)
{
//dodanie(usuniêcie) u¿ytkownika do grupy poprzez zmianê pola Groups powinno aktualizowaæ UserGrupê
	if (name != "Groups")
		return;
	kdebugf();
	QStringList oldGroups = oldValue.toStringList();
	QStringList newGroups = currentValue.toStringList();
	CONST_FOREACH(v, oldGroups)
		if (!newGroups.contains(*v)) //usuniêcie z grupy
			group(*v)->removeUser(elem);
	CONST_FOREACH(v, newGroups)
		if (!oldGroups.contains(*v)) //dodanie grupy
		{
			if (!groupExists(*v))
				addGroup(*v);
			group(*v)->addUser(elem);
		}
	refreshTabBarLater();
	kdebugf2();
}

//dodanie(usuniêcie) u¿ytkownika do grupy poprzez addUser (removeUser) powinno modyfikowaæ pole Groups
void GroupsManager::userAdded(UserListElement elem, bool massively, bool last)
{
//	kdebugf();
	CONST_FOREACH(group, Groups)
		if (sender() == *group)
		{
			QStringList ule_groups = elem.data("Groups").toStringList();
			if (!ule_groups.contains(group.key()))
			{
				ule_groups.append(group.key());
				elem.setData("Groups", ule_groups);
			}
			break;
		}
	refreshTabBarLater();
//	kdebugf2();
}

void GroupsManager::userRemoved(UserListElement elem, bool massively, bool last)
{
//	kdebugf();
	CONST_FOREACH(group, Groups)
		if (sender() == *group)
		{
			QStringList ule_groups = elem.data("Groups").toStringList();
			if (ule_groups.contains(group.key()))
			{
				ule_groups.remove(group.key());
				elem.setData("Groups", ule_groups);
			}
			break;
		}
	refreshTabBarLater();
//	kdebugf2();
}

UsersWithDescription::UsersWithDescription() : UserGroup(userlist->count(), "users_with_description")
{
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
}

UsersWithDescription::~UsersWithDescription()
{
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
}

void UsersWithDescription::statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last)
{
	if (oldStatus.hasDescription() == elem.status(protocolName).hasDescription())
		return;
	if (oldStatus.hasDescription()) // elem.status(protocolName).hasDescription() == false
		removeUser(elem);
	else // elem.status(protocolName).hasDescription() == true
		addUser(elem);
}

OnlineUsers::OnlineUsers() : UserGroup(userlist->count(), "online_users")
{
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
}

OnlineUsers::~OnlineUsers()
{
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
}

void OnlineUsers::statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last)
{
	if ((oldStatus.isOnline() || oldStatus.isBusy()) ==
			(elem.status(protocolName).isOnline() || elem.status(protocolName).isBusy()))
		return;
	if (oldStatus.isOnline() || oldStatus.isBusy())
		removeUser(elem);
	else
		addUser(elem);
}

BlockedUsers::BlockedUsers() : UserGroup(userlist->count() / 4, "blocked_users")
{
	CONST_FOREACH(user, *userlist)
	{
//		kdebugm(KDEBUG_INFO, "%s\n", (*user).altNick().local8Bit().data());
		if ((*user).usesProtocol("Gadu") && (*user).protocolData("Gadu", "Blocking").toBool())
			addUser(*user);
	}
	connect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(protocolUserDataChangedSlot(QString, UserListElement, QString, QVariant, QVariant, bool, bool)));
}

BlockedUsers::~BlockedUsers()
{
	disconnect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(protocolUserDataChangedSlot(QString, UserListElement, QString, QVariant, QVariant, bool, bool)));
}

void BlockedUsers::protocolUserDataChangedSlot(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last)
{
	if (protocolName != "Gadu")
		return;
	if (name != "Blocking")
		return;
	if (currentValue.toBool())
		addUser(elem);
	else
		removeUser(elem);
}

BlockingUsers::BlockingUsers() : UserGroup(userlist->count() / 4, "blocking_users")
{
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
}

BlockingUsers::~BlockingUsers()
{
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
}

void BlockingUsers::statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last)
{
	if (!oldStatus.isBlocking() && elem.status(protocolName).isBlocking())
		addUser(elem);
	else if (oldStatus.isBlocking() && !elem.status(protocolName).isBlocking())
		removeUser(elem);
}

AnonymousUsers::AnonymousUsers() : UserGroup(userlist->count() / 4, "anonymous_users")
{
	connect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChangedSlot(UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));

	connect(userlist, SIGNAL(removingUser(UserListElement, bool, bool)),
			this, SLOT(removingUser(UserListElement, bool, bool)));
}

AnonymousUsers::~AnonymousUsers()
{
	disconnect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChangedSlot(UserListElement, QString, QVariant, QVariant, bool, bool)));
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));

	disconnect(userlist, SIGNAL(removingUser(UserListElement, bool, bool)),
			this, SLOT(removingUser(UserListElement, bool, bool)));
}

void AnonymousUsers::userDataChangedSlot(UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last)
{
	if (name != "Anonymous")
		return;
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", elem.ID("Gadu").local8Bit().data(), currentValue.toBool(), contains(elem));
	if (currentValue.toBool())
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsers::userAdded(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", elem.ID("Gadu").local8Bit().data(), elem.isAnonymous(), contains(elem));
	if (elem.isAnonymous())
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsers::removingUser(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", elem.ID("Gadu").local8Bit().data(), elem.isAnonymous(), contains(elem));
	if (!contains(elem))
		addUser(elem, massively, last);
}


BlockedUsers *blockedUsers;
BlockingUsers *blockingUsers;
UsersWithDescription *usersWithDescription;
OnlineUsers *onlineUsers;
AnonymousUsers *anonymousUsers;
GroupsManager *groups_manager = NULL;
