/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-plugin-object.h"
#include "jabber-plugin-object.moc"

#include "actions/jabber-actions.h"
#include "actions/jabber-protocol-menu-manager.h"
#include "gtalk-protocol-factory.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-dom-visitor-provider.h"
#include "jabber-url-handler.h"

#include "dom/dom-visitor-provider-repository.h"
#include "menu/menu-inventory.h"
#include "protocols/protocols-manager.h"
#include "url-handlers/url-handler-manager.h"

JabberPluginObject::JabberPluginObject(QObject *parent) : QObject{parent}
{
}

JabberPluginObject::~JabberPluginObject()
{
}

void JabberPluginObject::setDomVisitorProviderRepository(DomVisitorProviderRepository *domVisitorProviderRepository)
{
    m_domVisitorProviderRepository = domVisitorProviderRepository;
}

void JabberPluginObject::setGTalkProtocolFactory(GTalkProtocolFactory *gtalkProtocolFactory)
{
    m_gtalkProtocolFactory = gtalkProtocolFactory;
}

void JabberPluginObject::setJabberActions(JabberActions *jabberActions)
{
    m_jabberActions = jabberActions;
}

void JabberPluginObject::setJabberProtocolFactory(JabberProtocolFactory *jabberProtocolFactory)
{
    m_jabberProtocolFactory = jabberProtocolFactory;
}

void JabberPluginObject::setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager)
{
    m_jabberProtocolMenuManager = jabberProtocolMenuManager;
}

void JabberPluginObject::setJabberUrlHandler(JabberUrlHandler *jabberUrlHandler)
{
    m_jabberUrlHandler = jabberUrlHandler;
}

void JabberPluginObject::setMenuInventory(MenuInventory *menuInventory)
{
    m_menuInventory = menuInventory;
}

void JabberPluginObject::setJabberUrlDomVisitorProvider(JabberUrlDomVisitorProvider *jabberUrlDomVisitorProvider)
{
    m_jabberUrlDomVisitorProvider = jabberUrlDomVisitorProvider;
}

void JabberPluginObject::setProtocolsManager(ProtocolsManager *protocolsManager)
{
    m_protocolsManager = protocolsManager;
}

void JabberPluginObject::setUrlHandlerManager(UrlHandlerManager *urlHandlerManager)
{
    m_urlHandlerManager = urlHandlerManager;
}

void JabberPluginObject::init()
{
    m_protocolsManager->registerProtocolFactory(m_jabberProtocolFactory);
    m_protocolsManager->registerProtocolFactory(m_gtalkProtocolFactory);
    m_urlHandlerManager->registerUrlHandler(m_jabberUrlHandler);
    // install before mail handler
    m_domVisitorProviderRepository->addVisitorProvider(m_jabberUrlDomVisitorProvider, 2000);

    m_menuInventory->registerProtocolMenuManager(m_jabberProtocolMenuManager);
}

void JabberPluginObject::done()
{
    m_menuInventory->unregisterProtocolMenuManager(m_jabberProtocolMenuManager);

    m_domVisitorProviderRepository->removeVisitorProvider(m_jabberUrlDomVisitorProvider);
    m_urlHandlerManager->unregisterUrlHandler(m_jabberUrlHandler);
    m_protocolsManager->registerProtocolFactory(m_gtalkProtocolFactory);
    m_protocolsManager->unregisterProtocolFactory(m_jabberProtocolFactory);
}
