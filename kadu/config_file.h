/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qfont.h>

struct ConfigFileEntry {
	QString name;
	QString value;
};
	
struct ConfigFileGroup {
	QString name;
	QValueList<ConfigFileEntry> entries;
};

class ConfigFile {
	public:
		ConfigFile(const QString &filename);
		void sync();
		void setGroup(const QString &name);
		void writeEntry(const QString &name, const QString &value);
		void writeEntry(const QString &name, const char *value);
		void writeEntry(const QString &name, const int value);
		void writeEntry(const QString &name, const double value);
		void writeEntry(const QString &name, const bool value);
		void writeEntry(const QString &name, const QRect &value);
		void writeEntry(const QString &name, const QSize &value);
		void writeEntry(const QString &name, const QColor &value);
		void writeEntry(const QString &name, const QFont &value);

		QString readEntry(const QString &name, const QString &def = QString::null) const;
//		char *readEntry(const QString &name, const char *value = NULL) const;
		int readNumEntry(const QString &name, int def = 0) const;
		double readDoubleNumEntry(const QString &name, double def = 0.0) const;
		bool readBoolEntry(const QString &name, bool def = false) const;
		QRect readRectEntry(const QString &name, const QRect *def = 0L) const;
		QSize readSizeEntry(const QString &name, const QSize *def = 0L) const;
		QColor readColorEntry(const QString &name, const QColor *def = 0L) const;
		QFont readFontEntry(const QString &name, const QFont *def = 0L) const;

	private:
		void read();
		void write();
		bool changeEntry(const QString &name, const QString &value);
		QString getEntry(const QString &name, bool *ok = 0) const;

		QString filename;
		QValueList<ConfigFileGroup> groups;
		struct ConfigFileGroup *activegroup;
};

#endif
