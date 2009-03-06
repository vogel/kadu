/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "account-data-manager.h"

AccountDataManager::AccountDataManager(Account *data)
	: Data(data)
{
}

void AccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Account")
		return;

	if (name == "Name")
		Data->setName(value.value<QString>());

	if (name == "Id")
		Data->setId(value.value<QString>());

	if (name == "Password")
		Data->setPassword(value.value<QString>());
}

QVariant AccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Account")
		return QVariant(QString::null);

	if (name == "Name")
		return Data->name();

	if (name == "Id")
		return Data->id();

	if (name == "Password")
		return Data->password();

	return QVariant(QString::null);
}
