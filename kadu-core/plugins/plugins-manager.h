/*
 * %kadu copyright begin%
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef PLUGINS_MANAGER_H
#define PLUGINS_MANAGER_H

#include <QtCore/QLibrary>
#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "storage/storable-object.h"
#include "exports.h"

class QCheckBox;
class QLabel;
class QPluginLoader;
class QTranslator;
class QTreeWidget;
class QTreeWidgetItem;

class GenericPlugin;
class ModulesWindow;
class Plugin;

class KADUAPI PluginsManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PluginsManager)

	static PluginsManager *Instance;

	typedef int InitModuleFunc(bool);
	typedef void CloseModuleFunc(void);

	QMap<QString, Plugin *> Plugins;

	ModulesWindow *Window;

	PluginsManager();
	virtual ~PluginsManager();

	void incDependenciesUsageCount(Plugin *plugin);

	void importFrom09();
	void ensureLoadedAtLeastOnce(const QString &moduleName);

	QStringList installedPlugins() const;

	QString findActiveConflict(Plugin *plugin) const;
	bool activateDependencies(Plugin *plugin);

	QString activeDependentPluginNames(const QString &pluginName) const;

private slots:
	void dialogDestroyed();

protected:
	virtual void load();

public:
	static PluginsManager * instance();

	// storage implementation
	virtual StorableObject * storageParent() { return 0; }
	virtual QString storageNodeName() { return QLatin1String("Plugins"); }

	virtual void store();

	const QMap<QString, Plugin *> & plugins() const { return Plugins; }
	QList<Plugin *> activePlugins() const;

	void activateProtocolPlugins();
	void activatePlugins();
	void deactivatePlugins();

	bool activatePlugin(Plugin *plugin);
	bool deactivatePlugin(Plugin *plugin, bool force);

	void usePlugin(const QString &pluginName);
	void releasePlugin(const QString &pluginName);

public slots:
	void showWindow(QAction *sender, bool toggled);

};

#endif // PLUGINS_MANAGER_H
