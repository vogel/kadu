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

#include "gadu-plugin-object.h"

#include "protocols/protocols-manager.h"
#include "gadu-protocol-factory.h"
#include "gadu-url-dom-visitor-provider.h"
#include "gadu-url-handler.h"

#include "accounts/account-manager.h"
#include "dom/dom-visitor-provider-repository.h"
#include "misc/memory.h"
#include "plugin/activation/plugin-activation-error-exception.h"
#include "url-handlers/url-handler-manager.h"

#include <libgadu.h>

GaduPluginObject::GaduPluginObject(QObject *parent) :
		QObject{parent}
{
}

GaduPluginObject::~GaduPluginObject()
{
}

void GaduPluginObject::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void GaduPluginObject::setDomVisitorProviderRepository(DomVisitorProviderRepository *domVisitorProviderRepository)
{
	m_domVisitorProviderRepository = domVisitorProviderRepository;
}

void GaduPluginObject::setGaduProtocolFactory(GaduProtocolFactory *gaduProtocolFactory)
{
	m_gaduProtocolFactory = gaduProtocolFactory;
}

void GaduPluginObject::setGaduUrlDomVisitorProvider(GaduUrlDomVisitorProvider *gaduUrlDomVisitorProvider)
{
	m_gaduUrlDomVisitorProvider = gaduUrlDomVisitorProvider;
}

void GaduPluginObject::setGaduUrlHandler(GaduUrlHandler *gaduUrlHandler)
{
	m_gaduUrlHandler = gaduUrlHandler;
}

void GaduPluginObject::setProtocolsManager(ProtocolsManager *protocolsManager)
{
	m_protocolsManager = protocolsManager;
}

void GaduPluginObject::setUrlHandlerManager(UrlHandlerManager *urlHandlerManager)
{
	m_urlHandlerManager = urlHandlerManager;
}

void GaduPluginObject::init()
{
#ifdef DEBUG_OUTPUT_ENABLED
	// 8 bits for gadu debug
	gg_debug_level = 255;
#else
	gg_debug_level = 0;
#endif

	if (!gg_libgadu_check_feature(GG_LIBGADU_FEATURE_USERLIST100))
		throw PluginActivationErrorException("gadu_protocol", tr("Cannot load Gadu-Gadu Protocol plugin. Please compile libgadu with zlib support."));

	gg_proxy_host = 0;
	gg_proxy_username = 0;
	gg_proxy_password = 0;

	m_protocolsManager->registerProtocolFactory(m_gaduProtocolFactory);
	m_urlHandlerManager->registerUrlHandler(m_gaduUrlHandler);
	m_domVisitorProviderRepository->addVisitorProvider(m_gaduUrlDomVisitorProvider, 1000);
}

void GaduPluginObject::done()
{
	m_domVisitorProviderRepository->removeVisitorProvider(m_gaduUrlDomVisitorProvider);
	m_urlHandlerManager->unregisterUrlHandler(m_gaduUrlHandler);
	m_protocolsManager->unregisterProtocolFactory(m_gaduProtocolFactory);
}

#include "moc_gadu-plugin-object.cpp"
