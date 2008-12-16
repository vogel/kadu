/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_DATA_MANAGER_H
#define ACCOUNT_DATA_MANAGER_H

#include "configuration-window-data-manager.h"

class AccountData;

class KADUAPI AccountDataManager : public ConfigurationWindowDataManager
{
	AccountData *Data;

protected:
	AccountData * data() { return Data; }

public:
	AccountDataManager(AccountData* data);
	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);
};

#endif 
