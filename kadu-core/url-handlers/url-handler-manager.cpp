/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "mail-url-handler.h"
#include "standard-url-handler.h"
#include "url-handler.h"

#include "url-handler-manager.h"

UrlHandlerManager * UrlHandlerManager::Instance = 0;

UrlHandlerManager * UrlHandlerManager::instance()
{
	if (0 == Instance)
		Instance = new UrlHandlerManager();

	return Instance;
}

UrlHandlerManager::UrlHandlerManager()
{
	standardUrlHandler = new StandardUrlHandler();
	registerUrlHandler("Standard", standardUrlHandler);

	mailUrlHandler = new MailUrlHandler();
	registerUrlHandler("Mail", mailUrlHandler);
}

UrlHandlerManager::~UrlHandlerManager()
{
	qDeleteAll(RegisteredHandlers);
	RegisteredHandlers.clear();
}

void UrlHandlerManager::registerUrlHandler(const QString &name, UrlHandler *handler)
{
	if (0 != handler && !RegisteredHandlers.contains(name))
		RegisteredHandlers[name] = handler;
}

void UrlHandlerManager::unregisterUrlHandler(const QString &name)
{
	if (RegisteredHandlers.contains(name))
	{
		delete RegisteredHandlers[name];
		RegisteredHandlers.remove(name);
	}
}
void UrlHandlerManager::convertAllUrls(HtmlDocument &document)
{
	foreach (UrlHandler *handler, RegisteredHandlers)
		handler->convertUrlsToHtml(document);
}

void UrlHandlerManager::openUrl(const QString &url, bool disableMenu)
{
	foreach (UrlHandler *handler, RegisteredHandlers)
	{
		if (handler->isUrlValid(url))
		{
			handler->openUrl(url, disableMenu);
			return;
		}
	}
}

const QRegExp & UrlHandlerManager::mailRegExp()
{
	return mailUrlHandler->mailRegExp();
}

const QRegExp & UrlHandlerManager::urlRegExp()
{
	return standardUrlHandler->urlRegExp();
}
