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

#ifndef KADU_MODULES_H
#define KADU_MODULES_H

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
class PluginInfo;

class KADUAPI ModulesManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ModulesManager)

	ModulesManager();

	static ModulesManager *Instance;

	friend class Plugin;

	typedef int InitModuleFunc(bool);
	typedef void CloseModuleFunc(void);

	/**
		\struct StaticModule
		\brief Informacje o statycznym module.
	**/
	struct StaticModule
	{
		InitModuleFunc *init;
		CloseModuleFunc *close;
	};

	QMap<QString, StaticModule> StaticModules;

	QMap<QString, Plugin *> Modules;

	QStringList protocolModulesList;

	ModulesWindow *Window;

	QTranslator *loadModuleTranslation(const QString &module_name);

	bool satisfyModuleDependencies(PluginInfo *pluginInfo);

	void incDependenciesUsageCount(PluginInfo *pluginInfo);

	void registerStaticModule(const QString &module_name, InitModuleFunc *init, CloseModuleFunc *close);

	void registerStaticModules();

	QStringList protocolModules() const;

	QObject *translators;

	void ensureLoadedAtLeastOnce(const QString &moduleName);

private slots:
	void dialogDestroyed();

protected:
	virtual void load();

public:
	// storage implementation
	virtual StorableObject * storageParent() { return 0; }
	virtual QString storageNodeName() { return QLatin1String("Plugins"); }

	virtual void store();

	void loadProtocolModules();

	void loadAllModules();

	void unloadAllModules();

	static ModulesManager * instance();

	~ModulesManager();

	QStringList staticModules() const;
	QStringList installedModules() const;
	QStringList loadedModules() const;
	QStringList unloadedModules() const;
	QStringList activeModules() const;

	QString moduleProvides(const QString &provides);

	PluginInfo * pluginInfo(const QString &moduleName) const;

	bool moduleIsStatic(const QString &module_name) const;
	bool moduleIsInstalled(const QString &module_name) const;
	bool moduleIsLoaded(const QString &module_name) const;
	bool moduleIsActive(const QString &module_name) const;

	QString modulesUsing(const QString &module_name) const;

	bool conflictsWithLoaded(const QString &module_name, PluginInfo *pluginInfo) const;

public slots:
	bool activateModule(const QString &module_name);
	bool deactivateModule(const QString &module_name, bool setAsUnloaded, bool force);

	void showWindow(QAction *sender, bool toggled);

	void moduleIncUsageCount(const QString &module_name);
	void moduleDecUsageCount(const QString &module_name);

	void saveLoadedModules();

};

#endif
