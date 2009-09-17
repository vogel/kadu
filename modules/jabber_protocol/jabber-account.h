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

#include "accounts/account.h"
#include "open-chat-with/jabber-open-chat-with-runner.h"
#include "protocols/protocol.h"

class JabberAccount : public Account
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
	QString Jid;
	QString Resource;
	bool AutoResource;
	int Priority;
	bool UseCustomHostPort;
	QString CustomHost;
	int CustomPort;
	EncryptionFlag EncryptionMode;
	bool LegacySSLProbe;

public:
	explicit JabberAccount(const QUuid &uuid = QUuid());
	virtual ~JabberAccount();

	// TODO - jid is always null - why?
	// QString jid() { return Jid; }
	QByteArray TlsOverrideCert;
	QString TlsOverrideDomain;

	QString resource() { return Resource; }
	void setResource(const QString &resource) { Resource = resource; }

	bool autoResource() { return AutoResource; }
	void setAutoResource(bool autoResource) { AutoResource = autoResource; }

	int priority() { return Priority; }
	void setPriority(const int &priority) { Priority = priority; }

	bool useCustomHostPort() { return UseCustomHostPort; }
	void setUseCustomHostPort(bool use) { UseCustomHostPort = use; }

	QString customHost() { return CustomHost; }
	void setCustomHost(QString host) { CustomHost = host; }

	int customPort() { return CustomPort; }
	void setCustomPort(int port) { CustomPort = port; }

	EncryptionFlag encryptionMode() { return EncryptionMode; }
	void setEncryptionMode(EncryptionFlag flag) { EncryptionMode = flag; }

	bool legacySSLProbe() { return LegacySSLProbe; }
	void setLegacySSLProbe(bool probe) { LegacySSLProbe = probe; }

	QByteArray &tlsOverrideCert() { return TlsOverrideCert; }
	void setTlsOverrideCert(QByteArray tlsOverrideCert) { TlsOverrideCert = tlsOverrideCert; }

	QString &tlsOverrideDomain() { return TlsOverrideDomain; }
	void setTlsOverrideDomain(QString tlsOverrideDomain) { TlsOverrideDomain = tlsOverrideDomain; }

	virtual bool setId(const QString &id);

	virtual void load();
	virtual void store();
};

#endif // JABBER_ACCOUNT
