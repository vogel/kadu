/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugin.h"

#include "configuration/configuration-file.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/plugin-error-dialog.h"
#include "misc/kadu-paths.h"
#include "plugins/active-plugin.h"
#include "plugins/generic-plugin.h"
#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugins-common.h"
#include "debug.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtGui/QApplication>

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Creates new Plugin object and loads PluginInfo data.
 * @param name name of plugin
 * @param parent parent object, almost always PluginManager instance
 *
 * This contructor load data for plugin with given name. New instance of PluginInfo class
 * is created if dataDir/kadu/plugins/name.desc is found. If this file is not found, plugin
 * is marked as invalid and will be unable to be activated.
 */
Plugin::Plugin(PluginInfo pluginInfo, QObject *parent) :
		QObject{parent}, m_pluginInfo(std::move(pluginInfo)),
		m_activePlugin{nullptr}, m_state{PluginStateNew}
{
	StorableObject::setState(StateNotLoaded);
}

Plugin::~Plugin()
{
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Loadas plugin data from storage.
 *
 * This method load plugin data (for now it is only State) from /Plugins/Plugin[\@name=pluginName]/ storage node.
 */
void Plugin::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	auto stateString = loadValue<QString>("State");
	if (stateString == "Loaded")
		m_state = PluginStateEnabled;
	else if (stateString == "NotLoaded")
		m_state = PluginStateDisabled;
	else
		m_state = PluginStateNew;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Stores plugin data to storage.
 *
 * This method stores plugin data (for now it is only State) into /Plugins/Plugin[\@name=pluginName]/ storage node.
 */
void Plugin::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	switch (m_state)
	{
		case PluginStateEnabled:
			storeValue("State", "Loaded");
			break;
		case PluginStateDisabled:
			storeValue("State", "NotLoaded");
			break;
		default:
			Q_ASSERT(false);
			break;
	}
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Reimplemented from StorableObject::shouldStore().
 *
 * Reimplemented from StorableObject::shouldStore(). Returns false if State is PluginStateNew.
 */
bool Plugin::shouldStore()
{
	ensureLoaded();

	return NamedStorableObject::shouldStore() && PluginStateNew != state();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns true if this plugin should be activated.
 * @return true if this plugin should be activated
 *
 * Module should be activated only if:
 * <ul>
 *   <li>it is valid (has .desc file associated with it)
 *   <li>is either PluginStateEnabled or PluginStateNew with PluginInfo::loadByDefault() set to true
 * </ul>
 */
bool Plugin::shouldBeActivated()
{
	ensureLoaded();

	if (isActive())
		return false;

	if (PluginStateEnabled == m_state)
		return true;
	if (PluginStateDisabled == m_state)
		return false;
	return m_pluginInfo.loadByDefault();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activates plugin and retursn true if plugin is active.
 * @param reason plugin activation reason
 * @return true if plugin is active after method return
 *
 * This method loads plugin library file (if exists) and set up  GenericPlugin object from this library.
 * Then translations file is loaded. Next GenericPlugin::init() method is called with firstLoad
 * paramer set to true if this plugin's state  if PluginStateNew. If this methods returns value different
 * than 0, plugin is deactivated and false value is returned.
 *
 * Translations must be loaded before the root component of the plugin is instantiated.
 *
 * This method returns true if plugin is active after method returns - especially when plugin was active
 * before this call.
 */
void Plugin::activate() noexcept(false)
{
	if (m_activePlugin)
		return;

	ensureLoaded();

	m_activePlugin = new ActivePlugin(this, PluginStateNew == m_state);

	kdebugf2();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Deactivates plugin.
 *
 * If plugin is active, its GenericPlugin::done() method is called. Then all deferred delete
 * events are sent so that we will not end up trying to delete objects belonging to unloaded
 * plugins. Finally all data is removed from memory - plugin library file and plugin translations.
 */
void Plugin::deactivate() noexcept
{
	if (!m_activePlugin)
		return;

	delete m_activePlugin;
	m_activePlugin = 0;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Sets state of plugin.
 *
 * This method changes state of plugin. Set state to PluginStateEnabled to make this plugin
 * activate at every Kadu run.
 *
 * Please do not call this method unless you are absolutely sure the plugin had been loaded
 * at least once.
 */
void Plugin::setState(Plugin::PluginState state)
{
	ensureLoaded();

	m_state = state;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets state enablement of plugin if it is inactive.
 *
 * If this plugin is active or its state is PluginStateNew, this method does nothing.
 *
 * Otherwise, this method sets its state to PluginStateEnabled if \p enable is true.
 * If \p enable is false, this method sets the plugin's state to PluginStateDisabled.
 */
void Plugin::setStateEnabledIfInactive(bool enable)
{
	if (isActive())
		return;

	// It is necessary to not break firstLoad.
	if (PluginStateNew == state())
		return;

	setState(enable ? PluginStateEnabled : PluginStateDisabled);
}

#include "moc_plugin.cpp"
