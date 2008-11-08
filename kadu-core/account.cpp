/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "protocol.h"

#include "account.h"

Account::Account(Protocol *protocol, AccountData *data)
	: ProtocolHandler(protocol), Data(data)
{
	protocol->setData(Data);
}

Account::~Account()
{
	if (0 != ProtocolHandler)
	{
		delete ProtocolHandler;
		ProtocolHandler = 0;
	}
}

UserStatus Account::currentStatus()
{
	return (0 == ProtocolHandler) ? UserStatus() : ProtocolHandler->currentStatus();
}
