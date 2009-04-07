/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_ACCOUNT
#define GADU_ACCOUNT

#include "accounts/account.h"

#include "open-chat-with/gadu-open-chat-with-runner.h"
#include "gadu-protocol.h"

class GaduAccount : public Account
{
	UinType Uin;
	bool AllowDcc;
	QHostAddress DccIP;
	bool DccIpDetect;
	short int DccPort;
	QHostAddress DccExternalIP;
	short int DccExternalPort;
	short int DccLocalPort;
	bool RemoveCompletedTransfers;
	bool DccForwarding;
	GaduOpenChatWithRunner *OpenChatRunner;

public:
	explicit GaduAccount(const QUuid &uuid = QUuid());
	virtual ~GaduAccount();

	virtual void loadConfiguration();
	virtual void store();

	UinType uin() { return Uin; }
	bool allowDCC() { return AllowDcc; }
	QHostAddress dccIP() { return DccIP; }
	QHostAddress dccExternalIP() { return DccExternalIP; }
	bool dccIpDetect() { return DccIpDetect; }
	short int dccPort() { return DccPort; }
	short int dccExternalPort() { return DccExternalPort; }
	short int dccLocalPort() { return DccLocalPort; }
	bool removeCompletedTransfers() { return RemoveCompletedTransfers; }
	bool dccForwarding() { return DccForwarding; }

	virtual bool setId(const QString &id);
	void setAllowDcc(bool allow) { AllowDcc = allow; }
	void setDccIP(QHostAddress ip) { DccIP = ip; }
	void setDccExternalIP(QHostAddress ip) { DccExternalIP = ip; }
	void setDccIpDetect(bool detect) { DccIpDetect = detect; }
	void setDccPort(short int port) { DccPort = port; }
	void setDccExternalPort(short int port) { DccExternalPort = port; }
	void setDccLocalPort(short int port) { DccLocalPort = port; }
	void setRemoveCompletedTransfers(bool remove) { RemoveCompletedTransfers = remove; }
	void setDccForwarding(bool forwarding) { DccForwarding = forwarding; }

};

#endif // GADU_ACCOUNT
