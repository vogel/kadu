#include <qstring.h>
#include <qvaluelist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qfont.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <qtextcodec.h>
#include <qmap.h>

#include "debug.h"
#include "config_file.h"

template<class T, class X> QValueList<T> keys(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<T> ret;
    for(QMap<T,X>::const_iterator it=m.begin(); it!=m.end(); it++)
        ret.append(it.key());
    return ret;
#else
    return m.keys();
#endif
}

ConfigFile::ConfigFile(const QString &filename) : filename(filename),activeGroup(NULL)
{
	read();
}

void ConfigFile::read()
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", filename.local8Bit().data());
	QFile file(filename);
	QString line;

	if (file.open(IO_ReadOnly))
	{
		QTextStream stream(&file);
		stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
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
				QString value=line.right(line.length()-name.length()-1).replace(QRegExp("\\\\n"), "\n");
				name = name.stripWhiteSpace();
				if (line.contains('=') >= 1 && name.length() && value.length())
					(*activeGroup)[name]=value;
			}
		}
		file.close();
	}
	kdebugf2();
}

void ConfigFile::write(const QString &f) const
{
	kdebugf();
	QFile file;
	if (f==QString::null)
		file.setName(filename);
	else
		file.setName(f);
	QString line;

	if (file.open(IO_WriteOnly | IO_Truncate))
	{
		kdebugm(KDEBUG_INFO, "file opened '%s'\n", (const char *)file.name().local8Bit());
		QTextStream stream(&file);
		stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
		for(QMap<QString, QMap<QString, QString> >::const_iterator i=groups.begin(); i!=groups.end(); ++i)
		{
//			kdebugm(KDEBUG_DUMP, ">> %s\n", (const char*)i.key().local8Bit());
			stream << '[' << i.key() << "]\n";
			for(QMap<QString, QString>::const_iterator j=i.data().begin(); j!=i.data().end(); ++j)
			{
				QString q=j.data();
				stream << j.key() << '=' << q.replace(QRegExp("\n"), "\\n") << '\n';
//				kdebugm(KDEBUG_DUMP, ">>>>> %s %s\n", (const char*)j.key().local8Bit(), (const char*)q.local8Bit());
			}
			stream << '\n';
		}
		file.close();
	}
	else
		kdebugm(KDEBUG_ERROR, "can't open '%s'\n", (const char *)file.name().local8Bit());
	kdebugf2();
}

QStringList ConfigFile::getGroupList() const
{
	return QStringList(keys(groups));
//	return QStringList(groups.keys());
}

void ConfigFile::sync() const
{
	write();
}

void ConfigFile::saveTo(const QString &f) const
{
	write(f);
}

QMap<QString, QString>& ConfigFile::getGroupSection(const QString& name)
{
	kdebugf();
	return groups[name];
}

bool ConfigFile::changeEntry(const QString &group, const QString &name, const QString &value)
{
//	kdebugm(KDEBUG_FUNCTION_START, "ConfigFile::changeEntry(%s, %s, %s) %p\n", (const char *)group.local8Bit(), (const char *)name.local8Bit(), (const char *)value.local8Bit(), this);
	if (activeGroupName!=group)
	{
		activeGroupName=group;
		activeGroup=&(groups[group]);
	}
	bool ret=activeGroup->contains(name);
	(*activeGroup)[name]=value;
	return ret;
}

QString ConfigFile::getEntry(const QString &group, const QString &name, bool *ok) const
{
//	kdebugm(KDEBUG_FUNCTION_START, "ConfigFile::getEntry(%s, %s) %p\n", (const char *)group.local8Bit(), (const char *)name.local8Bit(), this);
	if (activeGroupName!=group)
	{
		if (!groups.contains(group))
		{
			if (ok)
				*ok=false;
			return QString::null;
		}
		activeGroupName=group;
		activeGroup=&((QMap<QString, QString>&)groups[group]);
	}
	if (ok)
		*ok=activeGroup->contains(name);
	if (activeGroup->contains(name))
		return (*activeGroup)[name];
	else
		return QString::null;
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
	changeEntry(group, name, value.family()+","+QString::number(value.pointSize()));
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

void ConfigFile::addVariable(const QString &group, const QString &name, const QString &defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const char *defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const int defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const double defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const bool defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QRect &defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QSize &defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QColor &defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QFont &defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QPoint &defvalue)
{
	if (getEntry(group, name)=="")
		writeEntry(group,name,defvalue);
}
