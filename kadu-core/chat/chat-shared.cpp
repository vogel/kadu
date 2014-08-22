/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/group-manager.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "chat/type/chat-type.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "misc/change-notifier.h"
#include "parser/parser.h"
#include "debug.h"

#include "chat-shared.h"

ChatShared * ChatShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	ChatShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads chat data from given storage point.
 * @param storagePoint storagePoint from which chat data will be loaded
 *
 * Creates new object of ChatShared type and assigns storagePoint to it.
 * Object is lazy-loaded (it will be loaded when used first time).
 */
ChatShared * ChatShared::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	ChatShared *result = new ChatShared();
	result->setStorage(storagePoint);

	return result;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new ChatShared object.
 * @param uuid unique id of object
 *
 * Creates new object of ChatShared type with no storagePoint and
 * storage state set to StateNew. If unique id is provied it will
 * be assigned to this object, if not, new unique id will be
 * created.
 */
ChatShared::ChatShared(const QUuid &uuid) :
		Shared(uuid), Details(0), IgnoreAllMessages(false), UnreadMessagesCount(0), Open(false)
{
	ChatAccount = new Account();

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Destroys ChatShared object.
 *
 * Destroys object of ChatShared type. It also destros all details
 * data assigned to this object.
 */
ChatShared::~ChatShared()
{
	ref.ref();

	triggerAllChatTypesUnregistered();

	delete ChatAccount;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns parent storage node for this object - ChatManager node.
 * @return parent storage node for this object - @link ChatManager @endlink node
 *
 * Returns parent storage node for this object - @link ChatManager @endlink node.
 */
StorableObject * ChatShared::storageParent()
{
	return ChatManager::instance();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns storage node name for this object - 'Chat'.
 * @return storage node name for this object - 'Chat'
 *
 * Returns storage node name for this object - 'Chat'.
 */
QString ChatShared::storageNodeName()
{
	return QLatin1String("Chat");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads chat data from storage.
 *
 * This method is called when object is used at first time. It loads data from object's
 * storage point. After loading data chat type is known ('Type') so this method check
 * if the type is any of known chat types. If so, details class is created, assigned and
 * loaded - chat has full data available. If no, loading details class is deffered to
 * moment after good chat type is loaded. This mechanism is provided by
 * @link ChatTypeAvareObject @endlink class.
 */
void ChatShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement parent = storage()->point();

	Groups.clear();

	QDomElement groupsNode = configurationStorage->getNode(parent, "ChatGroups", XmlConfigFile::ModeFind);
	if (!groupsNode.isNull())
	{
		QDomNodeList groupsList = groupsNode.elementsByTagName("Group");

		int count = groupsList.count();
		for (int i = 0; i < count; i++)
		{
			QDomElement groupElement = groupsList.at(i).toElement();
			if (groupElement.isNull())
				continue;
			doAddToGroup(GroupManager::instance()->byUuid(groupElement.text()));
		}
	}

	*ChatAccount = AccountManager::instance()->byUuid(QUuid(loadValue<QString>("Account")));
	Display = loadValue<QString>("Display");
	Type = loadValue<QString>("Type");

	// import from alias to new name of chat type
	ChatType *chatType = ChatTypeManager::instance()->chatType(Type);
	if (chatType)
		Type = chatType->name();

	// we should not have display names for Contact chats
	if (Type == "Contact")
		Display.clear();

	triggerAllChatTypesRegistered();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stored chat data to storage.
 *
 * Stores all chat data to storage. If details class is loaded it is stored too.
 */
void ChatShared::store()
{
	ensureLoaded();

	if (!isValidStorage())
		return;

	Shared::store();

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement parent = storage()->point();

	storeValue("Account", ChatAccount->uuid().toString());
	storeValue("Display", Display);

	// import from alias to new name of chat type
	ChatType *chatType = ChatTypeManager::instance()->chatType(Type);
	if (chatType)
		Type = chatType->name();

	storeValue("Type", Type);

	if (!Groups.isEmpty())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "ChatGroups", XmlConfigFile::ModeCreate);
		foreach (const Group &group, Groups)
			configurationStorage->appendTextNode(groupsNode, "Group", group.uuid().toString());
	}
	else
		configurationStorage->removeNode(parent, "ChatGroups");

	if (Details)
		Details->ensureStored();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if object should be stored.
 * @return true if object should be stored
 *
 * Chat is only stored when it is valid and has either any custom property or display name set.
 */
bool ChatShared::shouldStore()
{
	ensureLoaded();

	if (type().isEmpty())
		return false;

	// we dont need data for non-roster contacts only from 4 version of sql schema
	if (config_file.readNumEntry("History", "Schema", 0) < 4)
		return true;

	if (customProperties()->shouldStore())
		return true;

	return UuidStorableObject::shouldStore()
			&& !ChatAccount->uuid().isNull()
			&& (!Details || Details->shouldStore())
			&& (!Display.isEmpty() && type() != "Contact");
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Called before chat data is removed from storage.
 *
 * Method is called before chat data is removed from storage. It clears all references (to chat account)
 * and removes details object.
 */
void ChatShared::aboutToBeRemoved()
{
	*ChatAccount = Account::null;
	Groups.clear();

	if (Details)
	{
		Details->ensureStored();
		delete Details;
		Details = 0;
	}
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Called after new chat type was registered.
 *
 * If no details class is loaded and registered chat type is valid for this chat
 * new details class is created, assigned and loaded for this object.
 */
void ChatShared::chatTypeRegistered(ChatType *chatType)
{
	if (chatType->name() != Type)
		return;

	if (!Details)
	{
		Details = chatType->createChatDetails(this);
		if (!Details)
			return;

		connect(Details, SIGNAL(connected()), this, SIGNAL(connected()));
		connect(Details, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
		connect(Details, SIGNAL(contactAboutToBeAdded(Contact)), this, SIGNAL(contactAboutToBeAdded(Contact)));
		connect(Details, SIGNAL(contactAdded(Contact)), this, SIGNAL(contactAdded(Contact)));
		connect(Details, SIGNAL(contactAboutToBeRemoved(Contact)), this, SIGNAL(contactAboutToBeRemoved(Contact)));
		connect(Details, SIGNAL(contactRemoved(Contact)), this, SIGNAL(contactRemoved(Contact)));

		Details->ensureLoaded();
	}

	ChatManager::instance()->registerItem(this);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Called after chat type was unregistered.
 *
 * If details class is loaded and unregistered chat type is valid for this chat
 * details class is stored and removed from this object.
 */
void ChatShared::chatTypeUnregistered(ChatType *chatType)
{
	if (chatType->name() != Type)
		return;

	if (Details)
	{
		Details->ensureStored();
		delete Details;
		Details = 0;
	}

	ChatManager::instance()->unregisterItem(this);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns set of contacts from this chat.
 * @return set of contacts from this chat
 *
 * Returns set of contacts from this chat. Set is fetched from details object, so if no
 * details object is present, empty set will be returned.
 */
ContactSet ChatShared::contacts()
{
	ensureLoaded();

	return Details ? Details->contacts() : ContactSet();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns chat name.
 * @return chat name
 *
 * Returns chat name. Name is fetched from details object, so if no details object is present
 * empty string will be returned.
 */
QString ChatShared::name()
{
	ensureLoaded();

	return Details ? Details->name() : QString();
}

void ChatShared::setType(const QString &type)
{
	ensureLoaded();

	if (Type == type)
		return;

	if (Details)
	{
		Details->ensureStored();
		delete Details;
		Details = 0;

		ChatManager::instance()->unregisterItem(this);
	}

	Type = type;

	ChatType *chatType = ChatTypeManager::instance()->chatType(type);
	if (chatType)
		chatTypeRegistered(chatType); // this will add details
}

void ChatShared::setGroups(const QSet<Group> &groups)
{
	ensureLoaded();

	if (Groups == groups)
		return;

	QSet<Group> groupsToRemove = Groups;

	foreach (const Group &group, groups)
		if (!groupsToRemove.remove(group))
			doAddToGroup(group);

	foreach (const Group &group, groupsToRemove)
		doRemoveFromGroup(group);

	changeNotifier().notify();
}

bool ChatShared::isInGroup(const Group &group)
{
	ensureLoaded();

	return Groups.contains(group);
}

bool ChatShared::showInAllGroup()
{
	ensureLoaded();

	foreach (const Group &group, Groups)
		if (group && !group.showInAllGroup())
			return false;

	return true;
}

bool ChatShared::doAddToGroup(const Group &group)
{
	if (!group || Groups.contains(group))
		return false;

	Groups.insert(group);
	connect(group, SIGNAL(groupAboutToBeRemoved()), this, SLOT(groupAboutToBeRemoved()));

	return true;
}

bool ChatShared::doRemoveFromGroup(const Group &group)
{
	if (!Groups.remove(group))
		return false;

	disconnect(group, 0, this, 0);

	return true;
}

void ChatShared::addToGroup(const Group &group)
{
	ensureLoaded();

	if (doAddToGroup(group))
		changeNotifier().notify();
}

void ChatShared::removeFromGroup(const Group &group)
{
	ensureLoaded();

	if (doRemoveFromGroup(group))
		changeNotifier().notify();
}

void ChatShared::groupAboutToBeRemoved()
{
	Group group(sender());
	if (!group.isNull())
		removeFromGroup(group);
}

KaduShared_PropertyPtrDefCRW(ChatShared, Account, chatAccount, ChatAccount);

bool ChatShared::isConnected() const
{
	if (!Details)
		return false;

	return Details->isConnected();
}

bool ChatShared::isOpen() const
{
	return Open;
}

void ChatShared::setOpen(bool open)
{
	if (Open == open)
		return;

	Open = open;
	if (Open)
		emit opened();
	else
		emit closed();
}

#include "moc_chat-shared.cpp"
