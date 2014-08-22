/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptEngine>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/windows/message-dialog.h"
#include "debug.h"

#include "scripts/sms-script-manager.h"
#include "sms-gateway-manager.h"
#include "sms-gateway-query.h"
#include "sms-token-read-job.h"

#include "sms-internal-sender.h"

SmsInternalSender::SmsInternalSender(const QString &number, const SmsGateway &gateway, QObject *parent) :
		SmsSender(number, parent), Gateway(gateway), TokenJob(0)
{
}

SmsInternalSender::~SmsInternalSender()
{
}

void SmsInternalSender::sendMessage(const QString &message)
{
	Message = message;

	if (Gateway.signatureRequired() && !validateSignature())
	{
		emit finished(false, "dialog-error", tr("Signature can't be empty."));
		kdebugf2();
		return;
	}

	if (Gateway.id().isEmpty())
		queryForGateway();
	else
		sendSms();
}

void SmsInternalSender::queryForGateway()
{
	emit progress("dialog-information", tr("Detecting gateway..."));

	SmsGatewayQuery *query = new SmsGatewayQuery(this);
	connect(query, SIGNAL(finished(const QString &)), this, SLOT(gatewayQueryDone(const QString &)));
	query->process(number());
}

void SmsInternalSender::jobFinished(bool ok, const QString &entryIcon, const QString &entryMessage)
{
	if (!ok)
	{
		emit finished(ok, entryIcon, entryMessage);
		emit canceled();
		deleteLater();
	}
	else
		emit progress(entryIcon, entryMessage);

	TokenJob = 0;
}

void SmsInternalSender::readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod)
{
	TokenJob = new SmsTokenReadJob(this);

	TokenJob->setCallback(callbackObject, callbackMethod);
	TokenJob->setTokenImageUrl(tokenImageUrl);

	connect(TokenJob, SIGNAL(progress(QString,QString)), this, SIGNAL(progress(QString,QString)));
	connect(TokenJob, SIGNAL(finished(bool,QString,QString)), this, SLOT(jobFinished(bool,QString,QString)));

	TokenJob->exec();
}

void SmsInternalSender::gatewayQueryDone(const QString &gatewayId)
{
	if (gatewayId.isEmpty())
	{
		emit finished(false, "dialog-error", tr("Automatic gateway selection is not available. Please select SMS gateway manually."));
		kdebugf2();
		return;
	}

	Gateway = SmsGatewayManager::instance()->byId(gatewayId);

	emit progress("dialog-information", tr("Detected gateway: %1.").arg(Gateway.name()));

	sendSms();
}

QScriptValue SmsInternalSender::readFromConfiguration(const QString &group, const QString &name, const QString &defaultValue)
{
	return Application::instance()->configuration()->deprecatedApi()->readEntry(group, name, defaultValue);
}

void SmsInternalSender::sendSms()
{
	emit gatewayAssigned(number(), Gateway.id());

	emit progress("dialog-information", tr("Sending SMS..."));

	QScriptEngine *engine = SmsScriptsManager::instance()->engine();

	QScriptValue jsGatewayManagerObject = engine->evaluate("gatewayManager");
	QScriptValue jsSendSms = jsGatewayManagerObject.property("sendSms");
	QScriptValueList arguments;
	arguments.append(Gateway.id());
	arguments.append(number());
	arguments.append(signature());
	arguments.append(Message);
	arguments.append(engine->newQObject(this));

	jsSendSms.call(jsGatewayManagerObject, arguments);
}

void SmsInternalSender::cancel()
{
	if (TokenJob)
	{
		disconnect(TokenJob, 0, this, 0);
		TokenJob->cancel(); // it will destroy job
		TokenJob = 0;
	}

	deleteLater();
}

void SmsInternalSender::result()
{
	emit smsSent(number(), Message);
	emit finished(true, "dialog-information", tr("SMS sent"));

	deleteLater();
}

void SmsInternalSender::failure(const QString &errorMessage)
{
	emit finished(false, "dialog-error", errorMessage);

	deleteLater();
}

#include "moc_sms-internal-sender.cpp"
