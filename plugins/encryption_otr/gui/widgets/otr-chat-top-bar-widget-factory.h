/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef OTR_CHAT_TOP_BAR_WIDGET_FACTORY_H
#define OTR_CHAT_TOP_BAR_WIDGET_FACTORY_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "gui/widgets/chat-top-bar-widget-factory.h"

class OtrChatTopBarWidget;
class OtrPeerIdentityVerificationWindowRepository;
class OtrSessionService;
class OtrTrustLevelService;

class OtrChatTopBarWidgetFactory : public QObject, public ChatTopBarWidgetFactory
{
	Q_OBJECT

	QPointer<OtrPeerIdentityVerificationWindowRepository> PeerIdentityVerificationWindowRepository;
	QPointer<OtrSessionService> SessionService;
	QPointer<OtrTrustLevelService> TrustLevelService;
	QList<OtrChatTopBarWidget *> Widgets;

private slots:
	void widgetDestroyed(QObject *widget);

public:
	explicit OtrChatTopBarWidgetFactory(QObject *parent = 0);
	virtual ~OtrChatTopBarWidgetFactory();

	void setPeerIdentityVerificationWindowRepository(OtrPeerIdentityVerificationWindowRepository *peerIdentityVerificationWindowRepository);
	void setSessionService(OtrSessionService *sessionService);
	void setTrustLevelService(OtrTrustLevelService *trustLevelService);

	virtual QWidget * createWidget(const Chat &chat, QWidget *parent);

};

#endif // OTR_CHAT_TOP_BAR_WIDGET_FACTORY_H
