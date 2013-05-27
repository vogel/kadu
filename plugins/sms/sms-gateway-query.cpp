/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QProcess>
#include <QtScript/QScriptEngine>

#include "scripts/sms-script-manager.h"

#include "sms-gateway-query.h"

SmsGatewayQuery::SmsGatewayQuery(QObject *parent) :
		QObject(parent)
{
}

SmsGatewayQuery::~SmsGatewayQuery()
{
}

void SmsGatewayQuery::queryFinished(const QString &provider)
{
	emit finished(provider);

	deleteLater();
}

void SmsGatewayQuery::process(const QString &number)
{
	QScriptEngine* engine = SmsScriptsManager::instance()->engine();
	QScriptValue jsGatewayQueryObject = engine->evaluate("new GatewayQuery()");
	QScriptValue jsGetGateway = jsGatewayQueryObject.property("getGateway");

	QScriptValueList arguments;
	arguments.append(number);
	arguments.append(engine->newQObject(this));

	jsGetGateway.call(jsGatewayQueryObject, arguments);
}

#include "moc_sms-gateway-query.cpp"
