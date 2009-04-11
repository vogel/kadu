/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QPushButton>

#include "gadu-account.h"
#include "gadu-contact-account-data-widget.h"
#include "gadu-configuration-dialog.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu-protocol-factory.h"

GaduProtocolFactory *GaduProtocolFactory::Instance = 0;

GaduProtocolFactory * GaduProtocolFactory::instance()
{
	if (!Instance)
		Instance = new GaduProtocolFactory();

	return Instance;
}

GaduProtocolFactory::GaduProtocolFactory()
{
}

Account * GaduProtocolFactory::newAccount()
{
	GaduAccount *account = new GaduAccount();
	Protocol *protocol = new GaduProtocol(account, this);
	return account;
}

Account * GaduProtocolFactory::loadAccount(StoragePoint *accountStoragePoint)
{
	GaduAccount *account = new GaduAccount();
	account->setStorage(accountStoragePoint);
	Protocol *protocol = new GaduProtocol(account, this);
	account->loadConfiguration();

	return account;
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

GaduConfigurationDialog * GaduProtocolFactory::newConfigurationDialog(Account *account, QWidget *parent)
{
	GaduAccount *gaduAccount = dynamic_cast<GaduAccount *>(account);

	return 0 != gaduAccount
		? new GaduConfigurationDialog(gaduAccount, parent)
		: 0;
}

ContactAccountDataWidget * GaduProtocolFactory::newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent)
{
	GaduContactAccountData *gaduContactAccountData = dynamic_cast<GaduContactAccountData *>(contactAccountData);

	return 0 != gaduContactAccountData
		? new GaduContactAccountDataWidget(gaduContactAccountData, parent)
		: 0;
}
