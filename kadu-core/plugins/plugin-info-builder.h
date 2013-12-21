/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QStringList>

class PluginInfo;

class KADUAPI PluginInfoBuilder
{

public:
	PluginInfoBuilder();

	PluginInfoBuilder & setName(const QString &name);
	PluginInfoBuilder & setDisplayName(const QString &displayName);
	PluginInfoBuilder & setCategory(const QString &category);
	PluginInfoBuilder & setType(const QString &type);
	PluginInfoBuilder & setDescription(const QString &description);
	PluginInfoBuilder & setAuthor(const QString &author);
	PluginInfoBuilder & setVersion(const QString &version);
	PluginInfoBuilder & setProvides(const QString &provides);
	PluginInfoBuilder & setDependencies(const QStringList &dependencies);
	PluginInfoBuilder & setReplaces(const QStringList &replaces);
	PluginInfoBuilder & setLoadByDefault(bool loadByDefault);

	PluginInfo create();

private:
	QString m_name;
	QString m_displayName;
	QString m_category;
	QString m_type;
	QString m_description;
	QString m_author;
	QString m_version;
	QString m_provides;
	QStringList m_dependencies;
	QStringList m_replaces;
	bool m_loadByDefault;

};
