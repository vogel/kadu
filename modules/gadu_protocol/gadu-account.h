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

#include "gadu-protocol.h"

class GaduAccount : public Account
{
	UinType Uin;

public:
	GaduAccount(const QUuid &uuid = QUuid());

	UinType uin() { return Uin; }

	virtual bool setId(const QString &id);

};

#endif // GADU_ACCOUNT
