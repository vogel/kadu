/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account_manager.h"
#include "config_file.h"
#include "contacts/contact.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "pending_msgs.h"
#include "userbox.h"

#include "groups_manager.h"

void GroupsManagerOld::initModule()
{
	kdebugf();
	groups_manager = new GroupsManagerOld();
	onlineUsers = new OnlineUsers();
	onlineAndDescriptionUsers = new OnlineAndDescriptionUsers();
	offlineUsers = new OfflineUsers();
	blockedUsers = new BlockedUsers();
	blockingUsers = new BlockingUsers();
	anonymousUsers = new AnonymousUsers();
	anonymousUsersWithoutMessages = new AnonymousUsersWithoutMessages();
	kdebugf2();
}

void GroupsManagerOld::closeModule()
{
	kdebugf();
	delete groups_manager;
	groups_manager = 0;

// 	UserBox *userbox = kadu->userbox();
/*
	userbox->removeNegativeFilter(blockingUsers);
	userbox->removeNegativeFilter(blockedUsers);
	userbox->removeFilter(onlineAndDescriptionUsers);
	userbox->removeNegativeFilter(offlineUsers);
	userbox->removeNegativeFilter(anonymousUsers);
	userbox->removeNegativeFilter(anonymousUsersWithoutMessages);
*/
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

QString GroupsManagerOld::currentGroupName() const
{
	if (currentGroup.isEmpty() || group(currentGroup)->count() == 0)
		return tr("All");
	else
		return currentGroup;
}

void GroupsManagerOld::configurationUpdated()
{/*
	if (config_file.readBoolEntry("General", "ShowBlocking") != showBlocking)
		changeDisplayingBlocking(!showBlocking);
	if (config_file.readBoolEntry("General", "ShowBlocked") != showBlocked)
		changeDisplayingBlocked(!showBlocked);*/
// 	if (config_file.readBoolEntry("General", "ShowOffline") != showOffline)
// 		changeDisplayingOffline(kadu->userbox(), !showOffline);
//  	if (config_file.readBoolEntry("General", "ShowWithoutDescription") != showWithoutDescription)
//  		changeDisplayingWithoutDescription(kadu->userbox(), !showWithoutDescription);
}

// void GroupsManagerOld::changeDisplayingBlocking(bool show)
// {
// 	kdebugf();
// 	showBlocking = show;
//	if (showBlocking)
//		kadu->userbox()->removeNegativeFilter(blockingUsers);
//	else
//		kadu->userbox()->applyNegativeFilter(blockingUsers);
// 	config_file.writeEntry("General", "ShowBlocking", showBlocking);
// 	kdebugf2();
// }

// void GroupsManagerOld::changeDisplayingBlocked(bool show)
// {
// 	kdebugf();
// 	showBlocked = show;
//	if (showBlocked)
//		kadu->userbox()->removeNegativeFilter(blockedUsers);
//	else
// //		kadu->userbox()->applyNegativeFilter(blockedUsers);
// 	config_file.writeEntry("General", "ShowBlocked", showBlocked);
// 	kdebugf2();
// }

// void GroupsManager::changeDisplayingOffline(UserBox *userBox, bool show)
// {
// 	kdebugf();
// 
//  	showOffline = show;
// 
// 	if (KaduActions["inactiveUsersAction"])
// 	{
// 		KaduAction *action = KaduActions["inactiveUsersAction"]->action(userBox->mainWindow());
// 		if (action)
// 			action->setChecked(!showOffline);
// 	}

//	if (showOffline)
//		userBox->removeNegativeFilter(offlineUsers);
//	else
//		userBox->applyNegativeFilter(offlineUsers);
// 	config_file.writeEntry("General", "ShowOffline", showOffline);
// 	kdebugf2();
// }

// void GroupsManager::changeDisplayingWithoutDescription(UserBox *userBox, bool show)
// {
// 	kdebugf();
// 
// 	showWithoutDescription = show;
// 
// 	if (KaduActions["descriptionUsersAction"])
// 	{
// 		KaduAction *action = KaduActions["descriptionUsersAction"]->action(userBox->mainWindow());
// 		if (action)
// 			action->setChecked(!showWithoutDescription);
// 	}

// }

// void GroupsManager::changeDisplayingOnlineAndDescription(UserBox *userBox, bool show)
// {
// 	kdebugf();
// 
// 	showOnlineAndDescription = show;
// 
// 	if (KaduActions["onlineAndDescriptionUsersAction"])
// 	{
// 		KaduAction *action = KaduActions["onlineAndDescriptionUsersAction"]->action(userBox->mainWindow());
// 		if (action)
// 			action->setChecked(showOnlineAndDescription);
// 	}

// 	if (showOnlineAndDescription)
// 		userBox->applyFilter(onlineAndDescriptionUsers);
// 	else
// 		userBox->removeFilter(onlineAndDescriptionUsers);
/*
	config_file.writeEntry("General", "ShowOnlineAndDescription", showOnlineAndDescription);

	kdebugf2();
}*/

GroupsManagerOld::GroupsManagerOld()
	: QObject(), Groups(), lastId(-1), currentGroup(), showBlocked(true),
	showBlocking(true), showOffline(true), showWithoutDescription(true), refreshTimer()
{
	kdebugf();
	foreach(UserListElement user, *userlist)
	{
//		kdebugm(KDEBUG_INFO, "%s\n", qPrintable((*user).altNick()));
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

GroupsManagerOld::~GroupsManagerOld()
{
	kdebugf();

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


UserGroup *GroupsManagerOld::group(const QString &name) const
{
	if (name == tr("All") || name.isEmpty())
		return userlist;
	else
	{
		if (!Groups.contains(name))
		{
			kdebugm(KDEBUG_PANIC, "group %s does not exist!\n", qPrintable(name));
			return 0;
		}
		else
			return Groups[name];
	}
}

QStringList GroupsManagerOld::groups() const
{
	return QStringList(Groups.keys());
}

bool GroupsManagerOld::groupExists(const QString &name)
{
	return Groups.contains(name);
}

UserGroup *GroupsManagerOld::addGroup(const QString &name)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s'\n", qPrintable(name));
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
// 	refreshTabBarLater();
	kdebugf2();
	return group;
}

void GroupsManagerOld::removeGroup(const QString &name)
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

// 	setActiveGroup(currentGroupName());
	Groups.remove(name);
	group->deleteLater();
// 	refreshTabBarLater();
	kdebugf2();
}

// void GroupsManagerOld::userDataChanged(UserListElement elem, QString name, QVariant oldValue,
// 							QVariant currentValue, bool /*massively*/, bool /*last*/)
// {
//dodanie(usuni�cie) u�ytkownika do grupy poprzez zmian� pola Groups powinno aktualizowa� UserGrup�
// 	if (name != "Groups")
// 		return;
// 	kdebugf();
// 	QStringList oldGroups = oldValue.toStringList();
// 	QStringList newGroups = currentValue.toStringList();
// 	foreach(const QString &v, oldGroups)
// 	{
// 		if (!newGroups.contains(v)) //usuni�cie z grupy
// 			group(v)->removeUser(elem);
// 
// 		if (group(v)->count() == 0)
// 		{
// 			removeGroup(v);
// 			config_file.removeVariable("GroupIcon", (v));
// 		}
// 	}
// 
// 	foreach(const QString &v, newGroups)
// 		if (!oldGroups.contains(v)) //dodanie grupy
// 		{
// 			if (!groupExists(v))
// 				addGroup(v);
// 			group(v)->addUser(elem);
// 		}
// 	refreshTabBarLater();
// 	kdebugf2();
// }
// 
//dodanie(usuni�cie) u�ytkownika do grupy poprzez addUser (removeUser) powinno modyfikowa� pole Groups
// void GroupsManagerOld::userAdded(UserListElement elem, bool /*massively*/, bool /*last*/)
// {
//	kdebugf();
// 	foreach(const QString &key, Groups.keys())
// 		if (sender() == Groups[key])
// 		{
// 			QStringList ule_groups = elem.data("Groups").toStringList();
// 			if (!ule_groups.contains(key))
// 			{
// 				ule_groups.append(key);
// 				elem.setData("Groups", ule_groups);
// 			}
// 			break;
// 		}
// 	refreshTabBarLater();
//	kdebugf2();
// }

// void GroupsManagerOld::userRemoved(UserListElement elem, bool /*massively*/, bool /*last*/)
// {
//	kdebugf();
// 	foreach(const QString &key, Groups.keys())
// 		if (sender() == Groups[key])
// 		{
// 			QStringList ule_groups = elem.data("Groups").toStringList();
// 			if (ule_groups.contains(key))
// 			{
// 				ule_groups.remove(key);
// 				elem.setData("Groups", ule_groups);
// 			}
// 			break;
// 		}
// 	refreshTabBarLater();
//	kdebugf2();
// }

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
// TODO: 0.6.6
// 	if (oldStatus.isOffline() == elem.status(protocolName).isOffline())
// 		return;
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
// TODO: 0.6.6
// 			UserStatus status = elem.status(proto);
// 			if (status.isOnline() || status.isBusy() || status.hasDescription())
// 			{
// 				isWanted = true;
// 				break;
// 			}
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
	foreach(const UserListElement &user, *userlist)
	{
		bool offline = true;

		QStringList protos = user.protocolList();
		if (!protos.empty()) // user uses any protocol? let's check them all...
		{
			foreach(const QString &proto, protos)
			{
// TODO: 0.6.6
// 				if (!user.status(proto).isOffline())
// 				{
// 					offline = false; // if online in at LEAST one proto
// 					break;
// 				}
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
// TODO: 0.6.6
// 			if (!elem.status(proto).isOffline())
// 			{
// 				offline = false;
// 				break;
// 			}
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
//		kdebugm(KDEBUG_INFO, "%s\n", qPrintable((*user).altNick()));
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
// TODO: 0.6.6
// 	if (!oldStatus.isBlocking() && elem.status(protocolName).isBlocking())
// 		addUser(elem);
// 	else if (oldStatus.isBlocking() && !elem.status(protocolName).isBlocking())
// 		removeUser(elem);
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
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", qPrintable(elem.ID("Gadu")), currentValue.toBool(), contains(elem));
	if (currentValue.toBool())
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsers::userAdded(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", qPrintable(elem.ID("Gadu")), elem.isAnonymous(), contains(elem));
	if (elem.isAnonymous())
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsers::removingUser(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", qPrintable(elem.ID("Gadu")), elem.isAnonymous(), contains(elem));
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

	connect(&pending, SIGNAL(messageFromUserAdded(Contact)),
			this, SLOT(messageFromUserAdded(Contact)));
	connect(&pending, SIGNAL(messageFromUserDeleted(Contact)),
			this, SLOT(messageFromUserDeleted(Contact)));
}

AnonymousUsersWithoutMessages::~AnonymousUsersWithoutMessages()
{
	disconnect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChangedSlot(UserListElement, QString, QVariant, QVariant, bool, bool)));
	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));

	disconnect(userlist, SIGNAL(removingUser(UserListElement, bool, bool)),
			this, SLOT(removingUser(UserListElement, bool, bool)));

	disconnect(&pending, SIGNAL(messageFromUserAdded(Contact)),
			this, SLOT(messageFromUserAdded(Contact)));
	disconnect(&pending, SIGNAL(messageFromUserDeleted(Contact)),
			this, SLOT(messageFromUserDeleted(Contact)));
}

static inline bool withoutMessages(const Contact &c)
{
	return !pending.pendingMsgs(c);
}

void AnonymousUsersWithoutMessages::userDataChangedSlot(UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last)
{
	if (name != "Anonymous")
		return;
//	kdebugmf(KDEBUG_WARNING, "%s %d %d %d\n", qPrintable(elem.ID("Gadu")), currentValue.toBool(), contains(elem), withoutMessages(elem));
	Contact contact = elem.toContact(AccountManager::instance()->defaultAccount());
	if (currentValue.toBool() && withoutMessages(contact))
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsersWithoutMessages::userAdded(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_ERROR, "%s %d %d\n", qPrintable(elem.ID("Gadu")), elem.isAnonymous(), contains(elem));
	Contact contact = elem.toContact(AccountManager::instance()->defaultAccount());
	if (elem.isAnonymous() && withoutMessages(contact))
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsersWithoutMessages::removingUser(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_WARNING, "%s %d %d %d\n", qPrintable(elem.ID("Gadu")), elem.isAnonymous(), contains(elem), withoutMessages(elem));
	Contact contact = elem.toContact(AccountManager::instance()->defaultAccount());
	if (withoutMessages(contact))
		addUser(elem, massively, last);
	else
		removeUser(elem, massively, last);
}

void AnonymousUsersWithoutMessages::messageFromUserAdded(Contact elem)
{
	removeUser(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount()));
}

void AnonymousUsersWithoutMessages::messageFromUserDeleted(Contact contact)
{
	UserListElement elem = UserListElement::fromContact(contact, AccountManager::instance()->defaultAccount());
	if (elem.isAnonymous() && withoutMessages(contact))
		addUser(elem);
}

BlockedUsers *blockedUsers;
BlockingUsers *blockingUsers;
OnlineUsers *onlineUsers;
OnlineAndDescriptionUsers *onlineAndDescriptionUsers;
OfflineUsers *offlineUsers;
AnonymousUsers *anonymousUsers;
AnonymousUsersWithoutMessages *anonymousUsersWithoutMessages;
GroupsManagerOld *groups_manager = 0;
