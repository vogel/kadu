/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_ACCOUNT_DETAILS_H
#define GADU_ACCOUNT_DETAILS_H

#include "accounts/account-details.h"

#include "open-chat-with/gadu-open-chat-with-runner.h"
#include "gadu-protocol.h"

class GaduAccountDetails : public AccountDetails
{
	PROPERTY_DEC(bool, AllowDcc)
	PROPERTY_DEC(QHostAddress, DccIP)
	PROPERTY_DEC(bool, DccIpDetect)
	PROPERTY_DEC(short int, DccPort)
	PROPERTY_DEC(QHostAddress, DccExternalIP)
	PROPERTY_DEC(short int, DccExternalPort)
	PROPERTY_DEC(short int, DccLocalPort)
	PROPERTY_DEC(bool, RemoveCompletedTransfers)
	PROPERTY_DEC(bool, DccForwarding)

	GaduOpenChatWithRunner *OpenChatRunner;

public:
	explicit GaduAccountDetails(AccountShared *data);
	virtual ~GaduAccountDetails();

	virtual void load();
	virtual void store();

	UinType uin();

	PROPERTY_DEF(bool, allowDcc, setAllowDcc, AllowDcc)
	PROPERTY_DEF(QHostAddress, dccIP, setDccIP, DccIP)
	PROPERTY_DEF(bool, dccIpDetect, setDccIpDetect, DccIpDetect)
	PROPERTY_DEF(short int, dccPort, setDccPort, DccPort)
	PROPERTY_DEF(QHostAddress, dccExternalIP, setDccExternalIP, DccExternalIP)
	PROPERTY_DEF(short int, dccExternalPort, setDccExternalPort, DccExternalPort)
	PROPERTY_DEF(short int, dccLocalPort, setDccLocalPort, DccLocalPort)
	PROPERTY_DEF(bool, removeCompletedTransfers, setRemoveCompletedTransfers, RemoveCompletedTransfers)
	PROPERTY_DEF(bool, dccForwarding, setDccForwarding, DccForwarding)

	void import_0_6_5_LastStatus();

};

#endif // GADU_ACCOUNT_DETAILS_H
