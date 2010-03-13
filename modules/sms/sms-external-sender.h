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

#ifndef SMS_EXTERNAL_SENDER_H
#define SMS_EXTERNAL_SENDER_H

#include <QtCore/QObject>

class QProcess;

class SmsExternalSender : public QObject
{
	Q_OBJECT

	QString GatewayId;
	QString Number;
	QString Contact;
	QString Signature;

	QProcess *Process;

	void fixNumber();
	bool validateNumber();
	bool validateSignature();

	QStringList buildProgramArguments(const QString &message);

private slots:
	void processFinished();

public:
	explicit SmsExternalSender(const QString &number, const QString &gatewayId = QString::null, QObject *parent = 0);
	virtual ~SmsExternalSender();

	void setContact(const QString& contact);
	void setSignature(const QString& signature);
	void sendMessage(const QString& message);

signals:
	void finished(const QString &errorMessage);

};

#endif // SMS_EXTERNAL_SENDER_H
