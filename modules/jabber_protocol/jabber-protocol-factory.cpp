/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/widgets/jabber-create-account-widget.h"

#include "jabber-account.h"
#include "jabber-configuration-dialog.h"
#include "jabber-contact-account-data-widget.h"
#include "jabber_protocol.h"
#include "jabber-contact-account-data.h"
#include "misc/misc.h"

#include "jabber-protocol-factory.h"

JabberProtocolFactory * JabberProtocolFactory::Instance = 0;

JabberProtocolFactory * JabberProtocolFactory::instance()
{
	if (!Instance)
		Instance = new JabberProtocolFactory();

	return Instance;
}

JabberProtocolFactory::JabberProtocolFactory()
{
}

Account * JabberProtocolFactory::newAccount()
{
	JabberAccount *account = new JabberAccount();
	Protocol *protocol = new JabberProtocol(account, this);
	return account;
}

QString JabberProtocolFactory::iconName()
{
	return dataPath("kadu/modules/data/jabber_protocol/").append("online.png");
}

Account * JabberProtocolFactory::loadAccount(StoragePoint *accountStoragePoint)
{
	JabberAccount *account = new JabberAccount();
	account->setStorage(accountStoragePoint);
	Protocol *protocol = new JabberProtocol(account, this);
	account->load();

	return account;
}

ContactAccountData * JabberProtocolFactory::newContactAccountData(Contact contact, Account *account, const QString &id, bool loadFromConfiguration)
{
	return new JabberContactAccountData(contact, account, id, loadFromConfiguration);
}

ContactAccountData * JabberProtocolFactory::loadContactAccountData(Contact contact, Account *account)
{
	return contact.hasStoredAccountData(account)
		? new JabberContactAccountData(contact, account, QString::null)
		: 0;
}

QWidget * JabberProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
	return new JabberCreateAccountWidget(parent);
}

JabberConfigurationDialog * JabberProtocolFactory::newConfigurationDialog(Account *account, QWidget *parent)
{
	JabberAccount *jabberAccount = dynamic_cast<JabberAccount *>(account);

	return 0 != jabberAccount
		? new JabberConfigurationDialog(jabberAccount, parent)
		: 0;
}

ContactAccountDataWidget * JabberProtocolFactory::newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent)
{
	JabberContactAccountData *jabberContactAccountData = dynamic_cast<JabberContactAccountData *>(contactAccountData);

	return 0 != jabberContactAccountData
		? new JabberContactAccountDataWidget(jabberContactAccountData, parent)
		: 0;
}
