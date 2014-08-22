/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QProcess>

#include "configuration/configuration-file.h"

#include "sms-external-sender.h"

SmsExternalSender::SmsExternalSender(const QString &number, QObject *parent) :
		SmsSender{number, parent},
		Process{}
{
}

SmsExternalSender::~SmsExternalSender()
{
}

QStringList SmsExternalSender::buildProgramArguments(const QString &message)
{
	QStringList programArguments;

	if (config_file.readBoolEntry("SMS", "UseCustomString"))
	{
		programArguments = config_file.readEntry("SMS", "SmsString").split(' ');
		programArguments.replaceInStrings("%k", number());
		programArguments.replaceInStrings("%m", message);
	}
	else
	{
		programArguments.append(number());
		programArguments.append(message);
	}

	return programArguments;
}

void SmsExternalSender::sendMessage(const QString &message)
{
	Message = message;

	QString smsAppPath = config_file.readEntry("SMS", "SmsApp");

	Process = new QProcess(this);
	Process->start(smsAppPath, buildProgramArguments(message));

	if (!Process->waitForStarted())
	{
		emit finished(false, "dialog-error", tr("Could not spawn child process. Check if the program is functional"));
		Process->deleteLater();
		Process = 0;
		deleteLater();
		return;
	}

	connect(Process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished()));
}

void SmsExternalSender::processFinished()
{
	if (QProcess::NormalExit == Process->exitStatus())
	{
		emit smsSent(number(), Message);
		emit finished(true, "dialog-information", tr("SMS sent"));
	}
	else
		emit finished(false, "dialog-error", tr("The process exited abnormally. The SMS may not be sent"));

	Process->deleteLater();
	Process = 0;

	deleteLater();
}

void SmsExternalSender::cancel()
{
	if (Process)
	{
		disconnect(Process, 0, this, 0);
		Process->terminate();
		Process->kill();
		Process->deleteLater();
		Process = 0;
	}

	deleteLater();
}

#include "moc_sms-external-sender.cpp"
