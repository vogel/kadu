/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#define SMS_USE_DEBUGGER 0

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtScript/QScriptEngine>
#include <QtWidgets/QMainWindow>
#if SMS_USE_DEBUGGER
#include <QtScriptTools/QScriptEngineDebugger>
#endif

#include "core/application.h"
#include "misc/paths-provider.h"

#include "scripts/network-access-manager-wrapper.h"
#include "scripts/sms-translator.h"

#include "sms-script-manager.h"

SmsScriptsManager * SmsScriptsManager::Instance = 0;

SmsScriptsManager * SmsScriptsManager::instance()
{
	if (!Instance)
	{
		Instance = new SmsScriptsManager();
		Instance->init();
	}

	return Instance;
}

void SmsScriptsManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SmsScriptsManager::SmsScriptsManager()
{
	Engine = new QScriptEngine(this);
	Network = new NetworkAccessManagerWrapper(Engine, this);

	Engine->globalObject().setProperty("network", Engine->newQObject(Network));
	Engine->globalObject().setProperty("translator", Engine->newQObject(new SmsTranslator(this)));
}

SmsScriptsManager::~SmsScriptsManager()
{
}

void SmsScriptsManager::init()
{
#if SMS_USE_DEBUGGER
 	QScriptEngineDebugger *debugger = new QScriptEngineDebugger(this);
 	debugger->attachTo(Engine);
 	debugger->standardWindow()->show();
#endif

	QString scriptPath = Application::instance()->pathsProvider()->profilePath() + QLatin1String("plugins/data/sms/scripts/gateway.js");
	if (QFile::exists(scriptPath))
		loadScript(scriptPath);
	else
	{
		scriptPath = Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/data/sms/scripts/gateway.js");
		if (QFile::exists(scriptPath))
			loadScript(scriptPath);
		// TODO: maybe we should return here if no gateway.js was found?
	}

	// scripts from profile path can replace the ones from data path if the file name is the same
	loadScripts(QDir(Application::instance()->pathsProvider()->profilePath() + QLatin1String("plugins/data/sms/scripts")));
	loadScripts(QDir(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/data/sms/scripts")));
}

void SmsScriptsManager::loadScripts(const QDir &dir)
{
	if (!dir.exists())
		return;

	QFileInfoList gateways = dir.entryInfoList(QStringList("gateway-*.js"));
	foreach (const QFileInfo &gatewayFile, gateways)
		loadScript(gatewayFile);
}

void SmsScriptsManager::loadScript(const QFileInfo &fileInfo)
{
	if (!fileInfo.exists())
		return;

	// We want file name exluding the path - file from a higher priority dir can
	// replace a file of the same name from different dir.
	QString fileName = fileInfo.fileName();
	if (LoadedFiles.contains(fileName))
		return;
	LoadedFiles.append(fileName);

	QFile file(fileInfo.absoluteFilePath());
	if (!file.open(QFile::ReadOnly))
		return;

	QTextStream reader(&file);
	reader.setCodec("UTF-8");
	QString content = reader.readAll();
	file.close();

	if (content.isEmpty())
		return;

	Engine->evaluate(content);
}

#include "moc_sms-script-manager.cpp"
