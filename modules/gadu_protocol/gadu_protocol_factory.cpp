/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QPushButton>

#include "gadu_account_data.h"
#include "gadu_configuration_dialog.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu_protocol_factory.h"

Protocol * GaduProtocolFactory::newInstance()
{
	return new GaduProtocol(0, this);
}

AccountData * GaduProtocolFactory::newAccountData()
{
	return new GaduAccountData();
}

ContactAccountData * GaduProtocolFactory::newContactAccountData(Contact contact, Account *account, const QString &id)
{
	return new GaduContactAccountData(contact, account, id);
}

ContactAccountData * GaduProtocolFactory::loadContactAccountData(Contact contact, Account *account)
{
	return contact.hasStoredAccountData(account)
		? new GaduContactAccountData(contact, account, QString::null)
		: 0;
}

GaduConfigurationDialog * GaduProtocolFactory::newConfigurationDialog(AccountData *accountData, QWidget *parent)
{
	GaduAccountData *gaduAccountData = dynamic_cast<GaduAccountData *>(accountData);

	return 0 != gaduAccountData
		? new GaduConfigurationDialog(gaduAccountData, parent)
		: 0;
}
