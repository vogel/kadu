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

#include "actions/gadu-protocol-menu-manager.h"
#include "helpers/gadu-importer.h"
#include "protocols/protocols-manager.h"
#include "gadu-protocol-factory.h"
#include "gadu-url-dom-visitor-provider.h"
#include "gadu-url-handler.h"

#include "accounts/account-manager.h"
#include "dom/dom-processor-service.h"
#include "gui/menu/menu-inventory.h"
#include "misc/memory.h"
#include "plugin/activation/plugin-activation-error-exception.h"
#include "url-handlers/url-handler-manager.h"
#include "debug.h"

#include <libgadu.h>

GaduPluginObject::GaduPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

GaduPluginObject::~GaduPluginObject()
{
}

void GaduPluginObject::setDomProcessorService(DomProcessorService *domProcessorService)
{
	m_domProcessorService = domProcessorService;
}

void GaduPluginObject::setGaduProtocolFactory(GaduProtocolFactory *gaduProtocolFactory)
{
	m_gaduProtocolFactory = gaduProtocolFactory;
}

void GaduPluginObject::setGaduProtocolMenuManager(GaduProtocolMenuManager *gaduProtocolMenuManager)
{
	m_gaduProtocolMenuManager = gaduProtocolMenuManager;
}

void GaduPluginObject::setGaduUrlHandler(GaduUrlHandler *gaduUrlHandler)
{
	m_gaduUrlHandler = gaduUrlHandler;
}

void GaduPluginObject::setGaduUrlDomVisitorProvider(GaduUrlDomVisitorProvider *gaduUrlDomVisitorProvider)
{
	m_gaduUrlDomVisitorProvider = gaduUrlDomVisitorProvider;
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
	gg_debug_level = debug_mask & 255;
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
	m_domProcessorService->registerVisitorProvider(m_gaduUrlDomVisitorProvider, 1000);

	auto importer = make_not_owned<GaduImporter>();
	if (AccountManager::instance()->allItems().isEmpty())
		importer->importAccounts();
	importer->importContacts();

	MenuInventory::instance()->registerProtocolMenuManager(m_gaduProtocolMenuManager);
}

void GaduPluginObject::done()
{
	MenuInventory::instance()->unregisterProtocolMenuManager(m_gaduProtocolMenuManager);
	m_domProcessorService->unregisterVisitorProvider(m_gaduUrlDomVisitorProvider);
	m_urlHandlerManager->unregisterUrlHandler(m_gaduUrlHandler);
	m_protocolsManager->unregisterProtocolFactory(m_gaduProtocolFactory);
}

#include "moc_gadu-plugin-object.cpp"
