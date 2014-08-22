/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "message/message-manager.h"
#include "services/message-filter-service.h"

#include "notify/cenzor-notification.h"

#include "cenzor.h"

Cenzor * Cenzor::Instance = 0;

void Cenzor::createInstance()
{
	if (!Instance)
		Instance = new Cenzor();
}

void Cenzor::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Cenzor::Cenzor()
{
	Core::instance()->messageFilterService()->registerMessageFilter(this);
}

Cenzor::~Cenzor()
{
	Core::instance()->messageFilterService()->unregisterMessageFilter(this);
}

bool Cenzor::acceptMessage(const Message &message)
{
	if (MessageTypeSent == message.type())
		return true;

	if (!Configuration.enabled())
		return true;

	if (!shouldIgnore(message.plainTextContent()))
		return true;

	Account account = message.messageChat().chatAccount();

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return false;


	if (MessageManager::instance()->sendMessage(message.messageChat(), Configuration.admonition(), true))
		CenzorNotification::notifyCenzored(message.messageChat());

	return false;
}

bool Cenzor::shouldIgnore(const QString &message)
{
	QStringList words = message.split(' ', QString::SkipEmptyParts);

	foreach (const QString &word, words)
	{
		QString lowerWord = word.toLower();
		foreach (const QRegExp &swear, Configuration.swearList())
			if ((swear.indexIn(lowerWord) >= 0) && (!isExclusion(lowerWord)))
				return true;
	}

	return false;
}

bool Cenzor::isExclusion(const QString &word)
{
	foreach (const QRegExp &exclusion, Configuration.exclusionList())
		if (exclusion.indexIn(word) >= 0)
			return true;

	return false;
}

#include "moc_cenzor.cpp"
