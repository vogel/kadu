/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

private:
	PROPERTY_DEC(QString, Resource)
	PROPERTY_DEC(bool, AutoResource)
	PROPERTY_DEC(int, Priority)
	PROPERTY_DEC(bool, UseCustomHostPort)
	PROPERTY_DEC(QString, CustomHost)
	PROPERTY_DEC(int, CustomPort)
	PROPERTY_DEC(EncryptionFlag, EncryptionMode)
	PROPERTY_DEC(bool, LegacySSLProbe)
	PROPERTY_DEC(QByteArray, TlsOverrideCert)
	PROPERTY_DEC(QString, TlsOverrideDomain)

public:
	explicit JabberAccountDetails(StoragePoint *storagePoint, Account *parent = 0);
	virtual ~JabberAccountDetails();

	virtual void load();
	virtual void store();

	PROPERTY_DEF(QString, resource, setResource, Resource)
	PROPERTY_DEF(bool, autoResource, setAutoResource, AutoResource)
	PROPERTY_DEF(int, priority, setPriority, Priority)
	PROPERTY_DEF(bool, useCustomHostPort, setUseCustomHostPort, UseCustomHostPort)
	PROPERTY_DEF(QString, customHost, setCustomHost, CustomHost)
	PROPERTY_DEF(int, customPort, setCustomPort, CustomPort)
	PROPERTY_DEF(EncryptionFlag, encryptionMode, setEncryptionMode, EncryptionMode)
	PROPERTY_DEF(bool, legacySSLProbe, setLegacySSLProbe, LegacySSLProbe)
	PROPERTY_DEF(QByteArray, tlsOverrideCert, setTlsOverrideCert, TlsOverrideCert)
	PROPERTY_DEF(QString, tlsOverrideDomain, setTlsOverrideDomain, TlsOverrideDomain)

};

#endif // JABBER_ACCOUNT
