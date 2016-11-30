/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXml/QDomDocument>

#include "configuration/configuration.h"
#include "dom/dom-visitor-provider-repository.h"
#include "gui/services/clipboard-html-transformer-service.h"
#include "os/generic/url-opener.h"
#include "url-handlers/mail-url-dom-visitor-provider.h"
#include "url-handlers/mail-url-handler.h"
#include "url-handlers/standard-url-dom-visitor-provider.h"
#include "url-handlers/standard-url-handler.h"
#include "url-handlers/url-clipboard-html-transformer.h"

#include "url-handler-manager.h"

UrlHandlerManager::UrlHandlerManager(QObject *parent) :
		QObject{parent}
{
	// NOTE: StandardUrlHandler has to be the first one to fix bug #1894
	registerUrlHandler(&standardUrlHandler);
	registerUrlHandler(&mailUrlHandler);
}

UrlHandlerManager::~UrlHandlerManager()
{
}

void UrlHandlerManager::setClipboardHtmlTransformerService(ClipboardHtmlTransformerService *clipboardHtmlTransformerService)
{
	m_clipboardHtmlTransformerService = clipboardHtmlTransformerService;
}

void UrlHandlerManager::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void UrlHandlerManager::setDomVisitorProviderRepository(DomVisitorProviderRepository *domVisitorProviderRepository)
{
	m_domVisitorProviderRepository = domVisitorProviderRepository;
}

void UrlHandlerManager::setUrlOpener(UrlOpener *urlOpener)
{
	m_urlOpener = urlOpener;
}

void UrlHandlerManager::init()
{
	ClipboardTransformer.reset(new UrlClipboardHtmlTransformer());
	m_clipboardHtmlTransformerService->registerTransformer(ClipboardTransformer.data());

	StandardUrlVisitorProvider = new StandardUrlDomVisitorProvider(m_configuration);
	m_domVisitorProviderRepository->addVisitorProvider(StandardUrlVisitorProvider, 0);

	MailUrlVisitorProvider = new MailUrlDomVisitorProvider();
	m_domVisitorProviderRepository->addVisitorProvider(MailUrlVisitorProvider, 500);
}

void UrlHandlerManager::done()
{
	m_clipboardHtmlTransformerService->unregisterTransformer(ClipboardTransformer.data());
	ClipboardTransformer.reset();

	m_domVisitorProviderRepository->removeVisitorProvider(StandardUrlVisitorProvider);
	m_domVisitorProviderRepository->removeVisitorProvider(MailUrlVisitorProvider);

	delete StandardUrlVisitorProvider;
	delete MailUrlVisitorProvider;
}

void UrlHandlerManager::registerUrlHandler(UrlHandler *handler)
{
	RegisteredHandlers.append(handler);
}

void UrlHandlerManager::unregisterUrlHandler(UrlHandler *handler)
{
	RegisteredHandlers.removeAll(handler);
}

void UrlHandlerManager::openUrl(const QByteArray &url, bool disableMenu)
{
	foreach (UrlHandler *handler, RegisteredHandlers)
	{
		if (handler->isUrlValid(url))
		{
			handler->openUrl(m_urlOpener, url, disableMenu);
			return;
		}
	}
}

const QRegExp & UrlHandlerManager::mailRegExp()
{
	return mailUrlHandler.mailRegExp();
}

const QRegExp & UrlHandlerManager::urlRegExp()
{
	return standardUrlHandler.urlRegExp();
}
