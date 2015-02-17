/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2007, 2008, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include "plugin/metadata/plugin-metadata.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtCore/QVector>
#include <functional>
#include <injeqt/injeqt.h>

class PluginActivationService;
class PluginDependencyHandler;
class PluginStateService;

enum class PluginState;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginManager
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for loading and unloading plugins.
 *
 * This class manages all Kadu plugins, allows for loading and unloading and manages dependencies and
 * replacements.
 *
 * Configuration of this class is stored in storage node /root/Plugins with each plugin stored as
 * /root/Plugins/Plugin/\@name="".
 * If attribute /root/Plugins/\@imported_from_09 is not present this object will try to import
 * configuration from depreceated 0.9.x nodes.
 *
 * List of plugins is loaded from datadir/kadu/plugins directory as list of *.desc files (soon to be
 * ported to *.desktop files). Also list of all known plugins from previous version is loaded from
 * depreceated (0.9.x and before) configuration entries and from storage of this object.
 *
 * Plugins can be valid and invalid. Invalid plugins do not have *.desc files. They are stored because
 * of replacements mechanism.
 *
 * For activating and deactivating plugins see activatePlugin() and deactivatingPlugin().
 * For enabling auto-activating plugins see Plugin::setState() method - only plugins with state equal
 * to PluginState::Enabled will be loaded automatically.
 */
class KADUAPI PluginManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PluginManager)

public:
	Q_INVOKABLE explicit PluginManager(QObject *parent = nullptr);
	virtual ~PluginManager();

	/**
	 * @short Activate all protocols plugins that are enabled.
	 *
	 * This method activates all plugins with type "protocol" that are either enabled (PluginState::Enabled)
	 * or new (PluginState::New) with attribute "load by default" set. This method is generally called before
	 * any other activation to ensure that all protocols and accounts are available for other plugins.
	 */
	void activateProtocolPlugins();

	/**
	 * @short Activate all plugins that are enabled.
	 *
	 * This method activates all plugins that are either enabled (PluginState::Enabled) or new (PluginState::New)
	 * with attribute "load by default" set. If given enabled plugin is no longer available replacement plugin is searched
	 * (by checking Plugin::replaces()). Any found replacement plugin is activated.
	 */
	void activatePlugins();
	void activateReplacementPlugins();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Activate all plugins that are enabled.
	 *
	 * This method deactivated all active plugins. First iteration of deactivation check Plugin::usageCounter() value
	 * to check if given plugin can be safely removed (no other active plugins depends on it). This procedure is
	 * performed for all active plugins until no more plugins can be deactivated. Then second iteration is performed.
	 * This time no checks are performed.
	 */
	void deactivatePlugins();

private:
	QPointer<PluginActivationService> m_pluginActivationService;
	QPointer<PluginDependencyHandler> m_pluginDependencyHandler;
	QPointer<PluginStateService> m_pluginStateService;
	bool m_runningUnity;

	QVector<QString> pluginsToActivate(std::function<bool(const PluginMetadata &)> filter = [](const PluginMetadata &){ return true; }) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if this plugin should be activated.
	 * @return true if this plugin should be activated
	 *
	 * Module should be activated only if:
	 * <ul>
	 *   <li>it is valid (has .desc file associated with it)
	 *   <li>is either PluginState::Enabled or PluginState::New with PluginMetadata::loadByDefault() set to true
	 * </ul>
	 */
	bool shouldActivate(const PluginMetadata &pluginMetadata) const noexcept;
	QString findReplacementPlugin(const QString &pluginToReplace) const noexcept;

private slots:
	INJEQT_SETTER void setPluginActivationService(PluginActivationService *pluginActivationService);
	INJEQT_SETTER void setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler);
	INJEQT_SETTER void setPluginStateService(PluginStateService *pluginStateService);

};

/**
 * @}
 */
