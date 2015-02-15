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

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QStringList>

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginMetadata
 * @author Rafał 'Vogel' Malinowski
 * @short Information about plugin loaded from .desc files.
 * @todo Rewrite to use .desktop files
 *
 * This class contains information about an installed plugin. Every installed plugin has
 * associated .desc file that contains info like plugin author, version or dependencies.
 */
class KADUAPI PluginMetadata
{

public:
	PluginMetadata();
	PluginMetadata(
			QString name, QString displayName, QString category, QString type, QString description, QString author, QString version,
			QString provides, QStringList dependencies, QStringList replaces, bool loadByDefault);
	~PluginMetadata();

	/**
	 * @return Unique name of plugin.
	 */
	QString name() const;

	/**
	 * @return Translated user-visible name of plugin.
	 */
	QString displayName() const;

	/**
	 * @return Translated user-visible name of category of plugin.
	 *
	 * Categories can be used to sort plugins in widgets.
	 */
	QString category() const;

	/**
	 * @return Type of plugin.
	 *
	 * Type of plugin can be empty. Kadu supports only one non-empty plugin type:
	 * Protocol. Plugins with type Protocol are activated before all other plugins.
	 */
	QString type() const;

	/**
	 * @short Translated user-visible description of plugin.
	 */
	QString description() const;

	/**
	 * @return Name of plugin's author.
	 */
	QString author() const;

	/**
	 * @return Plugin's version.
	 */
	QString version() const;

	/**
	 * @return Unique feature name of plugin.
	 *
	 * Two active plugins can not have the same unique feature string. For example, only
	 * one sound driver can be loaded at a time.
	 */
	QString provides() const;

	/**
	 * @return List of dependencies of plugin.
	 *
	 * Returns list of dependencies of plugin. This list contains names of plugins that have to be
	 * activated before this plugin. If any of plugins in this list is not available or could not
	 * be activated, this plugin wont get activated.
	 */
	QStringList dependencies() const;

	/**
	 * @return List of plugins that this replaces..
	 *
	 * Returns list of plugins replaced by this one. If plugin with name from this list was active in previous
	 * run of Kadu and is no longer avilable, this plugin will be activated instead. This allows us to
	 * change name of plugins and to create more generic plugins from less generic (for example: MPRIS mediaplayer
	 * replaces Amarok and other mediaplayer plugins)..
	 */
	QStringList replaces() const;

	/**
	 * @return Load by default value for this plugin.
	 *
	 * Returns load by default value for this plugin. If plugin has load by default set
	 * to true and it is new (was not known in previous run of Kadu) it will be activated
	 * automatically on start. This option should be only set for plugins that are very
	 * important, like new protocols plugns or plugins that imports data from older Kadu
	 * version.
	 */
	bool loadByDefault() const;

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

Q_DECLARE_METATYPE(PluginMetadata);

/**
 * @}
 */
