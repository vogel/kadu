/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CONTACT_ACCOUNT_DATA_MANAGER_H
#define TLEN_CONTACT_ACCOUNT_DATA_MANAGER_H

#include "configuration/configuration-contact-account-data-manager.h"

class TlenContactAccountData;

class TlenContactAccountDataManager : public ConfigurationContactAccountDataManager
{
	TlenContactAccountData *Data;

protected:
	TlenContactAccountData * data() { return Data; }

public:
	explicit TlenContactAccountDataManager(TlenContactAccountData *data, QObject *parent = 0);

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif // TLEN_CONTACT_ACCOUNT_DATA_MANAGER_H
