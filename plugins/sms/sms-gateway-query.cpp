/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtScript/QScriptEngine>

#include "scripts/sms-script-manager.h"

#include "sms-gateway-query.h"

SmsGatewayQuery::SmsGatewayQuery(SmsScriptsManager *smsScriptsManager, QObject *parent)
        : QObject{parent}, m_smsScriptsManager{smsScriptsManager}
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
    auto engine = m_smsScriptsManager->engine();
    auto jsGatewayQueryObject = engine->evaluate("new GatewayQuery()");
    auto jsGetGateway = jsGatewayQueryObject.property("getGateway");

    QScriptValueList arguments;
    arguments.append(number);
    arguments.append(engine->newQObject(this));

    jsGetGateway.call(jsGatewayQueryObject, arguments);
}

#include "moc_sms-gateway-query.cpp"
