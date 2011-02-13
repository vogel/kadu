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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QMainWindow>
#include <QtScript/QScriptEngine>
#ifndef Q_OS_MAC
#include <QtScriptTools/QScriptEngineDebugger>
#endif
#include "misc/path-conversion.h"

#include "gui/windows/sms-token-reader.h"
#include "scripts/network-access-manager-wrapper.h"

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
	TokenReader = new SmsTokenReader(this);

	QScriptValue scriptNetwork = Engine->newQObject(Network);
	QScriptValue tokenReader = Engine->newQObject(TokenReader);

	Engine->globalObject().setProperty("network", scriptNetwork);
	Engine->globalObject().setProperty("tokenReader", tokenReader);
}

SmsScriptsManager::~SmsScriptsManager()
{
}

void SmsScriptsManager::init()
{
// 	QScriptEngineDebugger* debuger = new QScriptEngineDebugger(this);
// 	debuger->attachTo(Engine);
// 	debuger->standardWindow()->show();

	loadScript(dataPath("kadu/modules/data/sms/scripts/gateway.js"));

	QDir scriptDirectory(dataPath("kadu/modules/data/sms/scripts/"));
	if (scriptDirectory.exists())
	{
		QStringList filters;
		filters.append("gateway-*.js");

		QFileInfoList gateways = scriptDirectory.entryInfoList(filters);
		foreach (const QFileInfo &gatewayFile, gateways)
			loadScript(gatewayFile.filePath());
	}
}

void SmsScriptsManager::loadScript(const QString &fileName)
{
	QFile file(fileName);
	if (!file.exists())
		return;

	if (LoadedFiles.contains(fileName))
		return;
	LoadedFiles.append(fileName);

	if (!file.open(QFile::ReadOnly))
		return;

	QTextStream reader(&file);
	QString content = reader.readAll();
	file.close();

	if (content.isEmpty())
		return;

	Engine->evaluate(content);
}
