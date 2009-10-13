/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QPushButton>

#include "gui/widgets/gadu-create-account-widget.h"
#include "gui/widgets/gadu-edit-account-widget.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "gadu-account.h"
#include "gadu-contact-account-data-widget.h"
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
	StatusTypeManager *statusTypeManager = StatusTypeManager::instance();
	SupportedStatusTypes.append(statusTypeManager->statusType("Online"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Away"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Invisible"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Offline"));

	qSort(SupportedStatusTypes.begin(), SupportedStatusTypes.end(), StatusType::lessThan);

	IdRegularExpression.setPattern("[0-9]{1,12}");
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
	account->load();

	return account;
}

ContactAccountData * GaduProtocolFactory::newContactAccountData(Account *account, Contact contact, const QString &id)
{
	return new GaduContactAccountData(account, contact, id, true);
}

ContactAccountData * GaduProtocolFactory::loadContactAccountData(Account *account, Contact contact)
{
	StoragePoint *point = contact.storagePointForAccountData(account);
	return point
			? new GaduContactAccountData(account, contact, QString::null, point)
			: 0;
}

AccountCreateWidget * GaduProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
	return new GaduCreateAccountWidget(parent);
}


AccountEditWidget * GaduProtocolFactory::newEditAccountWidget(Account *account, QWidget *parent)
{
	return new GaduEditAccountWidget(account, parent);
}

QList<StatusType *> GaduProtocolFactory::supportedStatusTypes()
{
	return SupportedStatusTypes;
}

QString GaduProtocolFactory::idLabel()
{
	return tr("Gadu Gadu number:");
}

QRegExp GaduProtocolFactory::idRegularExpression()
{
	return IdRegularExpression;
}

ContactAccountDataWidget * GaduProtocolFactory::newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent)
{
	GaduContactAccountData *gaduContactAccountData = dynamic_cast<GaduContactAccountData *>(contactAccountData);

	return 0 != gaduContactAccountData
		? new GaduContactAccountDataWidget(gaduContactAccountData, parent)
		: 0;
}
