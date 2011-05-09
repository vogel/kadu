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

#include <QtCore/QString>
#include <QtCore/QStringList>

/**
 * @addtogroup Plugins
 * @{
 */

/**
 * @class PluginInfo
 * @author Rafał 'Vogel' Malinowski
 * @short Information about plugin loaded from .desc files.
 * @todo Rewrite to use .desktop files
 *
 * This class contains information about an installed plugin. Every installed plugin has
 * associated .desc file that contains info like plugin author, version ordependencies.
 * New instances of this class are created with existing file name as paremeters. Otherwise,
 * object will contain invalid (empty) data.
 */
class PluginInfo
{
	QString Type;
	QStringList Dependencies;
	QStringList Conflicts;
	QStringList Provides;
	QStringList Replaces;
	QString Description;
	QString Author;
	QString Version;
	bool LoadByDefault;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new object with data loaded from given .desc file.
	 * @param fileName full path to .desc file
	 *
	 * Creates new object with data loaded from given .desc file. File must be present
	 * and valid, otherwise object will contain invalid (empty) data.
	 */
	PluginInfo(const QString &fileName);
	~PluginInfo();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns type of plugin.
	 * @return type of plugin
	 *
	 * Returns type of plugin. In Kadu 0.10.0 only one type of plugin is supported:
	 * Protocol plugin. All plugins that are not protocols should have this field
	 * empty.
	 *
	 * Plugins with type Protocol are activated before all other plugins (see
	 * PluginsManager::activateProtocolPlugins()).
	 */
	const QString & type() const { return Type; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of dependencies of plugin.
	 * @return list of dependencies of plugin
	 *
	 * Returns list of dependencies of plugin. This list contains names of plugins that have to be
	 * activated before this plugin. If any of plugins in this list is not available or could not
	 * be activated, this plugin wont get activated.
	 */
	const QStringList & dependencies() const { return Dependencies; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of conflicts of plugin.
	 * @return list of conflicts of plugin
	 *
	 * Returns list of conflicts of plugin. This list contains names of plugins and "provides" strings that
	 * that conflicts with this plugin. If any plugin from this list is active or "provides" string that
	 * is in this list, this plugin can not be activated.
	 */
	const QStringList & conflicts() const { return Conflicts; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of featured of plugin.
	 * @return list of conflicts of plugin
	 * @todo consider remove and replacing with Type
	 *
	 * Returns list of features of this plugin. Two active plugins can not have the same string in their
	 * "provides" lists.
	 */
	const QStringList & provides() const { return Provides; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of plugins replaced by this one.
	 * @return list of plugins replaced by this one
	 *
	 * Returns list of plugins replaced by this one. If plugin with name from this list was active in previous
	 * run of Kadu and is no longer avilable, this plugin will be activated instead. This allows us to
	 * change name of plugins and to create more generic plugins from less generic.
	 */
	const QStringList & replaces() const { return Replaces; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns description of plugin.
	 * @return description of plugin
	 *
	 * Returns description of plugin. If no description in current system language is available
	 * default one will be used.
	 */
	const QString & description() const { return Description; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns name of plugin's author.
	 * @return name of plugin's author
	 *
	 * Returns name of plugin's author.
	 */
	const QString & author() const { return Author; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns plugin's version.
	 * @return plugin's version
	 *
	 * Returns plugin's version.
	 */
	const QString & version() const { return Version; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns load by default value for this plugin.
	 * @return load by default value for this plugin
	 *
	 * Returns load by default value for this plugin. If plugin has load by default set
	 * to true and it is new (was not known in previous run of Kadu) it will be activated
	 * automatically on start. This option should be only set for plugins that are very
	 * important, like new protocols plugns or plugins that imports data from older Kadu
	 * version.
	 */
	bool loadByDefault() const { return LoadByDefault; }

};

/**
 * @}
 */

#endif // PLUGIN_INFO_H
