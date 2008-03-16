/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "debug.h"
#include "groups_manager.h"
#include "action.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "pending_msgs.h"
#include "tabbar.h"
#include "userbox.h"
#include "userlist.h"
//Added by qt3to4:
#include <QPixmap>

void GroupsManager::initModule()
{
	kdebugf();
	groups_manager = new GroupsManager();
	usersWithDescription = new UsersWithDescription();
	onlineUsers = new OnlineUsers();
	onlineAndDescriptionUsers = new OnlineAndDescriptionUsers();
	offlineUsers = new OfflineUsers();
	blockedUsers = new BlockedUsers();
	blockingUsers = new BlockingUsers();
	anonymousUsers = new AnonymousUsers();
	anonymousUsersWithoutMessages = new AnonymousUsersWithoutMessages();
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
	userbox->removeFilter(onlineAndDescriptionUsers);
	userbox->removeNegativeFilter(offlineUsers);
	userbox->removeFilter(usersWithDescription);
	userbox->removeNegativeFilter(anonymousUsers);
	userbox->removeNegativeFilter(anonymousUsersWithoutMessages);

	delete usersWithDescription;
	usersWithDescription = NULL;

	delete onlineUsers;
	onlineUsers = NULL;

	delete onlineAndDescriptionUsers;
	onlineAndDescriptionUsers = NULL;

	delete offlineUsers;
	offlineUsers = NULL;

	delete blockedUsers;
	blockedUsers = NULL;

	delete blockingUsers;
	blockingUsers = NULL;

	delete anonymousUsers;
	anonymousUsers = NULL;

	delete anonymousUsersWithoutMessages;
	anonymousUsersWithoutMessages = NULL;

	kdebugf2();
}

void GroupsManager::setTabBar(KaduTabBar *bar)
{
	kdebugf();
	GroupBar = bar;

	bar->setShape(QTabBar::RoundedBelow);
	int index = bar->addTab(tr("All"));
	bar->setTabIcon(index, icons_manager->loadPixmap("PersonalInfo").xForm(QMatrix().rotate(-90)));
	bar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(), QFont::Bold));
	connect(bar, SIGNAL(selected(int)), this, SLOT(tabSelected(int)));
	connect(userlist, SIGNAL(modified()), this, SLOT(refreshTabBarLater()));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTabBar()));
	lastId = -1;

	refreshTabBar();

	int configTab = config_file.readNumEntry( "Look", "CurrentGroupTab" );
	if (configTab >= 0 && configTab < GroupBar->count())
		((QTabBar*) GroupBar)->setCurrentTab(configTab);

	//najpierw ustawiamy odwrotnie, a pó¼niej robimy x=!x;
	// TODO: gówniana metoda, poprawiæ
	showBlocked = !config_file.readBoolEntry("General", "ShowBlocked");
	showBlocking = !config_file.readBoolEntry("General", "ShowBlocking");
	showOffline = !config_file.readBoolEntry("General", "ShowOffline");
	showWithoutDescription = !config_file.readBoolEntry("General", "ShowWithoutDescription");
	showOnlineAndDescription = !config_file.readBoolEntry("General", "ShowOnlineAndDescription");
	changeDisplayingBlocking();
	changeDisplayingBlocked();
	changeDisplayingOffline();
// 	changeDisplayingWithoutDescription();
	changeDisplayingOnlineAndDescription();
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
			setActiveGroup(GroupBar->tabText(id));
	}
}

void GroupsManager::setActiveGroup(const QString& name)
{
	kdebugf();
	if (name == currentGroup)
		return;
	if (GroupBar->tabText(GroupBar->currentIndex()) != name)
	{
		for (int i = 0, cnt = GroupBar->count(); i < cnt; ++i)
		{
			QString tab = GroupBar->tabText(i);
			if (tab == name)
			{
				GroupBar->setCurrentIndex(i);
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
	if (currentGroup.isEmpty() || group(currentGroup)->count() == 0)
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
	QStringList group_list = groups();
	kdebugm(KDEBUG_INFO, "%u groups found: %s\n", group_list.count(), group_list.join(",").local8Bit().data());

	/* usuwamy wszystkie niepotrzebne zakladki - od tylu,
	   bo indeksy sie przesuwaja po usunieciu */
	for (int i = GroupBar->count() - 1; i >= 1; --i)
		if (!group_list.contains(GroupBar->tabText(i)))
			GroupBar->removeTab(i);

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
		{
			if (GroupBar->tabText(j) == (*group))
			{
				createNewTab = false;
				break;
			}
		}

		if (createNewTab)
		{
			QString iconPath = config_file.readEntry("GroupIcon", (*group));	
			if (!iconPath.isEmpty()) 
			{
				QPixmap icon = icons_manager->loadPixmap(iconPath).xForm(QMatrix().rotate(-90));

				int index = GroupBar->addTab(*group);
				GroupBar->setTabIcon(index, icon.xForm(QMatrix().scale((double)16/icon.width(), (double)16/icon.height())));
			}
			else 
				GroupBar->addTab(*group);
		}
	}

	kdebugm(KDEBUG_INFO, "%i group tabs\n", GroupBar->count());
	GroupBar->show();

	/* odswiezamy - dziala tylko jesli jest widoczny */
	GroupBar->update();
	kdebugf2();
}

void GroupsManager::configurationUpdated()
{
	if (config_file.readBoolEntry("General", "ShowBlocking") != showBlocking)
		changeDisplayingBlocking();
	if (config_file.readBoolEntry("General", "ShowBlocked") != showBlocked)
		changeDisplayingBlocked();
	if (config_file.readBoolEntry("General", "ShowOffline") != showOffline)
		changeDisplayingOffline();
// 	if (config_file.readBoolEntry("General", "ShowWithoutDescription") != showWithoutDescription)
// 		changeDisplayingWithoutDescription();
}

void GroupsManager::iconThemeChanged()
{
	if (GroupBar)
		GroupBar->setTabIcon(0, icons_manager->loadPixmap("PersonalInfo").xForm(QMatrix().rotate(-90)));
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
// 	if (KaduActions["inactiveUsersAction"])
// 		KaduActions["inactiveUsersAction"]->setAllOn(showOffline);
// 	showOffline = !showOffline;
// 	if (showOffline)
// 		kadu->userbox()->removeNegativeFilter(offlineUsers);
// 	else
// 		kadu->userbox()->applyNegativeFilter(offlineUsers);
// 	config_file.writeEntry("General", "ShowOffline", showOffline);
	kdebugf2();
}

void GroupsManager::changeDisplayingWithoutDescription(UserBox *userBox, bool show)
{
	kdebugf();
	showWithoutDescription = show;
	if (showWithoutDescription)
		userBox->removeFilter(usersWithDescription);
	else
		userBox->applyFilter(usersWithDescription);
	config_file.writeEntry("General", "ShowWithoutDescription", showWithoutDescription);
	kdebugf2();
}

void GroupsManager::changeDisplayingOnlineAndDescription()
{
	kdebugf();

	showOnlineAndDescription = !showOnlineAndDescription;
/*
	if (KaduActions["onlineAndDescriptionUsersAction"])
		KaduActions["onlineAndDescriptionUsersAction"]->setAllOn(showOnlineAndDescription);

	if (showOnlineAndDescription)
		kadu->userbox()->applyFilter(onlineAndDescriptionUsers);
	else
		kadu->userbox()->removeFilter(onlineAndDescriptionUsers);

	config_file.writeEntry("General", "ShowOnlineAndDescription", showOnlineAndDescription);*/

	kdebugf2();
}

GroupsManager::GroupsManager() : QObject(0, "groups_manager"),
		Groups(), GroupBar(0), lastId(-1), currentGroup(), showBlocked(true),
		showBlocking(true), showOffline(true), showWithoutDescription(true), refreshTimer()
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
	connect(userlist, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromMainUserlist(UserListElement, bool, bool)));
	connect(icons_manager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));

	kdebugf2();
}

GroupsManager::~GroupsManager()
{
	kdebugf();

	if (GroupBar)
		config_file.writeEntry("Look", "CurrentGroupTab", GroupBar->currentIndex());

	disconnect(icons_manager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
	disconnect(userlist, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromMainUserlist(UserListElement, bool, bool)));
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToMainUserlist(UserListElement, bool, bool)));
	disconnect (userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			 this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));

	while (!Groups.isEmpty())
		removeGroup(Groups.begin().key());
	kdebugf2();
}

void GroupsManager::userAddedToMainUserlist(UserListElement elem, bool /*massively*/, bool /*last*/)
{
	kdebugf();
	QStringList groups = elem.data("Groups").toStringList();
	CONST_FOREACH(group, groups)
		addGroup(*group)->addUser(elem, true, false);
	kdebugf2();
}

void GroupsManager::userRemovedFromMainUserlist(UserListElement elem, bool /*massively*/, bool /*last*/)
{
	kdebugf();
	QStringList groups = elem.data("Groups").toStringList();
	CONST_FOREACH(grp, groups)
		addGroup(*grp)->removeUser(elem, true, false);
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

	setActiveGroup(currentGroupName());
	Groups.remove(name);
	group->deleteLater();
	refreshTabBarLater();
	kdebugf2();
}

void GroupsManager::userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool /*massively*/, bool /*last*/)
{
//dodanie(usuniêcie) u¿ytkownika do grupy poprzez zmianê pola Groups powinno aktualizowaæ UserGrupê
	if (name != "Groups")
		return;
	kdebugf();
	QStringList oldGroups = oldValue.toStringList();
	QStringList newGroups = currentValue.toStringList();
	CONST_FOREACH(v, oldGroups)
	{
		if (!newGroups.contains(*v)) //usuniêcie z grupy
			group(*v)->removeUser(elem);

		if (group(*v)->count() == 0)
		{
			removeGroup(*v);
			config_file.removeVariable("GroupIcon", (*v));
		}
	}

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
void GroupsManager::userAdded(UserListElement elem, bool /*massively*/, bool /*last*/)
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

void GroupsManager::userRemoved(UserListElement elem, bool /*massively*/, bool /*last*/)
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

void GroupsManager::setIconForTab(const QString &name)
{
	if (!GroupBar || !config_file.readBoolEntry("Look", "DisplayGroupTabs") || !Groups.contains(name))
	{
		kdebugf2();
		return;
	}

	for (int index = 0, count = GroupBar->count(); index < count; ++index)
	{
		if (GroupBar->tabText(index) == name)
		{
			int currentTab = GroupBar->currentIndex();

			GroupBar->removeTab(index);

			QString iconPath = config_file.readEntry("GroupIcon", name);	

			if (!iconPath.isEmpty()) 
			{
				QPixmap icon = icons_manager->loadPixmap(iconPath).xForm(QMatrix().rotate(-90));

				GroupBar->insertTab(index, name);
				GroupBar->setTabIcon(index, icon.xForm(QMatrix().scale((double)16/icon.width(), (double)16/icon.height())));
			}
			else 
				GroupBar->insertTab(index, name);

			GroupBar->update();

			GroupBar->setCurrentIndex(currentTab);
			return;
		}
	}
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
							const UserStatus &oldStatus, bool /*massively*/, bool /*last*/)
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
							const UserStatus &oldStatus, bool /*massively*/, bool /*last*/)
{
	if (oldStatus.isOffline() == elem.status(protocolName).isOffline())
		return;
	if (oldStatus.isOffline())
		addUser(elem);
	else
		removeUser(elem);
}

OnlineAndDescriptionUsers::OnlineAndDescriptionUsers() : UserGroup(userlist->count(), "online_and_description_users")
{
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userChangedSlot(UserListElement, bool, bool)));
	connect(userlist, SIGNAL(protocolAdded(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
	connect(userlist, SIGNAL(removingProtocol(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
}

OnlineAndDescriptionUsers::~OnlineAndDescriptionUsers()
{
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userChangedSlot(UserListElement, bool, bool)));
	disconnect(userlist, SIGNAL(protocolAdded(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
	disconnect(userlist, SIGNAL(removingProtocol(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
}

void OnlineAndDescriptionUsers::userChangedSlot(UserListElement elem, bool /*massively*/, bool /*last*/)
{
	bool isWanted = false; // czy chcemy zeby uzytkownik byl wyswietlany na liscie

	QStringList protos = elem.protocolList();
	if (!protos.isEmpty())
	{
		CONST_FOREACH(proto, protos)
		{
			UserStatus status = elem.status(*proto);
			if (status.isOnline() || status.isBusy() || status.hasDescription())
			{
				isWanted = true;
				break;
			}
		}
	}

	if (isWanted)
		addUser(elem);
	else
		removeUser(elem);
}

void OnlineAndDescriptionUsers::statusChangedSlot(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last)
{
	userChangedSlot(elem, massively, last);
}

void OnlineAndDescriptionUsers::protocolAddedOrRemoved(UserListElement elem, QString protocolName, bool massively, bool last)
{
	userChangedSlot(elem, massively, last);
}

OfflineUsers::OfflineUsers() : UserGroup(userlist->count(), "offline_users")
{
	CONST_FOREACH(user, *userlist)
	{
		bool offline = true;

		QStringList protos = (*user).protocolList();
		if (!protos.empty()) // user uses any protocol? let's check them all...
		{
			CONST_FOREACH(proto, protos)
			{
				if (!(*user).status(*proto).isOffline())
				{
					offline = false; // if online in at LEAST one proto
					break;
				}
			}
		}
		else // doesn't use any proto -> so can't be offline
			offline = false;

		if (offline)
			addUser(*user);
	}

	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userChangedSlot(UserListElement, bool, bool)));
	connect(userlist, SIGNAL(protocolAdded(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
	connect(userlist, SIGNAL(removingProtocol(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
}

OfflineUsers::~OfflineUsers()
{
	disconnect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(statusChangedSlot(UserListElement, QString, const UserStatus &, bool, bool)));
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userChangedSlot(UserListElement, bool, bool)));
	disconnect(userlist, SIGNAL(protocolAdded(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
	disconnect(userlist, SIGNAL(removingProtocol(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAddedOrRemoved(UserListElement, QString, bool, bool)));
}



void OfflineUsers::userChangedSlot(UserListElement elem, bool /*massively*/, bool /*last*/)
{
	bool offline = true;

	QStringList protos = elem.protocolList();
	if (!protos.empty()) // if elem uses any protocol, we check status in all of them...
	{
		CONST_FOREACH(proto, protos)
		{
			if (!elem.status(*proto).isOffline())
			{
				offline = false;
				break;
			}
		}
	}
	else // elem doesn't use any protocol, so it can't be offline :)
		offline = false;

	if (offline)
		addUser(elem);
	else
		removeUser(elem);
}

void OfflineUsers::statusChangedSlot(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last)
{
	userChangedSlot(elem, massively, last);
}

void OfflineUsers::protocolAddedOrRemoved(UserListElement elem, QString protocolName, bool massively, bool last)
{
	userChangedSlot(elem, massively, last);
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
							QString name, QVariant /*oldValue*/, QVariant currentValue,
							bool /*massively*/, bool /*last*/)
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
							const UserStatus &oldStatus, bool /*massively*/, bool /*last*/)
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

AnonymousUsersWithoutMessages::AnonymousUsersWithoutMessages() : UserGroup(userlist->count() / 4, "anonymous_users_without_messages")
{
	connect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChangedSlot(UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));

	connect(userlist, SIGNAL(removingUser(UserListElement, bool, bool)),
			this, SLOT(removingUser(UserListElement, bool, bool)));

	connect(&pending, SIGNAL(messageFromUserAdded(UserListElement)),
			this, SLOT(messageFromUserAdded(UserListElement)));
	connect(&pending, SIGNAL(messageFromUserDeleted(UserListElement)),
			this, SLOT(messageFromUserDeleted(UserListElement)));
}

AnonymousUsersWithoutMessages::~AnonymousUsersWithoutMessages()
{
	disconnect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChangedSlot(UserListElement, QString, QVariant, QVariant, bool, bool)));
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));

	disconnect(userlist, SIGNAL(removingUser(UserListElement, bool, bool)),
			this, SLOT(removingUser(UserListElement, bool, bool)));

	disconnect(&pending, SIGNAL(messageFromUserAdded(UserListElement)),
			this, SLOT(messageFromUserAdded(UserListElement)));
	disconnect(&pending, SIGNAL(messageFromUserDeleted(UserListElement)),
			this, SLOT(messageFromUserDeleted(UserListElement)));
}

static inline bool withoutMessages(const UserListElement &e)
{
	return !pending.pendingMsgs(e);
}

void AnonymousUsersWithoutMessages::userDataChangedSlot(UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last)
{
	if (name != "Anonymous")
		return;
//	kdebugmf(KDEBUG_WARNING, "%s %d %d %d\n", elem.ID("Gadu").local8Bit().data(), currentValue.toBool(), contains(elem), withoutMessages(elem));

	if (currentValue.toBool() && withoutMessages(elem))
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsersWithoutMessages::userAdded(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", elem.ID("Gadu").local8Bit().data(), elem.isAnonymous(), contains(elem));

	if (elem.isAnonymous() && withoutMessages(elem))
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsersWithoutMessages::removingUser(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_WARNING, "%s %d %d %d\n", elem.ID("Gadu").local8Bit().data(), elem.isAnonymous(), contains(elem), withoutMessages(elem));

	if (withoutMessages(elem))
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsersWithoutMessages::messageFromUserAdded(UserListElement elem)
{
	removeUser(elem);
}

void AnonymousUsersWithoutMessages::messageFromUserDeleted(UserListElement elem)
{
	if (elem.isAnonymous() && withoutMessages(elem))
		addUser(elem);
}

BlockedUsers *blockedUsers;
BlockingUsers *blockingUsers;
UsersWithDescription *usersWithDescription;
OnlineUsers *onlineUsers;
OnlineAndDescriptionUsers *onlineAndDescriptionUsers;
OfflineUsers *offlineUsers;
AnonymousUsers *anonymousUsers;
AnonymousUsersWithoutMessages *anonymousUsersWithoutMessages;
GroupsManager *groups_manager = NULL;
