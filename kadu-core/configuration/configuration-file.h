/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <QtCore/QMap>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtXml/QDomDocument>

#include "exports.h"

class KADUAPI PlainConfigFile
{
	void write() const;
	bool changeEntry(const QString &group, const QString &name, const QString &value);
	QString getEntry(const QString &group, const QString &name, bool *ok = 0);

	QString filename;
	QString CodecName;
	QMap<QString, QMap<QString, QString> > groups;
	mutable QString activeGroupName;
	mutable QMap<QString, QString> activeGroup;
	PlainConfigFile(const PlainConfigFile &);
	PlainConfigFile &operator = (const PlainConfigFile &);

	void changeActiveGroup(const QString &newGroup);

public:
	PlainConfigFile(const QString &filename, const QString &codec = "ISO8859-2");

	void read();

	void sync();

	QMap<QString, QString> & getGroupSection(const QString &name);

	QStringList getGroupList() const;

	void writeEntry(const QString &group,const QString &name, const QString &value);

	QString readEntry(const QString &group, const QString &name, const QString &def = QString());
	bool readBoolEntry(const QString &group, const QString &name, bool def = false);

};

class KADUAPI ConfigFile
{
	bool changeEntry(const QString &group, const QString &name, const QString &value);
	QString getEntry(const QString &group, const QString &name) const;

	QString filename;

public:
	ConfigFile(const QString &filename);

	void writeEntry(const QString &group, const QString &name, const QString &value);
	void writeEntry(const QString &group, const QString &name, const char *value);
	void writeEntry(const QString &group, const QString &name, const int value);
	void writeEntry(const QString &group, const QString &name, const bool value);
	void writeEntry(const QString &group, const QString &name, const QRect &value);
	void writeEntry(const QString &group, const QString &name, const QColor &value);
	void writeEntry(const QString &group, const QString &name, const QFont &value);

	QString readEntry(const QString &group, const QString &name, const QString &def = QString()) const;
	int readNumEntry(const QString &group, const QString &name, int def = 0) const;
	unsigned int readUnsignedNumEntry(const QString &group, const QString &name, unsigned int def = 0) const;
	bool readBoolEntry(const QString &group, const QString &name, bool def = false) const;
	QRect readRectEntry(const QString &group, const QString &name, const QRect *def = 0L) const;
	QColor readColorEntry(const QString &group, const QString &name, const QColor *def = 0L) const;
	QFont readFontEntry(const QString &group, const QString &name, const QFont *def = 0L) const;

	void removeVariable(const QString &group, const QString &name);

	void addVariable(const QString &group, const QString &name, const QString &value);
	void addVariable(const QString &group, const QString &name, const char *value);
	void addVariable(const QString &group, const QString &name, const int value);
	void addVariable(const QString &group, const QString &name, const bool value);
	void addVariable(const QString &group, const QString &name, const QColor &value);
	void addVariable(const QString &group, const QString &name, const QFont &value);

};

extern KADUAPI ConfigFile *config_file;

#endif
