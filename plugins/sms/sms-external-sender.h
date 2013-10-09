/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2007, 2008, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef SMS_EXTERNAL_SENDER_H
#define SMS_EXTERNAL_SENDER_H

#include "sms-sender.h"

class QProcess;

class SmsExternalSender : public SmsSender
{
	Q_OBJECT

	QProcess *Process;

	QStringList buildProgramArguments(const QString &message);

private slots:
	void processFinished();

public:
	explicit SmsExternalSender(const QString &number, QObject *parent = 0);
	virtual ~SmsExternalSender();

	virtual void sendMessage(const QString& message);

public slots:
	virtual void cancel();

};

#endif // SMS_EXTERNAL_SENDER_H
