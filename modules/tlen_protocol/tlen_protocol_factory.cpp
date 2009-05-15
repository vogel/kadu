/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/widgets/tlen-create-account-widget.h"

#include "tlen_account.h"
#include "tlen_configuration_dialog.h"
#include "tlen_protocol.h"
#include "tlen-contact-account-data.h"
#include "tlen-contact-account-data-widget.h"

#include "tlen_protocol_factory.h"

TlenProtocolFactory *TlenProtocolFactory::Instance = 0;

TlenProtocolFactory * TlenProtocolFactory::instance()
{
	if (!Instance)
		Instance = new TlenProtocolFactory();

	return Instance;
}

TlenProtocolFactory::TlenProtocolFactory()
{
}

Account * TlenProtocolFactory::newAccount()
{
	TlenAccount *account = new TlenAccount();
	Protocol *protocol = new TlenProtocol(account, this);
	return account;
}

Account * TlenProtocolFactory::loadAccount(StoragePoint *accountStoragePoint)
{
       TlenAccount *account = new TlenAccount();
       account->setStorage(accountStoragePoint);
       Protocol *protocol = new TlenProtocol(account, this);
       account->load();

       return account;
}

ContactAccountData * TlenProtocolFactory::newContactAccountData(Contact contact, Account *account, const QString &id, bool loadFromConfiguration)
{
	return new TlenContactAccountData(contact, account, id, loadFromConfiguration);
}

ContactAccountData * TlenProtocolFactory::loadContactAccountData(Contact contact, Account *account)
{
	return contact.hasStoredAccountData(account)
		? new TlenContactAccountData(contact, account, QString::null)
		: 0;
}

QWidget * TlenProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
    	return new TlenCreateAccountWidget(parent);
}

TlenConfigurationDialog * TlenProtocolFactory::newConfigurationDialog(Account *account, QWidget *parent)
{
	TlenAccount *tlenAccount = dynamic_cast<TlenAccount *>(account);

	return 0 != tlenAccount
		? new TlenConfigurationDialog(tlenAccount, parent)
		: 0;
}

ContactAccountDataWidget * TlenProtocolFactory::newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent)
{
	TlenContactAccountData *tlenContactAccountData = dynamic_cast<TlenContactAccountData *>(contactAccountData);

	return 0 != tlenContactAccountData
		? new TlenContactAccountDataWidget(tlenContactAccountData, parent)
		: 0;
}
