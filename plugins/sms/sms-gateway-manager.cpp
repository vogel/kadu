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

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include "scripts/sms-script-manager.h"
#include "sms-gateway.h"

#include "sms-gateway-manager.h"
#include "sms-gateway-manager.moc"

SmsGatewayManager::SmsGatewayManager(QObject *parent) : QObject{parent}
{
}

SmsGatewayManager::~SmsGatewayManager()
{
}

void SmsGatewayManager::setSmsScriptsManager(SmsScriptsManager *smsScriptsManager)
{
    m_smsScriptsManager = smsScriptsManager;
}

void SmsGatewayManager::load()
{
    QScriptEngine *engine = m_smsScriptsManager->engine();
    qint32 length = engine->evaluate("gatewayManager.items.length").toInt32();

    for (qint32 i = 0; i < length; ++i)
    {
        QScriptValue gatewayName = engine->evaluate(QString("gatewayManager.items[%1].name()").arg(i));
        QScriptValue gatewayId = engine->evaluate(QString("gatewayManager.items[%1].id()").arg(i));
        QScriptValue gatewayMaxLength = engine->evaluate(QString("gatewayManager.items[%1].maxLength()").arg(i));
        QScriptValue gatewaySignatureRequired =
            engine->evaluate(QString("gatewayManager.items[%1].signatureRequired()").arg(i));

        SmsGateway gateway;
        gateway.setName(gatewayName.toString());
        gateway.setId(gatewayId.toString());
        gateway.setMaxLength(gatewayMaxLength.toUInt16());
        gateway.setSignatureRequired(gatewaySignatureRequired.toBool());

        m_items.append(gateway);
    }
}

SmsGateway SmsGatewayManager::byId(const QString &id) const
{
    for (auto gateway : m_items)
        if (gateway.id() == id)
            return gateway;

    return SmsGateway();
}
