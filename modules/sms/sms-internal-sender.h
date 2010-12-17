/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef SMS_INTERNAL_SENDER_H
#define SMS_INTERNAL_SENDER_H

#include <QtScript/QScriptValue>

#include "sms-sender.h"

class QNetworkReply;

class SmsInternalSender : public SmsSender
{
	Q_OBJECT

	QString GatewayId;

	QNetworkReply *TokenReply;

	QScriptValue TokenCallbackObject;
	QScriptValue TokenCallbackMethod;

	void queryForGateway();
	void gatewaySelected();

	void sendSms();

private slots:
    void tokenImageDownloaded();

public:
	explicit SmsInternalSender(const QString &number, const QString &gatewayId = QString(), QObject *parent = 0);
	virtual ~SmsInternalSender();

	virtual void sendMessage(const QString& message);

	void findGatewayForNumber(const QString &number);

	virtual void tokenRead(const QString &tokenValue);

public slots:
	void gatewayQueryDone(const QString &gatewayId);
	void readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod);

	QScriptValue readFromConfiguration(const QString &group, const QString &name, const QString &defaultValue);

	void result();
	void failure(const QString &errorMessage);

};

#endif // SMS_INTERNAL_SENDER_H
