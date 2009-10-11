/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/widgets/tlen-create-account-widget.h"
#include "gui/widgets/tlen-edit-account-widget.h"

#include "tlen-account.h"
#include "tlen-protocol.h"
#include "tlen-contact-account-data.h"
#include "tlen-contact-account-data-widget.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "tlen-protocol-factory.h"

TlenProtocolFactory *TlenProtocolFactory::Instance = 0;

TlenProtocolFactory * TlenProtocolFactory::instance()
{
	if (!Instance)
		Instance = new TlenProtocolFactory();

	return Instance;
}

TlenProtocolFactory::TlenProtocolFactory()
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
//TODO 0.6.6:
	IdRegularExpression.setPattern("[a-zA-Z0-9\\._-]+@[a-zA-Z0-9\\._-]+");
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

AccountCreateWidget * TlenProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
    	return new TlenCreateAccountWidget(parent);
}

AccountEditWidget * TlenProtocolFactory::newEditAccountWidget(Account *account, QWidget *parent)
{
	return new TlenEditAccountWidget(account, parent);
}

QList<StatusType *> TlenProtocolFactory::supportedStatusTypes()
{
	return SupportedStatusTypes;
}

QString TlenProtocolFactory::idLabel()
{
	return tr("Tlen ID:");
}

QRegExp TlenProtocolFactory::idRegularExpression()
{
	return IdRegularExpression;
}

ContactAccountDataWidget * TlenProtocolFactory::newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent)
{
	TlenContactAccountData *tlenContactAccountData = dynamic_cast<TlenContactAccountData *>(contactAccountData);

	return 0 != tlenContactAccountData
		? new TlenContactAccountDataWidget(tlenContactAccountData, parent)
		: 0;
}
