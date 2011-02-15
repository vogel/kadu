/*
 * %kadu copyright begin%
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef GADU_ACCOUNT_DETAILS_H
#define GADU_ACCOUNT_DETAILS_H

#include "accounts/account-details.h"

#include "open-chat-with/gadu-open-chat-with-runner.h"
#include "gadu-features.h"
#include "gadu-protocol.h"

class GaduAccountDetails : public AccountDetails
{
	PROPERTY_DEC(bool, AllowDcc)
	PROPERTY_DEC(short int, MaximumImageSize)
	PROPERTY_DEC(bool, ReceiveImagesDuringInvisibility)
	PROPERTY_DEC(short int, MaximumImageRequests)
	PROPERTY_DEC(bool, InitialRosterImport)

#ifdef GADU_HAVE_TLS
	PROPERTY_DEC(bool, TlsEncryption)
#endif

#ifdef GADU_HAVE_TYPING_NOTIFY
	PROPERTY_DEC(bool, SendTypingNotification)
#endif // GADU_HAVE_TYPING_NOTIFY

	GaduOpenChatWithRunner *OpenChatRunner;

protected:
	virtual void load();

public:
	explicit GaduAccountDetails(AccountShared *data);
	virtual ~GaduAccountDetails();

	virtual void store();

	UinType uin();

	PROPERTY_DEF(bool, allowDcc, setAllowDcc, AllowDcc)
	PROPERTY_DEF(short int, maximumImageSize, setMaximumImageSize, MaximumImageSize)
	PROPERTY_DEF(bool, receiveImagesDuringInvisibility, setReceiveImagesDuringInvisibility, ReceiveImagesDuringInvisibility)
	PROPERTY_DEF(short int, maximumImageRequests, setMaximumImageRequests, MaximumImageRequests)
	PROPERTY_DEF(bool, initialRosterImport, setInitialRosterImport, InitialRosterImport)

#ifdef GADU_HAVE_TLS
	PROPERTY_DEF(bool, tlsEncryption, setTlsEncryption, TlsEncryption)
#endif // GADU_HAVE_TLS

#ifdef GADU_HAVE_TYPING_NOTIFY
	PROPERTY_DEF(bool, sendTypingNotification, setSendTypingNotification, SendTypingNotification)
#endif // GADU_HAVE_TYPING_NOTIFY

	void import_0_6_5_LastStatus();

};

#endif // GADU_ACCOUNT_DETAILS_H
