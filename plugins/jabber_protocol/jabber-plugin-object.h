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

#include "injeqt-type-roles.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class DomProcessorService;
class GTalkProtocolFactory;
class JabberActions;
class JabberProtocolFactory;
class JabberProtocolMenuManager;
class JabberUrlDomVisitorProvider;
class JabberUrlHandler;
class MenuInventory;
class ProtocolsManager;
class UrlHandlerManager;

class JabberPluginObject : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(PLUGIN)

public:
	Q_INVOKABLE explicit JabberPluginObject(QObject *parent = nullptr);
	virtual ~JabberPluginObject();

private:
	QPointer<DomProcessorService> m_domProcessorService;
	QPointer<GTalkProtocolFactory> m_gtalkProtocolFactory;
	QPointer<JabberActions> m_jabberActions;
	QPointer<JabberProtocolFactory> m_jabberProtocolFactory;
	QPointer<JabberProtocolMenuManager> m_jabberProtocolMenuManager;
	QPointer<JabberUrlDomVisitorProvider> m_jabberUrlDomVisitorProvider;
	QPointer<JabberUrlHandler> m_jabberUrlHandler;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<ProtocolsManager> m_protocolsManager;
	QPointer<UrlHandlerManager> m_urlHandlerManager;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SET void setGTalkProtocolFactory(GTalkProtocolFactory *gtalkProtocolFactory);
	INJEQT_SET void setJabberActions(JabberActions *jabberActions);
	INJEQT_SET void setJabberProtocolFactory(JabberProtocolFactory *jabberProtocolFactory);
	INJEQT_SET void setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager);
	INJEQT_SET void setJabberUrlDomVisitorProvider(JabberUrlDomVisitorProvider *jabberUrlDomVisitorProvider);
	INJEQT_SET void setJabberUrlHandler(JabberUrlHandler *jabberUrlHandler);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setProtocolsManager(ProtocolsManager *protocolsManager);
	INJEQT_SET void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);

};
