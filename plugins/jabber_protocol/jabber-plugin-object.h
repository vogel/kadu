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
class GTalkProtocolFactory;
class JabberActions;
class JabberProtocolFactory;
class JabberProtocolMenuManager;
class JabberUrlDomVisitorProvider;
class JabberUrlHandler;
class ProtocolsManager;
class UrlHandlerManager;

class JabberPluginObject : public PluginObject
{
	Q_OBJECT

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
	QPointer<ProtocolsManager> m_protocolsManager;
	QPointer<UrlHandlerManager> m_urlHandlerManager;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SETTER void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SETTER void setGTalkProtocolFactory(GTalkProtocolFactory *gtalkProtocolFactory);
	INJEQT_SETTER void setJabberActions(JabberActions *jabberActions);
	INJEQT_SETTER void setJabberProtocolFactory(JabberProtocolFactory *jabberProtocolFactory);
	INJEQT_SETTER void setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager);
	INJEQT_SETTER void setJabberUrlDomVisitorProvider(JabberUrlDomVisitorProvider *jabberUrlDomVisitorProvider);
	INJEQT_SETTER void setJabberUrlHandler(JabberUrlHandler *jabberUrlHandler);
	INJEQT_SETTER void setProtocolsManager(ProtocolsManager *protocolsManager);
	INJEQT_SETTER void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);

};
