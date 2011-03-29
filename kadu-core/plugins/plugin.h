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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtCore/QObject>

#include "plugins/plugins-manager.h"

#include "storage/named-storable-object.h"

class QLibrary;
class QPluginLoader;
class QTranslator;

class GenericPlugin;
class PluginInfo;

class Plugin : public QObject, public NamedStorableObject
{
	Q_OBJECT

public:
	enum PluginState
	{
		PluginStateNew,
		PluginStateDisabled,
		PluginStateEnabled
	};

private:
	typedef int InitModuleFunc(bool);
	typedef void CloseModuleFunc(void);

	QString Name;
	bool Active;
	PluginState State;
	bool FirstLoad;

	QPluginLoader *PluginLoader;
	GenericPlugin *PluginObject;

	QLibrary *PluginLibrary;
	CloseModuleFunc *Close;
	QTranslator *Translator;
	PluginInfo *Info;
	int UsageCounter;

	void loadTranslations();

protected:
	virtual void load();

public:
	Plugin(const QString &name, QObject *parent = 0);
	virtual ~Plugin();

	// storage implementation
	virtual StorableObject* storageParent() { return PluginsManager::instance(); }
	virtual QString storageNodeName() { return QLatin1String("Plugin"); }
	virtual QString name() const { return Name; }

	virtual void store();

	bool shouldBeActivated();

	bool activate();
	bool deactivate();

	bool isValid() const { return 0 != Info; }
	bool isActive() const { return Active; }
	PluginState state() { ensureLoaded(); return State; }
	void setState(PluginState state);

	PluginInfo * info() const { return Info; }

	GenericPlugin * pluginObject() const { return PluginObject; }
	QLibrary * pluginLibrary() const { return PluginLibrary; }

	int usageCounter() const { return UsageCounter; }
	void incUsage() { UsageCounter++; }
	void decUsage() { UsageCounter--; }

	QString type() const;

};

#endif // PLUGIN_H
