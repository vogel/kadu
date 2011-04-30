/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/type/chat-type.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "parser/parser.h"
#include "debug.h"

#include "chat-shared.h"

ChatShared * ChatShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
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
ChatShared * ChatShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
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
		Shared(uuid), IgnoreAllMessages(false)
{
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

	Type = loadValue<QString>("Type");
	ChatAccount = AccountManager::instance()->byUuid(QUuid(loadValue<QString>("Account")));

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

	storeValue("Type", Type);
	storeValue("Account", ChatAccount.uuid().toString());

	if (details())
		details()->store();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if object should be stored (is valid).
 * @return true if object should be stored (is valid)
 *
 * Returns true if object shoudl be stored (data is valid). When account is not set
 * or details class is loaded and is invalid chat data is removed from storage (it
 * is unusable after all).
 */
bool ChatShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore()
			&& !ChatAccount.uuid().isNull()
			&& (!details() || details()->shouldStore());
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
	ChatAccount = Account::null;
	setDetails(0);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Called after any chat data was changed.
 *
 * Method is called after any chat data was changed and signal emiting is non blocked.
 * Informs any object about chat data change.
 */
void ChatShared::emitUpdated()
{
	emit updated();
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
	if (details())
		return;

	if (chatType->name() != Type)
		return;

	setDetails(chatType->createChatDetails(this));
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
	if (!details())
		return;

	if (chatType->name() != Type)
		return;

	setDetails(0);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Called after new details class was assigned to this object.
 *
 * After new details class is assigned to this object, it data is loaded from storage.
 */
void ChatShared::detailsAdded()
{
	details()->ensureLoaded();

	ChatManager::instance()->detailsLoaded(this);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Called before old details class is removed from this object.
 *
 * Before old details class is removed from this object its data is stored to storage.
 */
void ChatShared::detailsAboutToBeRemoved()
{
	details()->store();
}

void ChatShared::detailsRemoved()
{
	ChatManager::instance()->detailsUnloaded(this);
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

	return details() ? details()->contacts() : ContactSet();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns chat name.
 * @return chat name
 *
 * Returns chae name. Name fetched from details object, so if no details object is present
 * empty string will be returned.
 */
QString ChatShared::name()
{
	ensureLoaded();

	return details() ? details()->name() : QString();
}
