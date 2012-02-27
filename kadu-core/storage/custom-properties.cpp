/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QVariant>

#include "custom-properties.h"
#include "storage-point.h"
#include <configuration/xml-configuration-file.h>

CustomProperties::CustomProperties(QObject *parent) :
		QObject(parent)
{
}

CustomProperties::~CustomProperties()
{
}

bool CustomProperties::shouldStore() const
{
	return !StorableProperties.isEmpty();
}

void CustomProperties::loadFromModuleData(const QDomNode &node)
{
	const QDomElement &element = node.toElement();
	const QString namespaceName = element.attribute("name");

	if (namespaceName.isEmpty())
		return;

	const QDomNodeList &propertyNodes = element.childNodes();
	const int propertyNodesCount = propertyNodes.length();
	for (int i = 0; i < propertyNodesCount; i++)
	{
		const QDomElement &propertyElement = propertyNodes.at(i).toElement();
		if (!propertyElement.isElement())
			continue;

		QString propertyName = QString("%1:%2").arg(namespaceName).arg(propertyElement.tagName());
		QString propertyValue = propertyElement.text();

		addProperty(propertyName, propertyValue, Storable);
	}
}

void CustomProperties::loadFrom(const QSharedPointer<StoragePoint> &storagePoint)
{
	if (!storagePoint)
		return;

	QDomElement element = storagePoint->point();
	const QDomNodeList &moduleDataNodes = element.elementsByTagName("ModuleData");
	const int moduleDataNodesCount = moduleDataNodes.length();
	for (int i = 0; i < moduleDataNodesCount; i++)
		loadFromModuleData(moduleDataNodes.at(i));
	for (int i = 0; i < moduleDataNodesCount; i++)
		element.removeChild(moduleDataNodes.at(i));

	const QDomNodeList &customProperties = element.elementsByTagName("CustomProperty");
	const int customPropertiesCount = customProperties.length();
	for (int i = 0; i < customPropertiesCount; i++)
	{
		const QDomElement &propertyElement = customProperties.at(i).toElement();
		if (!propertyElement.isElement())
			continue;

		QString propertyName = propertyElement.attribute("name");
		QString propertyValue = propertyElement.text();

		if (!propertyName.isEmpty())
			addProperty(propertyName, propertyValue, Storable);
	}
}

void CustomProperties::storeTo(const QSharedPointer<StoragePoint> &storagePoint) const
{
	if (!storagePoint)
		return;

	QDomElement element = storagePoint->point();
	const QDomNodeList &customProperties = element.elementsByTagName("CustomProperty");
	const int customPropertiesCount = customProperties.length();
	for (int i = 0; i < customPropertiesCount; i++)
		element.removeChild(customProperties.at(i));

	foreach (const QString &propertyName, StorableProperties)
		storagePoint->storage()->createNamedTextNode(storagePoint->point(), "CustomProperty",
				propertyName, Properties.value(propertyName).toString());
}

bool CustomProperties::hasProperty(const QString &name) const
{
	Q_ASSERT(!name.isEmpty());

	return Properties.contains(name);
}

void CustomProperties::addProperty(const QString &name, const QVariant &value, Storability storability)
{
	Q_ASSERT(!name.isEmpty());

	if (Storable == storability)
		StorableProperties.insert(name);
	else
		StorableProperties.remove(name);

	Properties.insert(name, value);
}

void CustomProperties::removeProperty(const QString &name)
{
	Q_ASSERT(!name.isEmpty());

	StorableProperties.remove(name);
	Properties.remove(name);
}

QVariant CustomProperties::property(const QString &name, const QVariant &defaultValue) const
{
	if (Properties.contains(name))
		return Properties.value(name);
	else
		return defaultValue;
}
