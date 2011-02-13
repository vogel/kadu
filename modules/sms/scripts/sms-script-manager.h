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

#ifndef SMS_SCRIPTS_MANAGER_H
#define SMS_SCRIPTS_MANAGER_H

#include <QtCore/QObject>

class QScriptEngine;

class NetworkAccessManagerWrapper;
class SmsTokenReader;

class SmsScriptsManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SmsScriptsManager)

	static SmsScriptsManager *Instance;

	QScriptEngine *Engine;
	NetworkAccessManagerWrapper *Network;
	SmsTokenReader *TokenReader;

	QList<QString> LoadedFiles;

	SmsScriptsManager();
	virtual ~SmsScriptsManager();

	void init();

public:
	static SmsScriptsManager * instance();
	static void destroyInstance();

	void loadScript(const QString &fileName);

	QScriptEngine * engine() { return Engine; }

};

#endif // SMS_SCRIPTS_MANAGER_H
