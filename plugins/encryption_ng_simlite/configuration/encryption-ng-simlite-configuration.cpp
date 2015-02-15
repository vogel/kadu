/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"

#include "encryption-ng-simlite-configuration.h"

EncryptionNgSimliteConfiguration * EncryptionNgSimliteConfiguration::Instance = 0;

EncryptionNgSimliteConfiguration * EncryptionNgSimliteConfiguration::instance()
{
	return Instance;
}

void EncryptionNgSimliteConfiguration::createInstance()
{
	if (!Instance)
		Instance = new EncryptionNgSimliteConfiguration();
}

void EncryptionNgSimliteConfiguration::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptionNgSimliteConfiguration::EncryptionNgSimliteConfiguration()
{
	configurationUpdated();
	createDefaultConfiguration();
}

EncryptionNgSimliteConfiguration::~EncryptionNgSimliteConfiguration()
{

}

void EncryptionNgSimliteConfiguration::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->removeVariable("Chat", "Encryption");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "EncryptAfterReceiveEncryptedMessage", true);
}

void EncryptionNgSimliteConfiguration::configurationUpdated()
{
	EncryptAfterReceiveEncryptedMessage = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "EncryptAfterReceiveEncryptedMessage", true);
}
