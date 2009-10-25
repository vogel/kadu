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

#include "configuration/storable-object.h"

class AccountDetails : public QObject, public StorableObject
{
	Q_OBJECT

	Account *MyAccount;

public:
	explicit AccountDetails(StoragePoint *storagePoint, Account *parent = 0);
	virtual ~AccountDetails();

	Account * account() { return MyAccount; }

};

#endif // ACCOUNT_DETAILS_H
