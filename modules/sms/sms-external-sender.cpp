/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtCore/QProcess>

#include "configuration/configuration-file.h"

#include "sms-external-sender.h"

SmsExternalSender::SmsExternalSender(const QString &number, const QString &gatewayId, QObject *parent) :
		QObject(parent), GatewayId(gatewayId), Number(number)
{
	fixNumber();
}

SmsExternalSender::~SmsExternalSender()
{
}

void SmsExternalSender::fixNumber()
{
	if (Number.length() == 12 && Number.left(3) == "+48")
		Number = Number.right(9);
}

bool SmsExternalSender::validateNumber()
{
	return 9 == Number.length();
}

bool SmsExternalSender::validateSignature()
{
	return !Signature.isEmpty();
}

void SmsExternalSender::setContact(const QString &contact)
{
	Contact = contact;
}

void SmsExternalSender::setSignature(const QString &signature)
{
	Signature = signature;
}

QStringList SmsExternalSender::buildProgramArguments(const QString &message)
{
	QStringList programArguments;

	if (config_file.readBoolEntry("SMS", "UseCustomString"))
	{
		programArguments = config_file.readEntry("SMS", "SmsString").split(' ');
		programArguments.replaceInStrings("%n", Number);
		programArguments.replaceInStrings("%n", message);
	}
	else
	{
		programArguments.append(Number);
		programArguments.append(message);
	}

	return programArguments;
}

void SmsExternalSender::sendMessage(const QString &message)
{
	QString smsAppPath = config_file.readEntry("SMS", "SmsApp");

	Process = new QProcess(this);
	Process->start(smsAppPath, buildProgramArguments(message));

	if (!Process->waitForStarted())
	{
		emit finished(tr("Could not spawn child process. Check if the program is functional"));
		delete Process;
		Process = 0;
		return;
	}

	connect(Process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished()));
}

void SmsExternalSender::processFinished()
{
	if (QProcess::NormalExit == Process->exitStatus())
		emit finished("");
	else
		emit finished(tr("The process exited abnormally. The SMS may not be sent"));

	delete Process;
	Process = 0;
}
