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

#pragma once

#include "exports.h"

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QFont>

class ConfigurationApi;

class KADUAPI DeprecatedConfigurationApi final
{
	bool changeEntry(const QString &group, const QString &name, const QString &value);
	QString getEntry(const QString &group, const QString &name) const;

	ConfigurationApi *m_xmlConfigFile;
	QString m_fileName;

public:
	DeprecatedConfigurationApi(ConfigurationApi *xmlConfigFile, const QString &fileName);

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
