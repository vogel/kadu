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
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "storage/storable-object.h"
#include "exports.h"

#include <memory>
#include <QtCore/QLibrary>
#include <QtCore/QMap>
#include <QtGui/QWidget>

class PluginRootComponent;
class PluginActivationService;
class PluginInfo;
class PluginInfoRepository;
class PluginStateService;
class PluginsWindow;
class StoragePointFactory;

class QCheckBox;
class QLabel;
class QPluginLoader;
class QTranslator;
class QTreeWidget;
class QTreeWidgetItem;

enum class PluginActivationReason;
enum class PluginDeactivationReason;
enum class PluginState;

/**
 * @addtogroup Plugins
 * @{
 */

/**
 * @class PluginsManager
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
class KADUAPI PluginsManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PluginsManager)

public:
	explicit PluginsManager(QObject *parent = nullptr);
	virtual ~PluginsManager();

	void setPluginActivationService(PluginActivationService *pluginActivationService);
	void setPluginInfoRepository(PluginInfoRepository *pluginInfoRepository);
	void setPluginStateService(PluginStateService *pluginStateService);
	void setStoragePointFactory(StoragePointFactory *storagePointFactory);

	// storage implementation
	virtual StorableObject * storageParent() { return nullptr; }
	virtual QString storageNodeName() { return QLatin1String("Plugins"); }

	void loadPluginStates();
	void storePluginStates();

	void activateProtocolPlugins();
	void activatePlugins();
	void deactivatePlugins();

	bool activatePlugin(const QString &pluginName, PluginActivationReason reason);
	void deactivatePlugin(const QString &pluginName, PluginDeactivationReason reason);

protected:
	virtual void load();
	virtual void store();

private:
	QWeakPointer<PluginActivationService> m_pluginActivationService;
	QWeakPointer<PluginInfoRepository> m_pluginInfoRepository;
	QWeakPointer<PluginStateService> m_pluginStateService;
	QWeakPointer<StoragePointFactory> m_storagePointFactory;

	std::unique_ptr<PluginDependencyGraph> m_pluginDependencyDAG;

	void importFrom09();

	QStringList installedPlugins() const;

	PluginInfo loadPlugin(const QString &pluginName);
	QString findActiveProviding(const QString &feature) const;
	QVector<QString> allDependencies(const QString &pluginName) noexcept;
	QVector<QString> allDependents(const QString &pluginName) noexcept;

	bool shouldActivate(const QString &pluginName) const noexcept;
	QString findReplacementPlugin(const QString &pluginToReplace) const noexcept;

	void activationError(const QString &pluginName, const QString &errorMessage, PluginActivationReason activationReason);

private slots:
	void setStateEnabledIfInactive(const QString &pluginName, bool enable);

};

/**
 * @}
 */
