/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QList>
#include <QtXml/QDomDocument>

#include "exports.h"

class KADUAPI ConfigurationApi
{
public:
	enum GetNodeMode
	{
		ModeGet,
		ModeFind,
		ModeCreate,
		ModeAppend
	};

private:
	QDomDocument DomDocument;

	bool isElementNamed(const QDomElement &element, const QString &name);
	bool isElementUuid(const QDomElement &element, const QString &uuid);

	void removeNodes(QDomElement parentNode, const QVector<QDomElement> &elements);
	void removeUuidNodes(QDomElement parentNode, const QVector<QDomElement> &elements, const QString &uuid);
	void removeNamedNodes(QDomElement parentNode, const QVector<QDomElement> &elements, const QString &name);

	QDomNode cdataOrText(const QString &text);

public:
	ConfigurationApi();
	explicit ConfigurationApi(const QString &content);

	void touch();

	QString configuration() const;

	QDomElement rootElement();
	QDomElement createElement(QDomElement parent, const QString &tag_name);
	QDomElement findElement(const QDomElement &parent, const QString &tag_name) const;
	QDomElement findElementByProperty(const QDomElement &parent, const QString &tag_name,
		const QString &property_name, const QString &property_value) const;
	QDomElement findElementByFileNameProperty(const QDomElement &parent, const QString &tag_name,
		const QString &property_name, const QString &property_value) const;

	QDomElement accessElement(const QDomElement &parent, const QString &tag_name);
	QDomElement accessElementByProperty(const QDomElement &parent, const QString &tag_name,
		const QString &property_name, const QString &property_value);
	QDomElement accessElementByFileNameProperty(const QDomElement &parent, const QString &tag_name,
		const QString &property_name, const QString &property_value);

	void removeChildren(QDomElement parent);

	bool hasNode(const QString &nodeTagName);
	bool hasNode(const QDomElement &parentNode, const QString &nodeTagName);

	QDomElement getNode(const QString &nodeTagName, GetNodeMode getMode = ModeGet);
	QDomElement getNamedNode(const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode = ModeGet);
	QDomElement getUuidNode(const QString &nodeTagName, const QString &nodeUuid, GetNodeMode getMode = ModeGet);
	QDomElement getNode(QDomElement parentNode, const QString &nodeTagName, GetNodeMode getMode = ModeGet);
	QDomElement getNamedNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode = ModeGet);
	QDomElement getUuidNode(QDomElement parentNode, const QString &uuidTagName, const QString &nodeUuid, GetNodeMode getMode = ModeGet);

	QVector<QDomElement> getNodes(const QDomElement &parent, const QString &nodeTagName);

	void appendTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &nodeContent);
	void createTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &nodeContent);
	void createNamedTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &nodeName,
			const QString &nodeContent);
	QString getTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &defaultValue = QString());

	void removeNode(QDomElement parentNode, const QString &nodeTagName);

};
