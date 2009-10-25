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

QString JabberProtocolFactory::iconName()
{
	return dataPath("kadu/modules/data/jabber_protocol/").append("online.png");
}

Protocol * JabberProtocolFactory::createProtocolHandler(Account *account)
{
	return new JabberProtocol(account, this);
}

AccountDetails * JabberProtocolFactory::createAccountDetails(Account *account)
{
	return 0;
}

ContactAccountData * JabberProtocolFactory::newContactAccountData(Account *account, Contact contact, const QString &id)
{
	return new JabberContactAccountData(account, contact, id, true);
}

ContactAccountData * JabberProtocolFactory::loadContactAccountData(StoragePoint *storagePoint)
{
	if (!storagePoint)
		return 0;

	return new JabberContactAccountData(storagePoint);
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

QString JabberProtocolFactory::idLabel()
{
	return tr("Jabber ID:");
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
