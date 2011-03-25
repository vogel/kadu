/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PLUGIN_INFO_H
#define PLUGIN_INFO_H

class PluginInfo
{
	bool IsValid;

	QString Type;
	QStringList Dependencies;
	QStringList Conflicts;
	QStringList Provides;
	QStringList Replaces;
	QString Description;
	QString Author;
	QString Version;
	bool LoadByDefault;
	bool IsPlugin;

public:
	PluginInfo(const QString &fileName);
	~PluginInfo();

	bool isValid() const { return IsValid; }

	QString type() const { return Type; }
	QStringList dependencies() const { return Dependencies; }
	QStringList conflicts() const { return Conflicts; }
	QStringList provides() const { return Provides; }
	QStringList replaces() const { return Replaces; }
	QString description() const { return Description; }
	QString author() const { return Author; }
	QString version() const { return Version; }
	bool loadByDefault() const { return LoadByDefault; }
	bool isPlugin() const { return IsPlugin; }

};

#endif // PLUGIN_INFO_H
