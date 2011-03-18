/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>

#include <errno.h>

#include "core/core.h"
#include "misc/misc.h"
#include "debug.h"
#include "kadu-config.h"

#include "xml-configuration-file.h"

XmlConfigFile::XmlConfigFile() : DomDocument()
{
	read();
}

void XmlConfigFile::read()
{
	kdebugf();
	QFile file;
	QDir backups(profilePath(), "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files);
	QDir oldbackups(profilePath(), "kadu.conf.xml.backup.*", QDir::Name, QDir::Files);
	QStringList files("kadu-0.6.6.conf.xml");

	files += backups.entryList();
	files += "kadu.conf.xml";
	files += oldbackups.entryList();

	bool fileOpened(false);

	foreach (const QString &fileName, files)
	{
		file.setFileName(profilePath(fileName));
		fileOpened = file.open(QIODevice::ReadOnly);
		if (fileOpened && file.size() > 0)
		{
			kdebugm(KDEBUG_INFO, "configuration file %s opened!\n", qPrintable(file.fileName()));
			break;
		}
		if (fileOpened) // && file.size() == 0
		{
			kdebugm(KDEBUG_INFO, "config file (%s) is empty, looking for backup\n", qPrintable(file.fileName()));
			file.close();
			fileOpened = false;
		}
		else
		{
			kdebugm(KDEBUG_INFO, "config file (%s) not opened, looking for backup\n", qPrintable(file.fileName()));
		}
	}

	if (fileOpened)
	{
		if (DomDocument.setContent(&file))
			kdebugm(KDEBUG_INFO, "xml configuration file loaded\n");
		else
		{
			fprintf(stderr, "error reading or parsing xml configuration file\n");
			fflush(stderr);
		}
		file.close();

		if (DomDocument.documentElement().tagName() != "Kadu")
		{
			QDomElement root = DomDocument.createElement( "Kadu" );
			DomDocument.appendChild(root);
		}
	}
	else
	{
		fprintf(stderr, "error opening xml configuration file (%s), creating empty document\n", qPrintable(file.errorString()));
		fflush(stderr);
		QDomElement root = DomDocument.createElement( "Kadu" );
		DomDocument.appendChild(root);
	}

	kdebugf2();
}

void XmlConfigFile::write(const QString& f)
{
	kdebugf();
	rootElement().setAttribute("last_save_time", QDateTime::currentDateTime().toString());
	rootElement().setAttribute("last_save_version", Core::version());
	QFile file;
	QString fileName, tmpFileName;
	if (f.isEmpty())
		fileName = profilePath("kadu-0.6.6.conf.xml");
	else
		fileName = f;
	tmpFileName = fileName + ".tmp"; // saving to another file to avoid truncation of output file when segfault occurs :|
	file.setFileName(tmpFileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		kdebugm(KDEBUG_INFO, "file opened '%s'\n", qPrintable(file.fileName()));
		QTextStream stream(&file);
		stream.setCodec(QTextCodec::codecForName("UTF-8"));
		stream << DomDocument.toString();
		file.close();
		// remove old file (win32)
		QFile::remove(fileName);
		if (!QFile::rename(tmpFileName, fileName))
		{
			fprintf(stderr, "cannot rename '%s' to '%s': %s\n", qPrintable(tmpFileName), qPrintable(fileName), strerror(errno));
			fflush(stderr);
		}
	}
	else
	{
		fprintf(stderr, "cannot open '%s': %s\n", qPrintable(file.fileName()), qPrintable(file.errorString()));
		fflush(stderr);
	}
	kdebugf2();
}

void XmlConfigFile::sync()
{
	write();
}

void XmlConfigFile::saveTo(const QString &f)
{
	write(f);
}

void XmlConfigFile::makeBackup()
{
	QString f = QString("kadu-0.6.6.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
	write(profilePath(f));
}

QDomElement XmlConfigFile::rootElement()
{
	return DomDocument.documentElement();
}

QDomElement XmlConfigFile::createElement(QDomElement parent, const QString& tag_name)
{
	const QDomElement &elem = DomDocument.createElement(tag_name);
	parent.appendChild(elem);
	return elem;
}

QDomElement XmlConfigFile::findElement(const QDomElement &parent, const QString& tag_name) const
{
	for (QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &e = n.toElement();
		if (e.isNull())
			continue;
		if (e.tagName() == tag_name)
			return e;
	}
	return QDomNode().toElement();
}

QDomElement XmlConfigFile::findElementByProperty(const QDomElement &parent, const QString &tag_name,
	const QString &property_name, const QString &property_value) const
{
	for (QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &e = n.toElement();
		if (e.isNull())
			continue;
		if (e.tagName() != tag_name)
			continue;
		const QString &val = e.attribute(property_name);
		if (val == property_value)
			return e;
	}
	return QDomNode().toElement();
}

QDomElement XmlConfigFile::findElementByFileNameProperty(const QDomElement &parent, const QString &tag_name,
	const QString &property_name, const QString &property_value) const
{
	for (QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &e = n.toElement();
		if (e.isNull())
			continue;
		if (e.tagName() != tag_name)
			continue;
		QString val = e.attribute(property_name);
		if (val.section('/', -1).section('\\', -1) == property_value)
			return e;
	}
	return QDomNode().toElement();
}

QDomElement XmlConfigFile::accessElement(const QDomElement &parent, const QString& tag_name)
{
	const QDomElement &elem = findElement(parent, tag_name);
	if (elem.isNull())
		return createElement(parent, tag_name);
	else
		return elem;
}

QDomElement XmlConfigFile::accessElementByProperty(const QDomElement &parent, const QString& tag_name,
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

QDomElement XmlConfigFile::accessElementByFileNameProperty(const QDomElement &parent, const QString& tag_name,
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

void XmlConfigFile::removeChildren(QDomElement parent)
{
	while (parent.hasChildNodes())
	{
		parent.firstChild().clear();
		parent.removeChild(parent.firstChild());
	}
}

void XmlConfigFile::removeNodes(QDomElement parentNode, const QList<QDomElement> &elements)
{
	foreach (const QDomElement &element, elements)
		parentNode.removeChild(element);
}

void XmlConfigFile::removeNamedNodes(QDomElement parentNode, const QList<QDomElement> &elements, const QString &name)
{
	foreach (const QDomElement &element, elements)
		if (isElementNamed(element, name))
			parentNode.removeChild(element);
}

void XmlConfigFile::removeUuidNodes(QDomElement parentNode, const QList<QDomElement> &elements, const QString &uuid)
{
	foreach (const QDomElement &element, elements)
		if (isElementUuid(element, uuid))
			parentNode.removeChild(element);
}

bool XmlConfigFile::isElementNamed(const QDomElement &element, const QString &name)
{
	return element.hasAttribute("name") && name == element.attribute("name");
}

bool XmlConfigFile::isElementUuid(const QDomElement &element, const QString &uuid)
{
	return element.hasAttribute("uuid") && uuid == element.attribute("uuid");
}

bool XmlConfigFile::hasNode(const QString &nodeTagName)
{
	return !getNode(nodeTagName, ModeFind).isNull();
}

bool XmlConfigFile::hasNode(const QDomElement &parentNode, const QString &nodeTagName)
{
	return !getNode(parentNode, nodeTagName, ModeFind).isNull();
}

QDomElement XmlConfigFile::getNode(const QString &nodeTagName, GetNodeMode getMode)
{
	return getNode(DomDocument.documentElement(), nodeTagName, getMode);
}

QDomElement XmlConfigFile::getNamedNode(const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode)
{
	return getNamedNode(DomDocument.documentElement(), nodeTagName, nodeName, getMode);
}

QDomElement XmlConfigFile::getUuidNode(const QString &nodeTagName, const QString &nodeUuid, GetNodeMode getMode)
{
	return getNamedNode(DomDocument.documentElement(), nodeTagName, nodeUuid, getMode);
}

QDomElement XmlConfigFile::getNode(QDomElement parentNode, const QString &nodeTagName, GetNodeMode getMode)
{
	QDomElement result;
	QList<QDomElement> nodes = getNodes(parentNode, nodeTagName);

	if (ModeCreate == getMode)
		removeNodes(parentNode, nodes);
	else if (ModeAppend != getMode && !nodes.isEmpty())
		return nodes.at(0);

	if (ModeFind != getMode)
	{
		result = DomDocument.createElement(nodeTagName);
		parentNode.appendChild(result);
	}

	return result;
}

QDomElement XmlConfigFile::getNamedNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeName, GetNodeMode getMode)
{
	QDomElement result;
	QList<QDomElement> nodes = getNodes(parentNode, nodeTagName);

	if (ModeAppend == getMode)
		return result;

	if (ModeCreate == getMode)
		removeNamedNodes(parentNode, nodes, nodeName);

	foreach (const QDomElement &element, nodes)
		if (isElementNamed(element, nodeName))
			return element;

	if (ModeFind != getMode)
	{
		result = DomDocument.createElement(nodeTagName);
		result.setAttribute("name", nodeName);
		parentNode.appendChild(result);
	}

	return result;
}

QDomElement XmlConfigFile::getUuidNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeUuid, GetNodeMode getMode)
{
	QDomElement result;
	QList<QDomElement> nodes = getNodes(parentNode, nodeTagName);

	if (ModeAppend == getMode)
		return result;

	if (ModeCreate == getMode)
		removeUuidNodes(parentNode, nodes, nodeUuid);

	foreach (const QDomElement &element, nodes)
		if (isElementUuid(element, nodeUuid))
			return element;

	if (ModeFind != getMode)
	{
		result = DomDocument.createElement(nodeTagName);
		result.setAttribute("uuid", nodeUuid);
		parentNode.appendChild(result);
	}

	return result;
}

QList<QDomElement> XmlConfigFile::getNodes(const QDomElement &parent, const QString &nodeTagName)
{
	QDomNodeList nodes = parent.childNodes();
	QList<QDomElement> result;

	int count = nodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement element = nodes.at(i).toElement();
		if (element.isNull())
			continue;

		if (element.tagName() == nodeTagName)
			result.append(element);
	}

	return result;
}

void XmlConfigFile::appendTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &nodeContent)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeAppend);
	element.appendChild(DomDocument.createTextNode(nodeContent));
}

void XmlConfigFile::createTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &nodeContent)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeCreate);
	element.appendChild(DomDocument.createTextNode(nodeContent));
}

void XmlConfigFile::createNamedTextNode(const QDomElement &parentNode, const QString &nodeTagName,
		const QString &nodeName, const QString &nodeContent)
{
	QDomElement element = getNamedNode(parentNode, nodeTagName, nodeName, ModeCreate);
	element.appendChild(DomDocument.createTextNode(nodeContent));
}

QString XmlConfigFile::getTextNode(const QDomElement &parentNode, const QString &nodeTagName, const QString &defaultValue)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeFind);
	if (element.isNull())
		return defaultValue;

	return element.text();
}

void XmlConfigFile::removeNode(QDomElement parentNode, const QString& nodeTagName)
{
	QDomElement elementToRemove = getNode(parentNode, nodeTagName, ModeFind);
	while (!elementToRemove.isNull())
	{
		parentNode.removeChild(elementToRemove);
		elementToRemove = getNode(parentNode, nodeTagName, ModeFind);
	}
}

XmlConfigFile* xml_config_file = NULL;
