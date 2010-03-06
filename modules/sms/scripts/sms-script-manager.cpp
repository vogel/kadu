/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtNetwork/QNetworkAccessManager>
#include <QtScript/QScriptEngine>

#include "sms-script-manager.h"

SmsScriptsManager * SmsScriptsManager::Instance = 0;

SmsScriptsManager * SmsScriptsManager::instance()
{
	if (!Instance)
		Instance = new SmsScriptsManager();

	return Instance;
}

SmsScriptsManager::SmsScriptsManager()
{
	Network = new QNetworkAccessManager(this);
	Engine = new QScriptEngine(this);
}

SmsScriptsManager::~SmsScriptsManager()
{
}
