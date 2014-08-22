/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QVector>

#include "storable-string-list.h"

#include "storage/string-list-storage.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Contructs empty object.
 *
 * Contructs empty object with state @link<StorableObject::StateNew StateNew @endlink.
 */
StorableStringList::StorableStringList()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Load string list from storagePoint XML node.
 *
 * Load string list from storagePoint XML node. Each item is loaded from subnode
 * with name given by methos @link<storageItemNodeName> storageItemNodeName @endlink.
 * If storagePoint is invalid no data is loaded (and no data is removed from this
 * string list).
 */
void StorableStringList::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	auto stringListStorage = StringListStorage(storage().get(), storageItemNodeName());
	StringList = stringListStorage.load();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Store string list to storagePoint XML node.
 *
 * Store string list to storagePoint XML node. Each item is stored to subnode
 * with name given by methos @link<storageItemNodeName> storageItemNodeName @endlink.
 */
void StorableStringList::store()
{
	if (!isValidStorage())
		return;

	auto stringListStorage = StringListStorage(storage().get(), storageItemNodeName());
	stringListStorage.store(content());
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns sontent of this string list.
 * @return Content of this string list
 *
 * Returns content of this string list.
 */
const QStringList & StorableStringList::content() const
{
	return StringList;
}
