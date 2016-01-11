/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "chat/chat.h"
#include "talkable/talkable.h"

class QAction;

class ActionDescription;
class ChatWidget;
class History;
class MenuInventory;
class MobileNumberManager;
class SmsDialogRepository;
class SmsGatewayManager;
class SmsScriptsManager;

class SmsActions : public QObject
{
	Q_OBJECT

	QPointer<History> m_history;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<MobileNumberManager> m_mobileNumberManager;
	QPointer<SmsDialogRepository> m_smsDialogRepository;
	QPointer<SmsGatewayManager> m_smsGatewayManager;
	QPointer<SmsScriptsManager> m_smsScriptsManager;

	ActionDescription *sendSmsActionDescription;

private slots:
	INJEQT_SET void setHistory(History *history);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setMobileNumberManager(MobileNumberManager *mobileNumberManager);
	INJEQT_SET void setSmsDialogRepository(SmsDialogRepository *smsDialogRepository);
	INJEQT_SET void setSmsGatewayManager(SmsGatewayManager *smsGatewayManager);
	INJEQT_SET void setSmsScriptsManager(SmsScriptsManager *smsScriptsManager);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void talkableActivated(const Talkable &talkable);
	void sendSmsActionActivated(QAction *sender);
	void newSms(const QString &mobile);

public:
	Q_INVOKABLE explicit SmsActions(QObject *parent = nullptr);
	virtual ~SmsActions();

};
