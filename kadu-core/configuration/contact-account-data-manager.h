/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_ACCOUNT_DATA_MANAGER_H
#define CONTACT_ACCOUNT_DATA_MANAGER_H

#include "configuration-window-data-manager.h"

class ContactAccountData;

class KADUAPI ContactAccountDataManager : public ConfigurationWindowDataManager
{
	ContactAccountData *Data;

protected:
	ContactAccountData * data() { return Data; }

public:
	explicit ContactAccountDataManager(ContactAccountData *data, QObject *parent = 0);

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif 
