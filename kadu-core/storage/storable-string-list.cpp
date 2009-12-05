/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "storable-string-list.h"

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

	clear();

	XmlConfigFile *storageFile = storage()->storage();
	QDomElement point = storage()->point();

	QList<QDomElement> elements = storageFile->getNodes(point, storageItemNodeName());
	foreach (QDomElement element, elements)
		append(element.text());
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

	XmlConfigFile *storageFile = storage()->storage();
	QDomElement point = storage()->point();

	storageFile->removeChildren(point);

	foreach (const QString &value, content())
		storageFile->appendTextNode(point, storageItemNodeName(), value);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns sontent of this string list.
 * @return Content of this string list
 *
 * Returns content of this string list.
 */
const QStringList StorableStringList::content() const
{
	return *this;
}
