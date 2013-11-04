/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "string-list-storage.h"

#include "configuration/xml-configuration-file.h"

#include <QtCore/QVector>
#include <QtCore/QStringList>

StringListStorage::StringListStorage(XmlConfigFile *storageFile, QDomElement storagePoint, QString nodeName) :
		m_storageFile(storageFile), m_storagePoint(storagePoint), m_nodeName(nodeName)
{
}

QStringList StringListStorage::load() const
{
	auto result = QStringList();
	auto elements = m_storageFile->getNodes(m_storagePoint, m_nodeName);
	for (const auto &element : elements)
		result.append(element.text());
	return result;
}

void StringListStorage::store(const QStringList &value) const
{
	m_storageFile->removeChildren(m_storagePoint);

	for (const auto &value : value)
		m_storageFile->appendTextNode(m_storagePoint, m_nodeName, value);
}
