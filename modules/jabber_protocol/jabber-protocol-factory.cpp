/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "gui/widgets/jabber-contact-account-data-widget.h"
#include "gui/widgets/jabber-create-account-widget.h"
#include "gui/widgets/jabber-edit-account-widget.h"
#include "jabber-account.h"
#include "jabber-contact-account-data.h"
#include "jabber-protocol.h"
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
	StatusTypeManager *statusTypeManager = StatusTypeManager::instance();
	SupportedStatusTypes.append(statusTypeManager->statusType("Online"));
	SupportedStatusTypes.append(statusTypeManager->statusType("FreeForChat"));
	SupportedStatusTypes.append(statusTypeManager->statusType("DoNotDisturb"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Away"));
	SupportedStatusTypes.append(statusTypeManager->statusType("NotAvailable"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Invisible"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Offline"));

	qSort(SupportedStatusTypes.begin(), SupportedStatusTypes.end(), StatusType::lessThan);

	IdRegularExpression.setPattern("[a-zA-Z0-9\\._-]+@[a-zA-Z0-9\\._-]+");
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

AccountCreateWidget * JabberProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
	return new JabberCreateAccountWidget(parent);
}

AccountEditWidget * JabberProtocolFactory::newEditAccountWidget(Account *account, QWidget *parent)
{
	return new JabberEditAccountWidget(account, parent);
}

QList<StatusType *> JabberProtocolFactory::supportedStatusTypes()
{
	return SupportedStatusTypes;
}

QRegExp JabberProtocolFactory::idRegularExpression()
{
	return IdRegularExpression;
}

ContactAccountDataWidget * JabberProtocolFactory::newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent)
{
	JabberContactAccountData *jabberContactAccountData = dynamic_cast<JabberContactAccountData *>(contactAccountData);

	return 0 != jabberContactAccountData
		? new JabberContactAccountDataWidget(jabberContactAccountData, parent)
		: 0;
}
