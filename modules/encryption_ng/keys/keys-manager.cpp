/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QTimer>

#include "contacts/contact.h"

#include "keys/key.h"
#include "keys/key-shared.h"

#include "keys-manager.h"

KeysManager * KeysManager::Instance = 0;

KeysManager * KeysManager::instance()
{
	if (!Instance)
		Instance = new KeysManager();

	return Instance;
}

KeysManager::KeysManager()
{
}

KeysManager::~KeysManager()
{
}

void KeysManager::itemAboutToBeAdded(Key item)
{
	emit keyAboutToBeAdded(item);
}

void KeysManager::itemAdded(Key item)
{
	connect(item, SIGNAL(updated()), this, SLOT(keyDataUpdated()));
	emit keyAdded(item);
}

void KeysManager::itemAboutToBeRemoved(Key item)
{
	emit keyAboutToBeRemoved(item);
	disconnect(item, SIGNAL(updated()), this, SLOT(keyDataUpdated()));
}

void KeysManager::itemRemoved(Key item)
{
	emit keyRemoved(item);
}

void KeysManager::keyDataUpdated()
{
	QMutexLocker(&mutex());

	Key key(sender());
	if (key)
		emit keyUpdated(key);
}

Key KeysManager::byContact(Contact contact, NotFoundAction action)
{
	Q_UNUSED(contact)
	Q_UNUSED(action)

	return Key::null;
/*
	if (contact.contactAvatar())
		return contact.contactAvatar();

	if (ActionReturnNull == action)
		return Avatar::null;

	Avatar avatar = Avatar::create();
	contact.setContactAvatar(avatar);

	if (ActionCreateAndAdd == action)
		addItem(avatar);

	return avatar;
*/
}
