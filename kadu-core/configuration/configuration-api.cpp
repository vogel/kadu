/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>

#include <errno.h>

#include "configuration/configuration-read-error-exception.h"
#include "core/core.h"
#include "debug.h"

#include "configuration-api.h"

ConfigurationApi::ConfigurationApi()
{
	auto root = DomDocument.createElement("Kadu");
	DomDocument.appendChild(root);
}

ConfigurationApi::ConfigurationApi(const QString &content)
{
	if (!DomDocument.setContent(content))
	{
		throw ConfigurationReadErrorException();
	}

	if (DomDocument.documentElement().tagName() != "Kadu")
	{
		throw ConfigurationReadErrorException();
	}
}

void ConfigurationApi::touch()
{
	rootElement().setAttribute("last_save_time", QDateTime::currentDateTime().toString());
	rootElement().setAttribute("last_save_version", Core::version());
}

QString ConfigurationApi::configuration() const
{
	return DomDocument.toString();
}

QDomElement ConfigurationApi::rootElement()
{
	return DomDocument.documentElement();
}

QDomElement ConfigurationApi::createElement(QDomElement parent, const QString& tag_name)
{
	const QDomElement &elem = DomDocument.createElement(tag_name);
	parent.appendChild(elem);
	return elem;
}

QDomElement ConfigurationApi::findElement(const QDomElement &parent, const QString& tag_name) const
{
	return parent.firstChildElement(tag_name);
}

QDomElement ConfigurationApi::findElementByProperty(const QDomElement &parent, const QString &tag_name,
	const QString &property_name, const QString &property_value) const
{
	for (QDomElement elem = parent.firstChildElement(tag_name); !elem.isNull(); elem = elem.nextSiblingElement(tag_name))
	{
		const QString &val = elem.attribute(property_name);
		if (val == property_value)
			return elem;
	}
	return QDomNode().toElement();
}

QDomElement ConfigurationApi::findElementByFileNameProperty(const QDomElement &parent, const QString &tag_name,
	const QString &property_name, const QString &property_value) const
{
	for (QDomElement elem = parent.firstChildElement(tag_name); !elem.isNull(); elem = elem.nextSiblingElement(tag_name))
	{
		QString val = elem.attribute(property_name);
		if (val.section('/', -1).section('\\', -1) == property_value)
			return elem;
	}
	return QDomNode().toElement();
}

QDomElement ConfigurationApi::accessElement(const QDomElement &parent, const QString& tag_name)
{
	const QDomElement &elem = findElement(parent, tag_name);
	if (elem.isNull())
		return createElement(parent, tag_name);
	else
		return elem;
}

QDomElement ConfigurationApi::accessElementByProperty(const QDomElement &parent, const QString& tag_name,
	const QString& property_name, const QString& property_value)
{
	QDomElement elem = findElementByProperty(parent, tag_name,
		property_name, property_value);
	if (elem.isNull())
	{
		elem = createElement(parent, tag_name);
		elem.setAttribute(property_name, property_value);
	}
	return elem;
}

QDomElement ConfigurationApi::accessElementByFileNameProperty(const QDomElement &parent, const QString& tag_name,
	const QString& property_name, const QString& property_value)
{
	QDomElement elem = findElementByFileNameProperty(parent, tag_name,
		property_name, property_value);
	if (elem.isNull())
	{
		elem = createElement(parent, tag_name);
		elem.setAttribute(property_name, property_value);
	}
	return elem;
}

void ConfigurationApi::removeChildren(QDomElement parent)
{
	while (parent.hasChildNodes())
	{
		parent.firstChild().clear();
		parent.removeChild(parent.firstChild());
	}
}

void ConfigurationApi::removeNodes(QDomElement parentNode, const QVector<QDomElement> &elements)
{
	foreach (const QDomElement &element, elements)
		parentNode.removeChild(element);
}

void ConfigurationApi::removeNamedNodes(QDomElement parentNode, const QVector<QDomElement> &elements, const QString &name)
{
	foreach (const QDomElement &element, elements)
		if (isElementNamed(element, name))
			parentNode.removeChild(element);
}

void ConfigurationApi::removeUuidNodes(QDomElement parentNode, const QVector<QDomElement> &elements, const QString &uuid)
{
	foreach (const QDomElement &element, elements)
		if (isElementUuid(element, uuid))
			parentNode.removeChild(element);
}

bool ConfigurationApi::isElementNamed(const QDomElement &element, const QString &name)
{
	return element.hasAttribute("name") && name == element.attribute("name");
}

bool ConfigurationApi::isElementUuid(const QDomElement &element, const QString &uuid)
{
	return element.hasAttribute("uuid") && uuid == element.attribute("uuid");
}

bool ConfigurationApi::hasNode(const QString &nodeTagName)
{
	return !getNode(nodeTagName, ModeFind).isNull();
}

bool ConfigurationApi::hasNode(const QDomElement &parentNode, const QString &nodeTagName)
{
	return !getNode(parentNode, nodeTagName, ModeFind).isNull();
}

QDomElement ConfigurationApi::getNode(const QString &nodeTagName, GetNodeMode getMode)
{
	return getNode(DomDocument.documentElement(), nodeTagName, getMode);
}

QDomElement ConfigurationApi::getNamedNode(const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode)
{
	return getNamedNode(DomDocument.documentElement(), nodeTagName, nodeName, getMode);
}

QDomElement ConfigurationApi::getUuidNode(const QString &nodeTagName, const QString &nodeUuid, GetNodeMode getMode)
{
	return getUuidNode(DomDocument.documentElement(), nodeTagName, nodeUuid, getMode);
}

QDomElement ConfigurationApi::getNode(QDomElement parentNode, const QString &nodeTagName, GetNodeMode getMode)
{
	if (ModeCreate == getMode)
	{
		QVector<QDomElement> nodes = getNodes(parentNode, nodeTagName);
		removeNodes(parentNode, nodes);
	}
	else if (ModeAppend != getMode)
	{
		QDomElement elem = parentNode.firstChildElement(nodeTagName);
		if (!elem.isNull())
			return elem;
	}

	QDomElement result;
	if (ModeFind != getMode)
	{
		result = DomDocument.createElement(nodeTagName);
		parentNode.appendChild(result);
	}

	return result;
}

QDomElement ConfigurationApi::getNamedNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode)
{
	QVector<QDomElement> nodes = getNodes(parentNode, nodeTagName);

	if (ModeCreate == getMode)
		removeNamedNodes(parentNode, nodes, nodeName);

	if (ModeGet == getMode || ModeFind == getMode)
		foreach (const QDomElement &element, nodes)
			if (isElementNamed(element, nodeName))
				return element;

	QDomElement result;
	if (ModeFind != getMode)
	{
		result = DomDocument.createElement(nodeTagName);
		result.setAttribute("name", nodeName);
		parentNode.appendChild(result);
	}
	return result;
}

QDomElement ConfigurationApi::getUuidNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeUuid, GetNodeMode getMode)
{
	QVector<QDomElement> nodes = getNodes(parentNode, nodeTagName);

	if (ModeCreate == getMode)
		removeUuidNodes(parentNode, nodes, nodeUuid);

	if (ModeGet == getMode || ModeFind == getMode)
		foreach (const QDomElement &element, nodes)
			if (isElementUuid(element, nodeUuid))
				return element;

	QDomElement result;
	if (ModeFind != getMode)
	{
		result = DomDocument.createElement(nodeTagName);
		result.setAttribute("uuid", nodeUuid);
		parentNode.appendChild(result);
	}
	return result;
}

QVector<QDomElement> ConfigurationApi::getNodes(const QDomElement &parent, const QString &nodeTagName)
{
	QVector<QDomElement> result;

	for (QDomElement elem = parent.firstChildElement(nodeTagName); !elem.isNull(); elem = elem.nextSiblingElement(nodeTagName))
	    result.append(elem);

	return result;
}

QDomNode ConfigurationApi::cdataOrText(const QString &text)
{
	if (text.trimmed() != text)
		return DomDocument.createCDATASection(text);
	else
		return DomDocument.createTextNode(text);
}

void ConfigurationApi::appendTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &nodeContent)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeAppend);
	element.appendChild(cdataOrText(nodeContent));
}

void ConfigurationApi::createTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &nodeContent)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeCreate);
	element.appendChild(cdataOrText(nodeContent));
}

void ConfigurationApi::createNamedTextNode(const QDomElement &parentNode, const QString &nodeTagName,
		const QString &nodeName, const QString &nodeContent)
{
	QDomElement element = getNamedNode(parentNode, nodeTagName, nodeName, ModeCreate);
	element.appendChild(cdataOrText(nodeContent));
}

QString ConfigurationApi::getTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &defaultValue)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeFind);
	if (element.isNull())
		return defaultValue;

	return element.text();
}

void ConfigurationApi::removeNode(QDomElement parentNode, const QString& nodeTagName)
{
	QDomElement elementToRemove = getNode(parentNode, nodeTagName, ModeFind);
	while (!elementToRemove.isNull())
	{
		parentNode.removeChild(elementToRemove);
		elementToRemove = getNode(parentNode, nodeTagName, ModeFind);
	}
}
