/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/widgets/tlen-contact-widget.h"
#include "gui/widgets/tlen-create-account-widget.h"
#include "gui/widgets/tlen-edit-account-widget.h"

#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "tlen-account-details.h"
#include "tlen-contact-details.h"
#include "tlen-protocol.h"

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

Protocol * TlenProtocolFactory::createProtocolHandler(Account account)
{
	return new TlenProtocol(account, this);
}

AccountDetails * TlenProtocolFactory::createAccountDetails(Account account)
{
	return new TlenAccountDetails(account.storage(), account);
}

ContactDetails * TlenProtocolFactory::createContactDetails(Contact contact)
{
	return new TlenContactDetails(contact.storage(), contact);
}

AccountCreateWidget * TlenProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
    	return new TlenCreateAccountWidget(parent);
}

AccountEditWidget * TlenProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
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

ContactWidget * TlenProtocolFactory::newContactWidget(Contact contact, QWidget *parent)
{
	TlenContactDetails *tlenContactDetails = dynamic_cast<TlenContactDetails *>(contact.details());

	return 0 != tlenContactDetails
		? new TlenContactWidget(contact, parent)
		: 0;
}
