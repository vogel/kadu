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

#include "gadu-account-details.h"
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

Protocol * GaduProtocolFactory::createProtocolHandler(Account account)
{
	return new GaduProtocol(account, this);
}

AccountDetails * GaduProtocolFactory::createAccountDetails(Account account)
{
	return new GaduAccountDetails(account.storage(), account);
}

ContactAccountData * GaduProtocolFactory::newContactAccountData(Account account, Buddy buddy, const QString &id)
{
	return new GaduContactAccountData(account, buddy, id, true);
}

ContactAccountData * GaduProtocolFactory::loadContactAccountData(StoragePoint *storagePoint)
{
	if (!storagePoint)
		return 0;

	return new GaduContactAccountData(storagePoint);
}

AccountCreateWidget * GaduProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
	return new GaduCreateAccountWidget(parent);
}

AccountEditWidget * GaduProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
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
