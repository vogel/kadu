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

#include <QtCore/QFileInfo>
#include <QtCore/QPluginLoader>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "misc/path-conversion.h"
#include "plugins/generic-plugin.h"
#include "plugins/plugin-info.h"
#include "debug.h"
#include "modules.h"

#include "plugin.h"

#if defined(Q_OS_WIN)
	#define SO_EXT "dll"
	#define SO_EXT_LEN 3
	#define SO_PREFIX ""
	#define SO_PREFIX_LEN 0
#else
	#define SO_EXT "so"
	#define SO_EXT_LEN 2
	#define SO_PREFIX "lib"
	#define SO_PREFIX_LEN 3
#endif

Plugin::Plugin(const QString &name, QObject *parent) :
		QObject(parent),
		Name(name), Active(false), State(PluginStateNew), PluginLoader(0), PluginObject(0),
		PluginLibrary(0), Close(0), Translator(0), UsageCounter(0)
{
	QString descFilePath = dataPath("kadu/modules/" + name + ".desc");
	QFileInfo descFileInfo(descFilePath);

	if (descFileInfo.exists())
		Info = new PluginInfo(descFilePath);
	else
		Info = 0;

	StorableObject::setState(StateNotLoaded);
}

Plugin::~Plugin()
{
}

void Plugin::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	QString stateString = loadValue<QString>("State");
	if (stateString == "Loaded")
		State = PluginStateLoaded;
	else if (stateString == "NotLoaded")
		State = PluginStateNotLoaded;
	else
		State = PluginStateNew;
}

void Plugin::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	switch (State)
	{
		case PluginStateNew:
			storeValue("State", "New"); // should not happen, but who knows..
			break;
		case PluginStateLoaded:
			storeValue("State", "Loaded");
			break;
		case PluginStateNotLoaded:
			storeValue("State", "NotLoaded");
			break;
	}
}

bool Plugin::activate()
{
	if (Active)
		return true;

	InitModuleFunc *init;

	int res = 0;

	loadTranslations();

	if (Info->isPlugin())
	{
		PluginLibrary = 0;
		Close = 0;

		PluginLoader = new QPluginLoader(libPath("kadu/modules/"SO_PREFIX + Name + "." SO_EXT));
		PluginLoader->setLoadHints(QLibrary::ExportExternalSymbolsHint);

		if (!PluginLoader->load())
		{
			QString err = PluginLoader->errorString();
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Cannot load %1 plugin library.:\n%2").arg(Name, err));
			kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", qPrintable(Name), qPrintable(err));
			kdebugf2();
			return false;
		}

		PluginObject = dynamic_cast<GenericPlugin *>(PluginLoader->instance());
		if (!PluginObject)
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Cannot find required object in module %1.\n"
					"Maybe it's not Kadu-compatible plugin.").arg(Name));
			delete PluginLoader;
			PluginLoader = 0;
			kdebugf2();
			return false;
		}

		res = PluginObject->init(PluginStateNew == State);
	}
	else
	{
		PluginLibrary = new QLibrary(libPath("kadu/modules/"SO_PREFIX + Name + "." SO_EXT));
		PluginLibrary->setLoadHints(/*QLibrary::ResolveAllSymbolsHint |*/ QLibrary::ExportExternalSymbolsHint);
		if (!PluginLibrary->load())
		{
			QString err = PluginLibrary->errorString();
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Cannot load %1 module library.:\n%2").arg(Name, err));
			kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", qPrintable(Name), qPrintable(err));
			delete PluginLibrary;
			PluginLibrary = 0;
			Close = 0;
			kdebugf2();
			return false;
		}
		init = (InitModuleFunc *)PluginLibrary->resolve(qPrintable(Name + "_init"));
		Close = (CloseModuleFunc *)PluginLibrary->resolve(qPrintable(Name + "_close"));
		if (!init || !Close)
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Cannot find required functions in module %1.\n"
					"Maybe it's not Kadu-compatible Module.").arg(Name));
			delete PluginLibrary;
			PluginLibrary = 0;
			Close = 0;
			kdebugf2();
			return false;
		}

		res = init(PluginStateNew == State);
	}

	if (PluginStateNew == State)
	{
		QStringList everLoaded = config_file.readEntry("General", "EverLoaded").split(',', QString::SkipEmptyParts);
		everLoaded.append(Name);
		config_file.writeEntry("General", "EverLoaded", everLoaded.join(","));
	}

	if (res != 0)
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Module initialization routine for %1 failed.").arg(Name));

		delete PluginLibrary;
		PluginLibrary = 0;

		if (Translator)
			qApp->removeTranslator(Translator);
		delete Translator;
		Translator = 0;

		delete PluginLoader;
		PluginLoader = 0;
		PluginObject = 0;

		return false;
	}

	ModulesManager::instance()->incDependenciesUsageCount(Info);

	UsageCounter = 0;

	ModulesManager::instance()->Modules.insert(Name, this);
	kdebugf2();

	Active = true;

	return true;
}

bool Plugin::deactivate()
{
	if (!Active)
		return true;

	if (Close)
		Close();

	if (Translator)
		qApp->removeTranslator(Translator);

	delete Translator;
	Translator = 0;

	PluginLibrary->deleteLater();
	PluginLibrary = 0;

	PluginLoader->deleteLater();
	PluginLoader = 0;
	PluginObject = 0;

	ModulesManager::instance()->Modules.remove(Name);

	Active = false;

	kdebugf2();
	return true;
}

void Plugin::loadTranslations()
{
	Translator = new QTranslator(this);
	const QString lang = config_file.readEntry("General", "Language");

	if (Translator->load(Name + '_' + lang, dataPath("kadu/modules/translations/")))
		qApp->installTranslator(Translator);
	else
	{
		delete Translator;
		Translator = 0;
	}
}

void Plugin::setState(Plugin::PluginState state)
{
	State = state;
}
