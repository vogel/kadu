#include <qstring.h>
#include <qvaluelist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <stdio.h>

#include "config_file.h"

ConfigFile::ConfigFile(const QString &filename) : filename(filename) {
	fprintf(stderr, "KK ConfigFile::ConfigFile()\n");
	read();
	activegroup = NULL;
	fprintf(stderr, "KK ConfigFile::ConfigFile(): finished\n");
}

void ConfigFile::read() {
	QFile file(filename);
	QString line;
	struct ConfigFileGroup activegroup;
	struct ConfigFileEntry activeentry;

	fprintf(stderr, "KK ConfigFile::read()\n");
	if (file.open(IO_ReadWrite)) {
		QTextStream stream(&file);
		while ((line = stream.readLine()) != QString::null) {
			line.stripWhiteSpace();
			if (line.startsWith("[") && line.endsWith("]")) {
				if (activegroup.name != line.mid(1, line.length() - 2)) {
					if (activegroup.name.length())
						groups.append(activegroup);
					activegroup.name = line.mid(1, line.length() - 2);
					activegroup.entries.clear();
					}
				}
			else 
				if (activegroup.name.length()) {
					activeentry.name = line.section('=', 0, 0);
					activeentry.value = line.section('=', -1, -1);
					if (line.contains('=') == 1 && activeentry.name.length()
						&& activeentry.value.length())
						activegroup.entries.append(activeentry);
					}
			}
		if (activegroup.name.length())
			groups.append(activegroup);
		file.close();
		}
	fprintf(stderr, "KK ConfigFile::read(): finished\n");
}

void ConfigFile::write() {
	QFile file(filename);
	QString line;
	struct ConfigFileGroup activegroup;
	struct ConfigFileEntry activeentry;

	fprintf(stderr, "KK ConfigFile::write()\n");
	if (file.open(IO_WriteOnly | IO_Truncate)) {
		QTextStream stream(&file);
		for (int i = 0; i < groups.count(); i++) {
			stream << '[' << groups[i].name << "]\n";
			for (int j = 0; j < groups[i].entries.count(); j++)
				stream << groups[i].entries[j].name << '=' << groups[i].entries[j].value << '\n';
			stream << '\n';
			}
		file.close();
		}
	fprintf(stderr, "KK ConfigFile::write(): finished\n");
}

void ConfigFile::sync() {
	fprintf(stderr, "KK ConfigFile::sync()\n");
	write();
	fprintf(stderr, "KK ConfigFile::sync()(): finished\n");
}

void ConfigFile::setGroup(const QString &name) {
	struct ConfigFileGroup newgroup;
	int i;

	fprintf(stderr, "KK ConfigFile::setGroup()\n");
	for (i = 0; i < groups.count(); i++)
		if (name == groups[i].name)
			break;
	if (i == groups.count()) {
		newgroup.name = name;
		groups.append(newgroup);
		for (i = 0; i < groups.count(); i++)
			if (name == groups[i].name)
				break;
		}
	activegroup = &(groups[i]);
	fprintf(stderr, "KK ConfigFile::setGroup(): finished\n");
}

bool ConfigFile::changeEntry(const QString &name, const QString &value) {
	struct ConfigFileEntry newentry;
	int i;

	fprintf(stderr, "KK ConfigFile::changeEntry()\n");
	for (i = 0; i < activegroup->entries.count(); i++)
		if (activegroup->entries[i].name == name)
			break;
	if (i == activegroup->entries.count()) {
		newentry.name = name;
		newentry.value = value;
		activegroup->entries.append(newentry);
		fprintf(stderr, "KK ConfigFile::changeEntry(): finished\n");
		return false;
		}
	else
		activegroup->entries[i].value = value;
	fprintf(stderr, "KK ConfigFile::changeEntry(): finished\n");
	return true;
}

QString ConfigFile::getEntry(const QString &name, bool *ok) const {
	struct ConfigFileEntry newentry;
	int i;

	fprintf(stderr, "KK ConfigFile::getEntry()\n");
	for (i = 0; i < activegroup->entries.count(); i++)
		if (activegroup->entries[i].name == name)
			break;
	if (ok)
		*ok = (i < activegroup->entries.count());
	if (i == activegroup->entries.count()) {
		fprintf(stderr, "KK ConfigFile::getEntry(): finished\n");
		return QString::null;
		}
	fprintf(stderr, "KK ConfigFile::getEntry(): finished\n");
	return activegroup->entries[i].value;
}

void ConfigFile::writeEntry(const QString &name, const QString &value) {
	changeEntry(name, value);
}

void ConfigFile::writeEntry(const QString &name, const int value) {
	changeEntry(name, QString::number(value));
}

void ConfigFile::writeEntry(const QString &name, const double value) {
	changeEntry(name, QString::number(value, 'f'));
}

void ConfigFile::writeEntry(const QString &name, const bool value) {
	changeEntry(name, value ? "true" : "false");
}

void ConfigFile::writeEntry(const QString &name, const QRect &value) {
	QStringList string;
	string.append(QString::number(value.left()));
	string.append(QString::number(value.top()));
	string.append(QString::number(value.right()));
	string.append(QString::number(value.bottom()));
	changeEntry(name, string.join(","));
}

void ConfigFile::writeEntry(const QString &name, const QSize &value) {
	QStringList string;
	string.append(QString::number(value.width()));
	string.append(QString::number(value.height()));
	changeEntry(name, string.join(","));
}

void ConfigFile::writeEntry(const QString &name, const QColor &value) {
	QStringList string;
	string.append(QString::number(value.red()));
	string.append(QString::number(value.green()));
	string.append(QString::number(value.blue()));
	changeEntry(name, string.join(","));
}

void ConfigFile::writeEntry(const QString &name, const QFont &value) {
	QStringList string;
	string.append(value.family());
	string.append(QString::number(value.pointSize()));
	changeEntry(name, string.join(","));
}

QString ConfigFile::readEntry(const QString &name, const QString &def) const {
	QString string = getEntry(name);
	if (string == QString::null)
		return def;
	return string;
}

int ConfigFile::readNumEntry(const QString &name, int def) const {
	bool ok;
	QString string = getEntry(name);
	if (string == QString::null)
		return def;
	int num = string.toInt(&ok);
	if (!ok)
		return def;
	return num;
}

double ConfigFile::readDoubleNumEntry(const QString &name, double def) const {
	bool ok;
	QString string = getEntry(name);
	if (string == QString::null)
		return def;
	double num = string.toDouble(&ok);
	if (!ok)
		return def;
	return num;
}

bool ConfigFile::readBoolEntry(const QString &name, bool def) const {
	QString string = getEntry(name);
	if (string == QString::null)
		return def;
	if (string == "true")
		return true;
	if (string == "false")
		return false;
	return def;
}

QRect ConfigFile::readRectEntry(const QString &name, const QRect *def) const {
	QString string = getEntry(name);
	QStringList stringlist;
	QRect rect;
	int l, t, r, b;
	bool ok;

	if (string == QString::null)
		return def ? *def : QRect(0, 0, 0, 0);
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 4)
		return def ? *def : QRect(0, 0, 0, 0);
	l = stringlist[0].toInt(&ok);
	if (!ok)
		return def ? *def : QRect(0, 0, 0, 0);
	t = stringlist[1].toInt(&ok);
	if (!ok)
		return def ? *def : QRect(0, 0, 0, 0);
	r = stringlist[2].toInt(&ok);
	if (!ok)
		return def ? *def : QRect(0, 0, 0, 0);
	b = stringlist[3].toInt(&ok);
	if (!ok)
		return def ? *def : QRect(0, 0, 0, 0);
	rect.setCoords(l, t, r, b);
	return rect;
}

QSize ConfigFile::readSizeEntry(const QString &name, const QSize *def) const {
	QString string = getEntry(name);
	QStringList stringlist;
	QSize size;
	int w, h;
	bool ok;

	if (string == QString::null)
		return def ? *def : QSize(0, 0);
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 2)
		return def ? *def : QSize(0, 0);
	w = stringlist[0].toInt(&ok);
	if (!ok)
		return def ? *def : QSize(0, 0);
	h = stringlist[1].toInt(&ok);
	if (!ok)
		return def ? *def : QSize(0, 0);
	size.setWidth(w);
	size.setHeight(h);
	return size;
}

QColor ConfigFile::readColorEntry(const QString &name, const QColor *def) const {
	QString string = getEntry(name);
	QStringList stringlist;
	QColor color;
	int r, g, b;
	bool ok;

	if (string == QString::null)
		return def ? *def : QColor(0, 0, 0);
	stringlist = QStringList::split(",", string);
	if (stringlist.count() != 3)
		return def ? *def : QColor(0, 0, 0);
	r = stringlist[0].toInt(&ok);
	if (!ok)
		return def ? *def : QColor(0, 0, 0);
	g = stringlist[1].toInt(&ok);
	if (!ok)
		return def ? *def : QColor(0, 0, 0);
	b = stringlist[2].toInt(&ok);
	if (!ok)
		return def ? *def : QColor(0, 0, 0);
	color.setRgb(r, g, b);
	return color;
}

QFont ConfigFile::readFontEntry(const QString &name, const QFont *def) const {
	QString string = getEntry(name);
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

