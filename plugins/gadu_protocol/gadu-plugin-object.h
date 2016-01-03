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

#pragma once

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class DomProcessorService;
class GaduProtocolFactory;
class GaduProtocolMenuManager;
class GaduUrlDomVisitorProvider;
class GaduUrlHandler;
class ProtocolsManager;
class UrlHandlerManager;

class GaduPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit GaduPluginObject(QObject *parent = nullptr);
	virtual ~GaduPluginObject();

private:
	QPointer<DomProcessorService> m_domProcessorService;
	QPointer<GaduProtocolFactory> m_gaduProtocolFactory;
	QPointer<GaduProtocolMenuManager> m_gaduProtocolMenuManager;
	QPointer<GaduUrlDomVisitorProvider> m_gaduUrlDomVisitorProvider;
	QPointer<GaduUrlHandler> m_gaduUrlHandler;
	QPointer<ProtocolsManager> m_protocolsManager;
	QPointer<UrlHandlerManager> m_urlHandlerManager;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SETTER void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SETTER void setGaduProtocolFactory(GaduProtocolFactory *gaduProtocolFactory);
	INJEQT_SETTER void setGaduProtocolMenuManager(GaduProtocolMenuManager *gaduProtocolMenuManager);
	INJEQT_SETTER void setGaduUrlDomVisitorProvider(GaduUrlDomVisitorProvider *gaduUrlDomVisitorProvider);
	INJEQT_SETTER void setGaduUrlHandler(GaduUrlHandler *gaduUrlHandler);
	INJEQT_SETTER void setProtocolsManager(ProtocolsManager *protocolsManager);
	INJEQT_SETTER void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);

};
