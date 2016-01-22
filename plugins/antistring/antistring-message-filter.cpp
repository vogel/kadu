/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "antistring-message-filter.h"

#include "antistring-configuration.h"
#include "antistring-notification.h"

#include "core/injected-factory.h"
#include "message/message-manager.h"
#include "notification/notification-manager.h"

#include <QtCore/QFile>

AntistringMessageFilter::AntistringMessageFilter(QObject *parent) :
		QObject{parent}
{
}

AntistringMessageFilter::~AntistringMessageFilter()
{
}

void AntistringMessageFilter::setAntistringConfiguration(AntistringConfiguration *antistringConfiguration)
{
	m_antistringConfiguration = antistringConfiguration;
}

void AntistringMessageFilter::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void AntistringMessageFilter::setMessageManager(MessageManager *messageManager)
{
	m_messageManager = messageManager;
}

void AntistringMessageFilter::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

bool AntistringMessageFilter::acceptMessage(const Message &message)
{
	if (!m_antistringConfiguration->enabled())
		return true;

	if (MessageTypeSent == message.type())
		return true;

	if (points(message.plainTextContent()) < 3)
		return true;

	AntistringNotification::notifyStringReceived(m_injectedFactory, m_notificationManager, message.messageChat());
	m_messageManager->sendMessage(message.messageChat(), m_antistringConfiguration->returnMessage(), true);

	if (m_antistringConfiguration->logMessage())
		writeLog(message.messageSender(), message.htmlContent());

	return !m_antistringConfiguration->messageStop();
}

void AntistringMessageFilter::writeLog(Contact sender, const QString &message)
{
	QFile logFile(m_antistringConfiguration->logFile());

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

int AntistringMessageFilter::points(const QString &message)
{
	int result = 0;

	if (message.length() > 600)
		result++;

	foreach (const ConditionPair &condition, m_antistringConfiguration->conditions())
		if (message.indexOf(QRegExp(condition.first)) >= 0)
			result += condition.second;

	return result;
}

#include "moc_antistring-message-filter.cpp"
