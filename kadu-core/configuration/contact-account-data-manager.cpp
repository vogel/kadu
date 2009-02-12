/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-account-data.h"

#include "contact-account-data-manager.h"

ContactAccountDataManager::ContactAccountDataManager(ContactAccountData *data)
	: Data(data)
{
}

// TODO: add rest data

void ContactAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Contact")
		return;

	if (name == "Blocked")
		Data->setBlocked(value.value<bool>());

	if (name == "OfflineTo")
		Data->setOfflineTo(value.value<bool>());
}

QVariant ContactAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Contact")
		return QVariant(QString::null);

	if (name == "Id")
		return Data->id();

	if (name == "Blocked")
		return Data->isBlocked();

	if (name == "OfflineTo")
		return Data->isOfflineTo();

	return QVariant(QString::null);
}
