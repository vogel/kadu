/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_ACCOUNT_DATA_MANAGER_H
#define TLEN_ACCOUNT_DATA_MANAGER_H

#include "configuration/account-data-manager.h"

class TlenAccountDetails;

class TlenAccountDataManager : public AccountDataManager
{
	TlenAccountDetails *Data;

public:
	TlenAccountDataManager(Account data);

	TlenAccountDetails * data() { return Data; }

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};
#endif // TLEN_ACCOUNT_DATA_MANAGER_H
