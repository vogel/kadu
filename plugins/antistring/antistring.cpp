/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFile>

#include "core/core.h"
#include "message/message-manager.h"
#include "services/message-filter-service.h"

#include "antistring-notification.h"

#include "antistring.h"

Antistring * Antistring::Instance = 0;

void Antistring::createInstance()
{
	if (!Instance)
		Instance = new Antistring();
}

void Antistring::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Antistring::Antistring()
{
	Core::instance()->messageFilterService()->registerMessageFilter(this);
}

Antistring::~Antistring()
{
	Core::instance()->messageFilterService()->unregisterMessageFilter(this);
}

bool Antistring::acceptMessage(const Message &message)
{
	if (!Configuration.enabled())
		return true;

	if (MessageTypeSent == message.type())
		return true;

	if (points(message.plainTextContent()) < 3)
		return true;

	AntistringNotification::notifyStringReceived(message.messageChat());
	MessageManager::instance()->sendMessage(message.messageChat(), Configuration.returnMessage(), true);

	if (Configuration.logMessage())
		writeLog(message.messageSender(), message.htmlContent());

	return !Configuration.messageStop();
}

void Antistring::writeLog(Contact sender, const QString &message)
{
	QFile logFile(Configuration.logFile());

	if (!logFile.exists())
	{
		if (!logFile.open(QFile::WriteOnly))
			return;

		QTextStream stream(&logFile);
		stream << tr("     DATA AND TIME      ::   ID   ::    MESSAGE\n") <<
		             "====================================================\n";

		logFile.close();
	}

	if (!logFile.open(QFile::WriteOnly | QFile::Append))
		return;

	QTextStream stream(&logFile);
	stream << QDateTime::currentDateTime().toString() << " :: " << sender.id() << " :: " <<
			message << "\n";
	logFile.close();
}

int Antistring::points(const QString &message)
{
	int result = 0;

	if (message.length() > 600)
		result++;

	foreach (const ConditionPair &condition, Configuration.conditions())
		if (message.indexOf(QRegExp(condition.first)) >= 0)
			result += condition.second;

	return result;
}

#include "moc_antistring.cpp"
