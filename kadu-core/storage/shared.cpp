/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/change-notifier.h"

#include "shared.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @param uuid objects uuid
 * @short Contructs empty object with given (or generated) uuid.
 *
 * Contructs empty object with given uuid. When uuid is invalid (NULL)
 * new uuid is created and assigned to object.
 */
Shared::Shared(const QUuid &uuid)
{
	setUuid(uuid.isNull() ? QUuid::createUuid() : uuid);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @author Bartosz 'beevvy' Brachaczek
 * @short Destroys object.
 *
 * Destroys object.
 *
 * If you are going to reimplement destructor, you need to call ref.ref() at
 * the beginning of it. This is needed to prevent infinite recursion that may
 * occur in case some method called directly or indirectly by this destructor
 * created SharedBase-derived object with pointer to this. Then, when this
 * object would be destroyed (I assume it was a short-lifetime object), it
 * would also try to delete this Shared object because its reference count was
 * already 0 at time of creation of that SharedBase-derived object. This in
 * turn would lead to calling the same destructor again, which destructor would
 * do exactly the same thing, etc.
 */
Shared::~Shared()
{
	ref.ref();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads object from storage.
 *
 * Loads the object from storage. Loads uuid from uuid attribute. Superclass method
 * is also called. You must call this method in load methods of derivered class.
 *
 * When reimplementing this method, avoid calling any methods or emitting signals.
 * It could lead to big trouble as the object is in an unstable state where State
 * is set to StateLoaded while actually it may be not fully loaded yet.
 */
void Shared::load()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::load();
	setUuid(QUuid(loadAttribute<QString>("uuid")));
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads object from storage.
 *
 * Loads obejct from storage. Loads only uuid from uuid attribute. Superclass method
 * is not called (storage status remaining unchanged - access to each other property will
 * fire load method). You must call this method in loadStub methods of derivered class.
 */
void Shared::loadStub()
{
	if (!isValidStorage())
		return;

	setUuid(QUuid(loadAttribute<QString>("uuid")));
	setState(StateNotLoaded);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores object to storage.
 *
 * Storeas obejct to storage. Stored uuid to uuid attribute.
 * You must call this method in load methods of derivered class.
 */
void Shared::store()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::store();

	storeAttribute("uuid", uuid().toString());
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Method called just before this obejct is removed from storage.
 *
 * Method is called just before this obejct is removed from storage.
 * Override it when you need to take special actions (like removing files from disk).
 * If this object stored references to other SharedBase objects you must set them
 * all to null value or remove them (if stored in list).
 */
void Shared::aboutToBeRemoved()
{
}

ChangeNotifier & Shared::changeNotifier()
{
	return MyChangeNotifier;
}
