/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_ACCOUNT
#define JABBER_ACCOUNT

#include <QtCore/QString>

#include "accounts/account-details.h"
#include "open-chat-with/jabber-open-chat-with-runner.h"
#include "protocols/protocol.h"

class JabberAccountDetails : public AccountDetails
{
	JabberOpenChatWithRunner *OpenChatRunner;

public:
	enum EncryptionFlag
	{
		Encryption_No = 0,
		Encryption_Yes = 1,
		Encryption_Auto = 2,
		Encryption_Legacy = 3
	};
	
	enum AllowPlainType
	{
		NoAllowPlain = 0,
		AllowPlain = 1,
		AllowPlainOverTLS = 2
	};

private:
	PROPERTY_DEC(QString, Resource)
	PROPERTY_DEC(bool, AutoResource)
	PROPERTY_DEC(int, Priority)
	PROPERTY_DEC(bool, UseCustomHostPort)
	PROPERTY_DEC(QString, CustomHost)
	PROPERTY_DEC(int, CustomPort)
	PROPERTY_DEC(EncryptionFlag, EncryptionMode)
	PROPERTY_DEC(AllowPlainType, PlainAuthMode)
	PROPERTY_DEC(QByteArray, TlsOverrideCert)
	PROPERTY_DEC(QString, TlsOverrideDomain)
	PROPERTY_DEC(QString, DataTransferProxy)
	PROPERTY_DEC(bool, SendTypingNotification)
	PROPERTY_DEC(bool, SendGoneNotification)
	PROPERTY_DEC(bool, PublishSystemInfo)
protected:
	virtual void load();
	virtual void store();

public:
	explicit JabberAccountDetails(AccountShared *data);
	virtual ~JabberAccountDetails();

	PROPERTY_DEF(QString, resource, setResource, Resource)
	PROPERTY_DEF(bool, autoResource, setAutoResource, AutoResource)
	PROPERTY_DEF(int, priority, setPriority, Priority)
	PROPERTY_DEF(bool, useCustomHostPort, setUseCustomHostPort, UseCustomHostPort)
	PROPERTY_DEF(QString, customHost, setCustomHost, CustomHost)
	PROPERTY_DEF(int, customPort, setCustomPort, CustomPort)
	PROPERTY_DEF(EncryptionFlag, encryptionMode, setEncryptionMode, EncryptionMode)
	PROPERTY_DEF(AllowPlainType, plainAuthMode, setPlainAuthMode, PlainAuthMode)
	PROPERTY_DEF(QByteArray, tlsOverrideCert, setTlsOverrideCert, TlsOverrideCert)
	PROPERTY_DEF(QString, tlsOverrideDomain, setTlsOverrideDomain, TlsOverrideDomain)
	PROPERTY_DEF(QString, dataTransferProxy, setDataTransferProxy, DataTransferProxy)
	PROPERTY_DEF(bool, sendTypingNotification, setSendTypingNotification, SendTypingNotification)
	PROPERTY_DEF(bool, sendGoneNotification, setSendGoneNotification, SendGoneNotification)
	PROPERTY_DEF(bool, publishSystemInfo, setPublishSystemInfo, PublishSystemInfo)

};

#endif // JABBER_ACCOUNT
