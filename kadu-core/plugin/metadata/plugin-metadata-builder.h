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

class PluginMetadata;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginMetadataBuilder
 * @short Builder of PluginMetadata objects.
 *
 * This class allows for building PluginMetadata objects without directly calling its big constructor.
 * Each setXXX call affects all PluginMetadata objects created after it with create() method.
 */
class KADUAPI PluginMetadataBuilder
{

public:
	PluginMetadataBuilder();

	PluginMetadataBuilder & setName(const QString &name);
	PluginMetadataBuilder & setDisplayName(const QString &displayName);
	PluginMetadataBuilder & setCategory(const QString &category);
	PluginMetadataBuilder & setType(const QString &type);
	PluginMetadataBuilder & setDescription(const QString &description);
	PluginMetadataBuilder & setAuthor(const QString &author);
	PluginMetadataBuilder & setVersion(const QString &version);
	PluginMetadataBuilder & setProvides(const QString &provides);
	PluginMetadataBuilder & setDependencies(const QStringList &dependencies);
	PluginMetadataBuilder & setReplaces(const QStringList &replaces);
	PluginMetadataBuilder & setLoadByDefault(bool loadByDefault);

	/**
	 * @return New PluginMetadata object with properties set with setXXX calls.
	 */
	PluginMetadata create();

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

/**
 * @}
 */
