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
#include "icons_manager.h"
#include "kadu.h"
#include "pending_msgs.h"
#include "tabbar.h"
#include "userbox.h"

#include "groups_manager.h"

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
	groups_manager = 0;

	UserBox *userbox = kadu->userbox();
	userbox->removeNegativeFilter(blockingUsers);
	userbox->removeNegativeFilter(blockedUsers);
	userbox->removeFilter(onlineAndDescriptionUsers);
	userbox->removeNegativeFilter(offlineUsers);
	userbox->removeFilter(usersWithDescription);
	userbox->removeNegativeFilter(anonymousUsers);
	userbox->removeNegativeFilter(anonymousUsersWithoutMessages);

	delete usersWithDescription;
	usersWithDescription = 0;

	delete onlineUsers;
	onlineUsers = 0;

	delete onlineAndDescriptionUsers;
	onlineAndDescriptionUsers = 0;

	delete offlineUsers;
	offlineUsers = 0;

	delete blockedUsers;
	blockedUsers = 0;

	delete blockingUsers;
	blockingUsers = 0;

	delete anonymousUsers;
	anonymousUsers = 0;

	delete anonymousUsersWithoutMessages;
	anonymousUsersWithoutMessages = 0;

	kdebugf2();
}

void GroupsManager::setTabBar(KaduTabBar *bar)
{
	kdebugf();
	GroupBar = bar;

	bar->setShape(QTabBar::RoundedBelow);
	bar->addTab(icons_manager->loadIcon("PersonalInfo"), tr("All"));
	bar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(), QFont::Bold));
	bar->setIconSize(QSize(16,16));
	connect(bar, SIGNAL(selected(int)), this, SLOT(tabSelected(int)));
	connect(userlist, SIGNAL(modified()), this, SLOT(refreshTabBarLater()));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTabBar()));
	lastId = -1;

	refreshTabBar();

	int configTab = config_file.readNumEntry( "Look", "CurrentGroupTab" );
	if (configTab >= 0 && configTab < GroupBar->count())
		((QTabBar*) GroupBar)->setCurrentTab(configTab);

	//najpierw ustawiamy odwrotnie, a p��niej robimy x=!x;
	// TODO: g�wniana metoda, poprawi�
	showBlocked = !config_file.readBoolEntry("General", "ShowBlocked");
	showBlocking = !config_file.readBoolEntry("General", "ShowBlocking");
	showOffline = !config_file.readBoolEntry("General", "ShowOffline");
	showWithoutDescription = !config_file.readBoolEntry("General", "ShowWithoutDescription");
	showOnlineAndDescription = !config_file.readBoolEntry("General", "ShowOnlineAndDescription");
	changeDisplayingBlocking();
	changeDisplayingBlocked();
//	changeDisplayingOffline();
// 	changeDisplayingWithoutDescription();
//	changeDisplayingOnlineAndDescription();
	kdebugf2();
}

void GroupsManager::tabSelected(int id)
{
	if (lastId != id) // od�wie�amy UserBoksa dopiero gdy grupa naprawd� si� zmieni...
	{
		lastId = id;
		if (id == 0)
			setActiveGroup(QString::null);
		else
			setActiveGroup(GroupBar->tabText(id));
	}
}

void GroupsManager::setActiveGroup(const QString &name)
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

	/* budujemy list� grup */
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
	foreach(QString group, group_list)
	{
		bool createNewTab = true;

		for (int j = 0, count = GroupBar->count(); j < count; ++j)
		{
			if (GroupBar->tabText(j) == group)
			{
				createNewTab = false;
				break;
			}
		}

		if (createNewTab)
		{
			QString iconPath = config_file.readEntry("GroupIcon", group);
			if (!iconPath.isEmpty()) 
				GroupBar->addTab(icons_manager->loadIcon(iconPath), group);
			else 
				GroupBar->addTab(group);
		}
	}

	kdebugm(KDEBUG_INFO, "%i group tabs\n", GroupBar->count());
	GroupBar->show();

	kdebugf2();
}

void GroupsManager::configurationUpdated()
{
	if (config_file.readBoolEntry("General", "ShowBlocking") != showBlocking)
		changeDisplayingBlocking();
	if (config_file.readBoolEntry("General", "ShowBlocked") != showBlocked)
		changeDisplayingBlocked();
//	if (config_file.readBoolEntry("General", "ShowOffline") != showOffline)
//		changeDisplayingOffline();
// 	if (config_file.readBoolEntry("General", "ShowWithoutDescription") != showWithoutDescription)
// 		changeDisplayingWithoutDescription();
}

void GroupsManager::iconThemeChanged()
{
	if (GroupBar)
		GroupBar->setTabIcon(0, icons_manager->loadIcon("PersonalInfo"));
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

void GroupsManager::changeDisplayingOffline(UserBox *userBox, bool show)
{
	kdebugf();
// 	if (KaduActions["inactiveUsersAction"])
// 		KaduActions["inactiveUsersAction"]->setAllOn(showOffline);
 	showOffline = show;
 	if (showOffline)
 		userBox->removeNegativeFilter(offlineUsers);
 	else
 		userBox->applyNegativeFilter(offlineUsers);
 	config_file.writeEntry("General", "ShowOffline", showOffline);
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

void GroupsManager::changeDisplayingOnlineAndDescription(UserBox *userBox, bool show)
{
	kdebugf();

	showOnlineAndDescription = show;
/*
	if (KaduActions["onlineAndDescriptionUsersAction"])
		KaduActions["onlineAndDescriptionUsersAction"]->setAllOn(showOnlineAndDescription);
*/
	if (!showOnlineAndDescription)
		userBox->applyFilter(onlineAndDescriptionUsers);
	else
		userBox->removeFilter(onlineAndDescriptionUsers);

	config_file.writeEntry("General", "ShowOnlineAndDescription", showOnlineAndDescription);

	kdebugf2();
}

GroupsManager::GroupsManager()
	: QObject(), Groups(), GroupBar(0), lastId(-1), currentGroup(), showBlocked(true),
	showBlocking(true), showOffline(true), showWithoutDescription(true), refreshTimer()
{
	kdebugf();
	foreach(UserListElement user, *userlist)
	{
//		kdebugm(KDEBUG_INFO, "%s\n", (*user).altNick().local8Bit().data());
		QStringList groups_ = user.data("Groups").toStringList();
		foreach(const QString &g, groups_)
			addGroup(g)->addUser(user);
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
	foreach(const QString &group, groups)
		addGroup(group)->addUser(elem, true, false);
	kdebugf2();
}

void GroupsManager::userRemovedFromMainUserlist(UserListElement elem, bool /*massively*/, bool /*last*/)
{
	kdebugf();
	QStringList groups = elem.data("Groups").toStringList();
	foreach(const QString &group, groups)
		addGroup(group)->removeUser(elem, true, false);
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
			return 0;
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
	UserGroup *group = new UserGroup();
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
		group->removeUser(*group->constBegin(), true, i == cnt);

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
//dodanie(usuni�cie) u�ytkownika do grupy poprzez zmian� pola Groups powinno aktualizowa� UserGrup�
	if (name != "Groups")
		return;
	kdebugf();
	QStringList oldGroups = oldValue.toStringList();
	QStringList newGroups = currentValue.toStringList();
	foreach(const QString &v, oldGroups)
	{
		if (!newGroups.contains(v)) //usuni�cie z grupy
			group(v)->removeUser(elem);

		if (group(v)->count() == 0)
		{
			removeGroup(v);
			config_file.removeVariable("GroupIcon", (v));
		}
	}

	foreach(const QString &v, newGroups)
		if (!oldGroups.contains(v)) //dodanie grupy
		{
			if (!groupExists(v))
				addGroup(v);
			group(v)->addUser(elem);
		}
	refreshTabBarLater();
	kdebugf2();
}

//dodanie(usuni�cie) u�ytkownika do grupy poprzez addUser (removeUser) powinno modyfikowa� pole Groups
void GroupsManager::userAdded(UserListElement elem, bool /*massively*/, bool /*last*/)
{
//	kdebugf();
	foreach(const QString &key, Groups.keys())
		if (sender() == Groups[key])
		{
			QStringList ule_groups = elem.data("Groups").toStringList();
			if (!ule_groups.contains(key))
			{
				ule_groups.append(key);
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
	foreach(const QString &key, Groups.keys())
		if (sender() == Groups[key])
		{
			QStringList ule_groups = elem.data("Groups").toStringList();
			if (ule_groups.contains(key))
			{
				ule_groups.remove(key);
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
			QString iconPath = config_file.readEntry("GroupIcon", name);	

			if (!iconPath.isEmpty()) 
				GroupBar->setTabIcon(index, icons_manager->loadIcon(iconPath));
			else 
				GroupBar->setTabIcon(index, QIcon());

			return;
		}
	}
}

UsersWithDescription::UsersWithDescription() : UserGroup()
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

OnlineUsers::OnlineUsers() : UserGroup()
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

OnlineAndDescriptionUsers::OnlineAndDescriptionUsers() : UserGroup()
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
		foreach(const QString &proto, protos)
		{
			UserStatus status = elem.status(proto);
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

OfflineUsers::OfflineUsers() : UserGroup()
{
	foreach(const UserListElement user, *userlist)
	{
		bool offline = true;

		QStringList protos = user.protocolList();
		if (!protos.empty()) // user uses any protocol? let's check them all...
		{
			foreach(const QString &proto, protos)
			{
				if (!user.status(proto).isOffline())
				{
					offline = false; // if online in at LEAST one proto
					break;
				}
			}
		}
		else // doesn't use any proto -> so can't be offline
			offline = false;

		if (offline)
			addUser(user);
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
		foreach(const QString &proto, protos)
		{
			if (!elem.status(proto).isOffline())
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

BlockedUsers::BlockedUsers() : UserGroup()
{
	foreach(UserListElement user, *userlist)
	{
//		kdebugm(KDEBUG_INFO, "%s\n", (*user).altNick().local8Bit().data());
		if (user.usesProtocol("Gadu") && user.protocolData("Gadu", "Blocking").toBool())
			addUser(user);
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

BlockingUsers::BlockingUsers() : UserGroup()
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

AnonymousUsers::AnonymousUsers() : UserGroup()
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

AnonymousUsersWithoutMessages::AnonymousUsersWithoutMessages() : UserGroup()
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
GroupsManager *groups_manager = 0;
