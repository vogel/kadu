/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-plugin-object.h"

#include "facebook-protocol-factory.h"

#include "protocols/protocols-manager.h"

FacebookPluginObject::FacebookPluginObject(QObject *parent) : QObject{parent}
{
}

FacebookPluginObject::~FacebookPluginObject()
{
}

void FacebookPluginObject::setFacebookProtocolFactory(FacebookProtocolFactory *facebookProtocolFactory)
{
    m_facebookProtocolFactory = facebookProtocolFactory;
}

void FacebookPluginObject::setProtocolsManager(ProtocolsManager *protocolsManager)
{
    m_protocolsManager = protocolsManager;
}

void FacebookPluginObject::init()
{
    m_protocolsManager->registerProtocolFactory(m_facebookProtocolFactory);
}

void FacebookPluginObject::done()
{
    m_protocolsManager->unregisterProtocolFactory(m_facebookProtocolFactory);
}

#include "facebook-plugin-object.moc"
