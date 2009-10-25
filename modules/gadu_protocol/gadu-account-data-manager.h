/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_ACCOUNT_DATA_MANAGER_H
#define GADU_ACCOUNT_DATA_MANAGER_H

#include "configuration/account-data-manager.h"

class Account;

class GaduAccountDataManager : public AccountDataManager
{
public:
	GaduAccountDataManager(Account *data);

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif // GADU_ACCOUNT_DATA_MANAGER_H
