/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_ACCOUNT_DATA_MANAGER_H
#define JABBER_ACCOUNT_DATA_MANAGER_H

#include "configuration/account-data-manager.h"

class JabberAccount;

class JabberAccountDataManager : public AccountDataManager
{
	JabberAccount *Data;
public:
	JabberAccountDataManager(JabberAccount *data);

	JabberAccount * data() { return Data; }

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif // JABBER_ACCOUNT_DATA_MANAGER_H
