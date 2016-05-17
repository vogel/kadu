/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-details.h"

#include "open-chat-with/gadu-open-chat-with-runner.h"
#include "gadu-protocol.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injector.h>

class InjectedFactory;

class GaduAccountDetails : public AccountDetails
{
	Q_OBJECT

	QPointer<InjectedFactory> m_injectedFactory;

	PROPERTY_DEC(bool, ReceiveImagesDuringInvisibility)
	PROPERTY_DEC(bool, ChatImageSizeWarning)
	PROPERTY_DEC(bool, InitialRosterImport)
	PROPERTY_DEC(bool, SendTypingNotification)
	PROPERTY_DEC(int, UserlistVersion)
	PROPERTY_DEC(bool, ReceiveSpam)

	GaduOpenChatWithRunner *OpenChatRunner;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

protected:
	virtual void load();
	virtual void store();

public:
	explicit GaduAccountDetails(AccountShared *data, QObject *parent = nullptr);
	virtual ~GaduAccountDetails();

	UinType uin();

	PROPERTY_DEF(bool, receiveImagesDuringInvisibility, setReceiveImagesDuringInvisibility, ReceiveImagesDuringInvisibility)
	PROPERTY_DEF(bool, chatImageSizeWarning, setChatImageSizeWarning, ChatImageSizeWarning)
	PROPERTY_DEF(bool, initialRosterImport, setInitialRosterImport, InitialRosterImport)
	PROPERTY_DEF(bool, sendTypingNotification, setSendTypingNotification, SendTypingNotification)
	PROPERTY_DEF(int, userlistVersion, setUserlistVersion, UserlistVersion)
	PROPERTY_DEF(bool, receiveSpam, setReceiveSpam, ReceiveSpam)

};
