/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QtCore/QObject>

#include "status.h"

class AccountData;
class Protocol;

class Account : public QObject
{
	Q_OBJECT

	Protocol *ProtocolHandler;
	AccountData *Data;

public:
	Account(Protocol *protocol, AccountData *data);
	virtual ~Account();

	Protocol * protocol() { return ProtocolHandler; }

	UserStatus currentStatus();

};

#endif // ACCOUNT_H
