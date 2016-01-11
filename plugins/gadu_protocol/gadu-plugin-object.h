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

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class DomProcessorService;
class GaduProtocolFactory;
class GaduProtocolMenuManager;
class GaduUrlDomVisitorProvider;
class GaduUrlHandler;
class MenuInventory;
class ProtocolsManager;
class UrlHandlerManager;

class GaduPluginObject : public QObject
{
	Q_OBJECT
	INJEQT_INSTANCE_IMMEDIATE

public:
	Q_INVOKABLE explicit GaduPluginObject(QObject *parent = nullptr);
	virtual ~GaduPluginObject();

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<DomProcessorService> m_domProcessorService;
	QPointer<GaduProtocolFactory> m_gaduProtocolFactory;
	QPointer<GaduProtocolMenuManager> m_gaduProtocolMenuManager;
	QPointer<GaduUrlDomVisitorProvider> m_gaduUrlDomVisitorProvider;
	QPointer<GaduUrlHandler> m_gaduUrlHandler;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<ProtocolsManager> m_protocolsManager;
	QPointer<UrlHandlerManager> m_urlHandlerManager;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SET void setGaduProtocolFactory(GaduProtocolFactory *gaduProtocolFactory);
	INJEQT_SET void setGaduProtocolMenuManager(GaduProtocolMenuManager *gaduProtocolMenuManager);
	INJEQT_SET void setGaduUrlDomVisitorProvider(GaduUrlDomVisitorProvider *gaduUrlDomVisitorProvider);
	INJEQT_SET void setGaduUrlHandler(GaduUrlHandler *gaduUrlHandler);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setProtocolsManager(ProtocolsManager *protocolsManager);
	INJEQT_SET void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);

};
