/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_DETAILS_H
#define ACCOUNT_DETAILS_H

#include <QtCore/QObject>

#include "accounts/account-shared.h"
#include "storage/details.h"

#include "exports.h"

class AccountDetails : public Details<AccountShared>
{

public:
	explicit AccountDetails(AccountShared *mainData);
	virtual ~AccountDetails();

};

#endif // ACCOUNT_DETAILS_H
