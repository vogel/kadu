/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/chat-top-bar-widget-factory.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class OtrChatTopBarWidget;
class OtrPeerIdentityVerificationWindowRepository;
class OtrSessionService;
class OtrTrustLevelService;

class OtrChatTopBarWidgetFactory : public QObject, public ChatTopBarWidgetFactory
{
	Q_OBJECT

public:
	Q_INVOKABLE OtrChatTopBarWidgetFactory();
	virtual ~OtrChatTopBarWidgetFactory();

	virtual QWidget * createWidget(const Chat &chat, QWidget *parent);

private slots:
	INJEQT_SETTER void setPeerIdentityVerificationWindowRepository(OtrPeerIdentityVerificationWindowRepository *peerIdentityVerificationWindowRepository);
	INJEQT_SETTER void setSessionService(OtrSessionService *sessionService);
	INJEQT_SETTER void setTrustLevelService(OtrTrustLevelService *trustLevelService);

	void widgetDestroyed(QObject *widget);

private:
	QPointer<OtrPeerIdentityVerificationWindowRepository> PeerIdentityVerificationWindowRepository;
	QPointer<OtrSessionService> SessionService;
	QPointer<OtrTrustLevelService> TrustLevelService;
	QList<OtrChatTopBarWidget *> Widgets;

};
