/*
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GENERIC_PLUGIN_H
#define GENERIC_PLUGIN_H

#include <QtCore/QtPlugin>

/**
 * @addtogroup Plugins
 * @{
 */

/**
 * @class GenericPlugin
 * @author Rafał 'Vogel' Malinowski
 * @short Base interface for all Kadu plugins.
 *
 * Every Kadu plugin has to have a class that inherits from GenericPlugin. Next this class has to
 * be registered using Q_EXPORT_PLUGIN2 macro with plugin library name and class name as parameters.
 *
 * A new instance of given object is created when such plugin is loaded and then init() method is
 * called. Before plugin gets unloaded done() method is called.
 */
class GenericPlugin
{
public:
	virtual ~GenericPlugin() {}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called after plugin activation.
	 * @param firstLoad true, if this is first activation of current plugin
	 *
	 * This method is called every time a plugin is activated. Implementations should do all work
	 * needed to properly run plugin, like registering protocols, notifications and do on, in this
	 * method. Parameter firstLoad is set to true if it is first activation of plugin for given
	 * Kadu instance. Plugin can then do additional work if reuired (like showing configuration
	 * wizard window).
	 */
	virtual int init(bool firstLoad) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called before plugin deactivation.
	 *
	 * This method is called before a plugin is deactivated. Implementations should do all work
	 * needed to properly finalize plugin, like unregistering protocols, notifications and do on,
	 * in this method. Every action run in init() has to have a counterpart in this method.
	 */
	virtual void done() = 0;

};

Q_DECLARE_INTERFACE(GenericPlugin, "im.kadu.GenericPlugin/0.1")

/**
 * @}
 */

#endif // GENERIC_PLUGIN_H
