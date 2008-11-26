/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>

#include <errno.h>

#include "debug.h"
#include "misc.h"

#include "xml_config_file.h"

XmlConfigFile::XmlConfigFile() : DomDocument()
{
	read();
}

void XmlConfigFile::read()
{
	kdebugf();
	QFile file;
	QDir backups(ggPath(), "kadu.conf.xml.backup.*", QDir::Name, QDir::Files);
	QStringList files("kadu.conf.xml");
	files += backups.entryList();
	bool fileOpened(false);

	foreach(const QString &fileName, files)
	{
		file.setName(ggPath(fileName));
		fileOpened = file.open(QIODevice::ReadOnly);
		if (fileOpened && file.size() > 0)
		{
			kdebugm(KDEBUG_INFO, "file %s opened!\n", qPrintable(file.name()));
			break;
		}
		if (fileOpened) // && file.size() == 0
		{
			kdebugm(KDEBUG_INFO, "config file (%s) is empty, looking for backup\n", qPrintable(file.name()));
			file.close();
			fileOpened = false;
		}
		else
			kdebugm(KDEBUG_INFO, "config file (%s) not opened, looking for backup\n", qPrintable(file.name()));
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
	rootElement().setAttribute("last_save_version", VERSION);
	QFile file;
	QString fileName, tmpFileName;
	if (f.isEmpty())
		fileName = ggPath("kadu.conf.xml");
	else
		fileName = f;
	tmpFileName = fileName + ".tmp"; // saving to another file to avoid truncation of output file when segfault occurs :|
	file.setName(tmpFileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		kdebugm(KDEBUG_INFO, "file opened '%s'\n", qPrintable(file.name()));
		QTextStream stream(&file);
		stream.setEncoding(QTextStream::UnicodeUTF8);
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
		fprintf(stderr, "cannot open '%s': %s\n", qPrintable(file.name()), qPrintable(file.errorString()));
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
	QString f = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
	write(ggPath(f));
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

QDomElement XmlConfigFile::findElement(QDomElement parent, const QString& tag_name) const
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

QDomElement XmlConfigFile::findElementByProperty(QDomElement parent, const QString& tag_name,
	const QString& property_name, const QString& property_value) const
{
//	kdebugf();
	for (QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &e = n.toElement();
		if (e.isNull())
			continue;
		if (e.tagName() != tag_name)
			continue;
		const QString &val = e.attribute(property_name);
//		kdebug("Checking if property value \"%s\" equals \"%s\"\n",
//			qPrintable(val), qPrintable(property_value));
		if (val == property_value)
		{
//			kdebug("Element found.\n");
//			kdebugf2();
			return e;
		}
	}
//	kdebug("Element not found.\n");
//	kdebugf2();
	return QDomNode().toElement();
}

QDomElement XmlConfigFile::accessElement(QDomElement parent, const QString& tag_name)
{
	const QDomElement &elem = findElement(parent, tag_name);
	if (elem.isNull())
		return createElement(parent, tag_name);
	else
		return elem;
}

QDomElement XmlConfigFile::accessElementByProperty(QDomElement parent, const QString& tag_name,
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

void XmlConfigFile::removeChildren(QDomElement parent)
{
	while (parent.hasChildNodes())
	{
		parent.firstChild().clear();
		parent.removeChild(parent.firstChild());
	}
}

void XmlConfigFile::removeNodes(QDomElement parentNode, QDomNodeList nodes)
{
	int count = nodes.count();
	for (int i = 0; i < count; ++i)
		parentNode.removeChild(nodes.item(i));
}

void XmlConfigFile::removeNamedNodes(QDomElement parentNode, QDomNodeList nodes, const QString &name)
{
	int count = nodes.count();
	for (int i = 0; i < count; ++i)
		if (isElementNamed(nodes.item(i).toElement(), name))
			parentNode.removeChild(nodes.item(i));
}

bool XmlConfigFile::isElementNamed(const QDomElement &element, const QString &name)
{
	return element.hasAttribute("name") && name == element.attribute("name");
}

bool XmlConfigFile::hasNode(const QString &nodeTagName)
{
	return !getNode(nodeTagName, ModeFind).isNull();
}

bool XmlConfigFile::hasNode(QDomElement parentNode, const QString &nodeTagName)
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

QDomElement XmlConfigFile::getNode(QDomElement parentNode, const QString &nodeTagName, GetNodeMode getMode)
{
	QDomElement result;
	QDomNodeList nodes = parentNode.elementsByTagName(nodeTagName);

	if (ModeCreate == getMode)
		removeNodes(parentNode, nodes);
	else if (!nodes.isEmpty())
		return nodes.item(0).toElement();

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
	QDomNodeList nodes = parentNode.elementsByTagName(nodeTagName);

	if (ModeCreate == getMode)
		removeNamedNodes(parentNode, nodes, nodeName);

	int count = nodes.count();
	for (int i = 0; i < count; ++i)
	{
		QDomElement element = nodes.item(i).toElement();
		if (element.isNull())
			continue;
		if (isElementNamed(element, nodeName))
			return element;
	}

	if (ModeFind != getMode)
	{
		result = DomDocument.createElement(nodeTagName);
		result.setAttribute("name", nodeName);
		parentNode.appendChild(result);
	}

	return result;
}

QDomNodeList XmlConfigFile::getNodes(QDomElement parent, const QString &nodeTagName)
{
	return parent.elementsByTagName(nodeTagName);
}

void XmlConfigFile::createTextNode(QDomElement parentNode, const QString &nodeTagName, const QString &nodeContent)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeCreate);
	element.appendChild(DomDocument.createTextNode(nodeContent));
}

QString XmlConfigFile::getTextNode(QDomElement parentNode, const QString &nodeTagName)
{
	QDomElement element = getNode(parentNode, nodeTagName, ModeFind);
	if (element.isNull())
		return "";

	return element.text();
}

XmlConfigFile* xml_config_file = NULL;
