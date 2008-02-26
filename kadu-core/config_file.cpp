/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <q3textstream.h>
#include <qfile.h>
#include <qmutex.h>

#include "debug.h"
#include "config_file.h"
#include "misc.h"

//rename
#include <stdio.h>
//strerror
#include <string.h>
#include <errno.h>

QMutex GlobalMutex;

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

	CONST_FOREACH(fileName, files)
	{
		file.setName(ggPath(*fileName));
		fileOpened = file.open(QIODevice::ReadOnly);
		if (fileOpened && file.size() > 0)
		{
			kdebugm(KDEBUG_INFO, "file %s opened!\n", file.name().local8Bit().data());
			break;
		}
		if (fileOpened) // && file.size() == 0
		{
			kdebugm(KDEBUG_INFO, "config file (%s) is empty, looking for backup\n", file.name().local8Bit().data());
			file.close();
			fileOpened = false;
		}
		else
			kdebugm(KDEBUG_INFO, "config file (%s) not opened, looking for backup\n", file.name().local8Bit().data());
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
		fprintf(stderr, "error opening xml configuration file (%s), creating empty document\n", file.errorString().local8Bit().data());
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
		kdebugm(KDEBUG_INFO, "file opened '%s'\n", file.name().local8Bit().data());
		Q3TextStream stream(&file);
		stream.setEncoding(Q3TextStream::UnicodeUTF8);
		stream << DomDocument.toString();
		file.close();
		if (rename(tmpFileName.local8Bit().data(), fileName.local8Bit().data()) == -1)
		{
			fprintf(stderr, "cannot rename '%s' to '%s': %s\n", tmpFileName.local8Bit().data(), fileName.local8Bit().data(), strerror(errno));
			fflush(stderr);
		}
	}
	else
	{
		fprintf(stderr, "cannot open '%s': %s\n", file.name().local8Bit().data(), file.errorString().local8Bit().data());
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
//			val.local8Bit().data(), property_value.local8Bit().data());
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

XmlConfigFile* xml_config_file = NULL;



PlainConfigFile::PlainConfigFile(const QString &filename) : filename(filename), groups(), activeGroupName(), activeGroup(0)
{
	read();
}

PlainConfigFile::PlainConfigFile(const PlainConfigFile &c) : filename(c.filename), groups(c.groups), activeGroupName(), activeGroup(0)
{
}

PlainConfigFile &PlainConfigFile::operator=(const PlainConfigFile &c)
{
	filename = c.filename;
	groups = c.groups;
	activeGroupName = QString::null;
	activeGroup = 0;
	return *this;
}

void PlainConfigFile::read()
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", filename.local8Bit().data());
	QFile file(filename);
	QString line;

	if (file.open(QIODevice::ReadOnly))
	{
		Q3TextStream stream(&file);
		stream.setCodec(codec_latin2);
		while (!stream.atEnd())
		{
			line = stream.readLine();
			line.stripWhiteSpace();
			if (line.startsWith("[") && line.endsWith("]"))
			{
				QString name=line.mid(1, line.length() - 2).stripWhiteSpace();
				if (activeGroupName!=name)
				{
					activeGroupName=name;
					activeGroup=&groups[name];
				}
			}
			else if (activeGroupName.length())
			{
				QString name = line.section('=', 0, 0);
				QString value = line.right(line.length()-name.length()-1).replace("\\n", "\n");
				name = name.stripWhiteSpace();

				if (line.contains('=') && !name.isEmpty() && !value.isEmpty())
					(*activeGroup)[name]=value;
			}
		}
		file.close();
	}
	kdebugf2();
}

//#include <sys/time.h>
void PlainConfigFile::write() const
{
	kdebugf();

/*	struct timeval t1,t2;
	gettimeofday(&t1, NULL);
	for(int j=0; j<100; ++j)
	{*/

	QFile file(filename);
	QString line;
	QStringList out;
	QString format1("[%1]\n");
	QString format2("%1=%2\n");

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		kdebugm(KDEBUG_INFO, "file opened '%s'\n", (const char *)file.name().local8Bit());
		Q3TextStream stream(&file);
		stream.setCodec(codec_latin2);
		CONST_FOREACH(i, groups)
		{
//			kdebugm(KDEBUG_DUMP, ">> %s\n", (const char*)i.key().local8Bit());
			out.append(format1.arg(i.key()));
			CONST_FOREACH(j, i.data())
			{
				QString q = j.data();
				out.append(format2.arg(j.key()).arg(q.replace('\n', "\\n")));
//				kdebugm(KDEBUG_DUMP, ">>>>> %s %s\n", (const char*)j.key().local8Bit(), (const char*)q.local8Bit());
			}
			out.append("\n");
		}
		stream << out.join(QString::null);
		file.close();
	}
	else
	{
		fprintf(stderr, "cannot open '%s': %s\n", file.name().local8Bit().data(), file.errorString().local8Bit().data());
		fflush(stderr);
	}

/*	}
	gettimeofday(&t2, NULL);
	kdebugm(KDEBUG_INFO, "czas: %ld\n", (t2.tv_usec-t1.tv_usec)+(t2.tv_sec*1000000)-(t1.tv_sec*1000000));
*/
	kdebugf2();
}

QStringList PlainConfigFile::getGroupList() const
{
	return QStringList(groups.keys());
}

void PlainConfigFile::sync() const
{
	write();
}

QMap<QString, QString>& PlainConfigFile::getGroupSection(const QString& name)
{
	kdebugf();
	return groups[name];
}

bool PlainConfigFile::changeEntry(const QString &group, const QString &name, const QString &value)
{
//	kdebugm(KDEBUG_FUNCTION_START, "PlainConfigFile::changeEntry(%s, %s, %s) %p\n", (const char *)group.local8Bit(), (const char *)name.local8Bit(), (const char *)value.local8Bit(), this);
	if (activeGroupName!=group)
	{
		activeGroupName=group;
		activeGroup=&(groups[group]);
	}
	bool ret=activeGroup->contains(name);
	(*activeGroup)[name]=value;
	//
	return ret;
}

QString PlainConfigFile::getEntry(const QString &group, const QString &name, bool *ok) const
{
//	kdebugm(KDEBUG_FUNCTION_START, "PlainConfigFile::getEntry(%s, %s) %p\n", (const char *)group.local8Bit(), (const char *)name.local8Bit(), this);
	if (activeGroupName!=group)
	{
		if (!groups.contains(group))
		{
			if (ok)
				*ok=false;
			return QString::null;
		}
		activeGroupName=group;
		activeGroup=&((QMap<QString, QString>)groups[group]);
	}
	if (ok)
		*ok=activeGroup->contains(name);
	if (activeGroup->contains(name))
		return (*activeGroup)[name];
	else
		return QString::null;
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const QString &value)
{
	changeEntry(group, name, value);
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const char *value)
{
	changeEntry(group, name, QString::fromLocal8Bit(value));
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const int value)
{
	changeEntry(group, name, QString::number(value));
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const double value)
{
	changeEntry(group, name, QString::number(value, 'f'));
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const bool value)
{
	changeEntry(group, name, value ? "true" : "false");
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const QRect &value)
{
	changeEntry(group, name, QString("%1,%2,%3,%4").arg(value.left()).arg(value.top()).
				arg(value.width()).arg(value.height()));
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const QSize &value)
{
	changeEntry(group, name, QString("%1,%2").arg(value.width()).arg(value.height()));
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const QColor &value)
{
	changeEntry(group, name, value.name());
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const QFont &value)
{
	changeEntry(group, name, value.family() + ',' + QString::number(value.pointSize()));
}

void PlainConfigFile::writeEntry(const QString &group,const QString &name, const QPoint &value)
{
	changeEntry(group, name, QString("%1,%2").arg(value.x()).arg(value.y()));
}

QString PlainConfigFile::readEntry(const QString &group,const QString &name, const QString &def) const
{
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	return string;
}

unsigned int PlainConfigFile::readUnsignedNumEntry(const QString &group,const QString &name, unsigned int def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	unsigned int num = string.toUInt(&ok);
	if (!ok)
		return def;
	return num;
}

int PlainConfigFile::readNumEntry(const QString &group,const QString &name, int def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	int num = string.toInt(&ok);
	if (!ok)
		return def;
	return num;
}

double PlainConfigFile::readDoubleNumEntry(const QString &group,const QString &name, double def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	double num = string.toDouble(&ok);
	if (!ok)
		return def;
	return num;
}

bool PlainConfigFile::readBoolEntry(const QString &group,const QString &name, bool def) const
{
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	return string=="true";
}

QRect PlainConfigFile::readRectEntry(const QString &group,const QString &name, const QRect *def) const
{
	QString string = getEntry(group, name);
	QStringList stringlist;
	QRect rect(0,0,0,0);
	int l, t, w, h;
	bool ok;

	if (string == QString::null)
		return def ? *def : rect;
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 4)
		return def ? *def : rect;
	l = stringlist[0].toInt(&ok); if (!ok) return def ? *def : rect;
	t = stringlist[1].toInt(&ok); if (!ok) return def ? *def : rect;
	w = stringlist[2].toInt(&ok); if (!ok) return def ? *def : rect;
	h = stringlist[3].toInt(&ok); if (!ok) return def ? *def : rect;
	rect.setRect(l, t, w, h);
	return rect;
}

QSize PlainConfigFile::readSizeEntry(const QString &group,const QString &name, const QSize *def) const
{
	QString string = getEntry(group, name);
	QStringList stringlist;
	QSize size(0,0);
	int w, h;
	bool ok;

	if (string == QString::null)
		return def ? *def : size;
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 2)
		return def ? *def : size;
	w = stringlist[0].toInt(&ok); if (!ok) return def ? *def : size;
	h = stringlist[1].toInt(&ok); if (!ok) return def ? *def : size;
	size.setWidth(w);
	size.setHeight(h);
	return size;
}

QColor PlainConfigFile::readColorEntry(const QString &group,const QString &name, const QColor *def) const
{
	QColor col(0,0,0);
	QString str = getEntry(group, name);
	if (str==QString::null)
		return def ? *def : col;
	else
	{
		if (!str.contains(','))
			return QColor(str);

		//stary zapis kolorów, w 0.5.0 mo¿na bêdzie wywaliæ
		bool ok;
		QStringList stringlist = QStringList::split(",", str);
		if (stringlist.count() != 3)
			return def ? *def : col;
		int r = stringlist[0].toInt(&ok); if (!ok) return def ? *def : col;
		int g = stringlist[1].toInt(&ok); if (!ok) return def ? *def : col;
		int b = stringlist[2].toInt(&ok); if (!ok) return def ? *def : col;
		col.setRgb(r, g, b);
		return col;
	}
}


QFont PlainConfigFile::readFontEntry(const QString &group,const QString &name, const QFont *def) const
{
	QString string = getEntry(group, name);
	QStringList stringlist;
	QFont font;
	bool ok;

	if (string == QString::null)
		return def ? *def : QApplication::font();
	stringlist = QStringList::split(",", string);
	if (stringlist.count() < 2)
		return def ? *def : QApplication::font();
	font.setFamily(stringlist[0]);
	font.setPointSize(stringlist[1].toInt(&ok));
	if (!ok)
		return def ? *def : QApplication::font();
	return font;
}

void PlainConfigFile::removeVariable(const QString &group, const QString &name)
{
	if (activeGroupName != group)
	{
		activeGroupName = group;
		activeGroup = &(groups[group]);
	}

	if (activeGroup->contains(name))
		activeGroup->remove(name);
}

QPoint PlainConfigFile::readPointEntry(const QString &group,const QString &name, const QPoint *def) const
{
	QString string = getEntry(group, name);
	QStringList stringlist;
	QPoint point(0,0);
	int x, y;
	bool ok;

	if (string == QString::null)
		return def ? *def : point;
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 2)
		return def ? *def : point;
	x = stringlist[0].toInt(&ok); if (!ok) return def ? *def : point;
	y = stringlist[1].toInt(&ok); if (!ok) return def ? *def : point;
	point.setX(x);
	point.setY(y);
	return point;
}

void PlainConfigFile::addVariable(const QString &group, const QString &name, const QString &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const char *defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const int defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const double defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const bool defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const QRect &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const QSize &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const QColor &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const QFont &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void PlainConfigFile::addVariable(const QString &group, const QString &name, const QPoint &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}




ConfigFile::ConfigFile(const QString &filename) : filename(filename)
{
}

void ConfigFile::sync() const
{
	xml_config_file->sync();
}

bool ConfigFile::changeEntry(const QString &group, const QString &name, const QString &value)
{
	GlobalMutex.lock();

//	kdebugm(KDEBUG_FUNCTION_START, "ConfigFile::changeEntry(%s, %s, %s) %p\n", group.local8Bit().data(), name.local8Bit().data(), value.local8Bit().data(), this);
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", filename.section("/", -1));
	QDomElement group_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", group);
	QDomElement entry_elem = xml_config_file->accessElementByProperty(
		group_elem, "Entry", "name", name);
	entry_elem.setAttribute("value", value);

	GlobalMutex.unlock();

	return true;
}

QString ConfigFile::getEntry(const QString &group, const QString &name, bool *ok) const
{
	GlobalMutex.lock();

	bool resOk;
	QString result = QString::null;

//	kdebugm(KDEBUG_FUNCTION_START, "ConfigFile::getEntry(%s, %s) %p\n", group.local8Bit().data(), name.local8Bit().data(), this);
	{
		QDomElement root_elem = xml_config_file->rootElement();
		QDomElement deprecated_elem = xml_config_file->findElement(root_elem, "Deprecated");
		if (!deprecated_elem.isNull())
		{
			QDomElement config_file_elem = xml_config_file->findElementByProperty(
				deprecated_elem, "ConfigFile", "name", filename.section("/", -1));
			if (!config_file_elem.isNull())
			{
				QDomElement group_elem = xml_config_file->findElementByProperty(
					config_file_elem, "Group", "name", group);
				if (!group_elem.isNull())
				{
					QDomElement entry_elem =
						xml_config_file->findElementByProperty(
							group_elem, "Entry", "name", name);
					if (!entry_elem.isNull())
					{
						resOk = true;
						result = entry_elem.attribute("value");
					}
				}
			}
		}
	}

	resOk = false;
	if (ok)
		*ok = resOk;

	GlobalMutex.unlock();
	return result;
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QString &value)
{
	changeEntry(group, name, value);
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const char *value)
{
	changeEntry(group, name, QString::fromLocal8Bit(value));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const int value)
{
	changeEntry(group, name, QString::number(value));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const double value)
{
	changeEntry(group, name, QString::number(value, 'f'));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const bool value)
{
	changeEntry(group, name, value ? "true" : "false");
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QRect &value)
{
	changeEntry(group, name, QString("%1,%2,%3,%4").arg(value.left()).arg(value.top()).
				arg(value.width()).arg(value.height()));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QSize &value)
{
	changeEntry(group, name, QString("%1,%2").arg(value.width()).arg(value.height()));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QColor &value)
{
	changeEntry(group, name, value.name());
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QFont &value)
{
	changeEntry(group, name, value.toString());
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QPoint &value)
{
	changeEntry(group, name, QString("%1,%2").arg(value.x()).arg(value.y()));
}

QString ConfigFile::readEntry(const QString &group,const QString &name, const QString &def) const
{
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	return string;
}

unsigned int ConfigFile::readUnsignedNumEntry(const QString &group,const QString &name, unsigned int def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	unsigned int num = string.toUInt(&ok);
	if (!ok)
		return def;
	return num;
}

int ConfigFile::readNumEntry(const QString &group,const QString &name, int def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	int num = string.toInt(&ok);
	if (!ok)
		return def;
	return num;
}

double ConfigFile::readDoubleNumEntry(const QString &group,const QString &name, double def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	double num = string.toDouble(&ok);
	if (!ok)
		return def;
	return num;
}

bool ConfigFile::readBoolEntry(const QString &group,const QString &name, bool def) const
{
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def;
	return string=="true";
}

QRect ConfigFile::readRectEntry(const QString &group,const QString &name, const QRect *def) const
{
	QString string = getEntry(group, name);
	QStringList stringlist;
	QRect rect(0,0,0,0);
	int l, t, w, h;
	bool ok;

	if (string == QString::null)
		return def ? *def : rect;
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 4)
		return def ? *def : rect;
	l = stringlist[0].toInt(&ok); if (!ok) return def ? *def : rect;
	t = stringlist[1].toInt(&ok); if (!ok) return def ? *def : rect;
	w = stringlist[2].toInt(&ok); if (!ok) return def ? *def : rect;
	h = stringlist[3].toInt(&ok); if (!ok) return def ? *def : rect;
	rect.setRect(l, t, w, h);
	return rect;
}

QSize ConfigFile::readSizeEntry(const QString &group,const QString &name, const QSize *def) const
{
	QString string = getEntry(group, name);
	QStringList stringlist;
	QSize size(0,0);
	int w, h;
	bool ok;

	if (string == QString::null)
		return def ? *def : size;
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 2)
		return def ? *def : size;
	w = stringlist[0].toInt(&ok); if (!ok) return def ? *def : size;
	h = stringlist[1].toInt(&ok); if (!ok) return def ? *def : size;
	size.setWidth(w);
	size.setHeight(h);
	return size;
}

QColor ConfigFile::readColorEntry(const QString &group,const QString &name, const QColor *def) const
{
	QColor col(0,0,0);
	QString str = getEntry(group, name);
	if (str==QString::null)
		return def ? *def : col;
	else
	{
		if (!str.contains(','))
			return QColor(str);

		//stary zapis kolorów, w 0.5.0 mo¿na bêdzie wywaliæ
		bool ok;
		QStringList stringlist = QStringList::split(",", str);
		if (stringlist.count() != 3)
			return def ? *def : col;
		int r = stringlist[0].toInt(&ok); if (!ok) return def ? *def : col;
		int g = stringlist[1].toInt(&ok); if (!ok) return def ? *def : col;
		int b = stringlist[2].toInt(&ok); if (!ok) return def ? *def : col;
		col.setRgb(r, g, b);
		return col;
	}
}


QFont ConfigFile::readFontEntry(const QString &group,const QString &name, const QFont *def) const
{
	QString string = getEntry(group, name);
	if (string == QString::null)
		return def ? *def : QApplication::font();
	QFont font;
	if(font.fromString(string))
		return font;
	return def ? *def : QApplication::font();
}

QPoint ConfigFile::readPointEntry(const QString &group,const QString &name, const QPoint *def) const
{
	QString string = getEntry(group, name);
	QStringList stringlist;
	QPoint point(0,0);
	int x, y;
	bool ok;

	if (string == QString::null)
		return def ? *def : point;
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 2)
		return def ? *def : point;
	x = stringlist[0].toInt(&ok); if (!ok) return def ? *def : point;
	y = stringlist[1].toInt(&ok); if (!ok) return def ? *def : point;
	point.setX(x);
	point.setY(y);
	return point;
}

void ConfigFile::removeVariable(const QString &group, const QString &name)
{
	GlobalMutex.lock();

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", filename.section("/", -1));
	QDomElement group_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", group);
	QDomElement entry_elem = xml_config_file->accessElementByProperty(
		group_elem, "Entry", "name", name);
	group_elem.removeChild(entry_elem);

	GlobalMutex.unlock();
}

void ConfigFile::addVariable(const QString &group, const QString &name, const QString &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const char *defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const int defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const double defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const bool defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QRect &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QSize &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QColor &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QFont &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QPoint &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
