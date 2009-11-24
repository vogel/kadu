/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "account-details.h"

AccountDetails::AccountDetails(AccountShared *accountData) :
		StorableObject(accountData->storage()), MyAccountData(accountData)
{
}

AccountDetails::~AccountDetails()
{
}
