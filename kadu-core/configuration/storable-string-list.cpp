/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "storable-string-list.h"

StorableStringList::StorableStringList(const QString &nodeName, const QString contentNodeName, StorableObject *parent) :
		StorableObject(nodeName, parent), ContentNodeName(contentNodeName)
{
}

void StorableStringList::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	clear();

	XmlConfigFile *storageFile = storage()->storage();
	QDomElement point = storage()->point();

	QDomNodeList nodes = storageFile->getNodes(point, ContentNodeName);
	int count = nodes.count();

	for (int i = 0; i < count; i++)
	{
		QDomNode node = nodes.at(i);
		if (!node.isElement())
			return;

		QDomElement element = node.toElement();
		append(element.text());
	}
}

void StorableStringList::store()
{
	if (!isValidStorage())
		return;

	XmlConfigFile *storageFile = storage()->storage();
	QDomElement point = storage()->point();

	storageFile->removeChildren(point);

	foreach (const QString &value, content())
		storageFile->appendTextNode(point, ContentNodeName, value);
}

const QStringList StorableStringList::content() const
{
	return *this;
}
