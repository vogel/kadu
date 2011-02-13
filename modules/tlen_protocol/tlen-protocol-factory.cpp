/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "misc/misc.h"

#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "gui/widgets/tlen-add-account-widget.h"
#include "gui/widgets/tlen-contact-widget.h"
#include "gui/widgets/tlen-create-account-widget.h"
#include "gui/widgets/tlen-edit-account-widget.h"

#include "tlen-account-details.h"
#include "tlen-contact-details.h"
#include "tlen-protocol.h"

#include "tlen-protocol-factory.h"

TlenProtocolFactory * TlenProtocolFactory::Instance = 0;

void TlenProtocolFactory::createInstance()
{
	if (!Instance)
		Instance = new TlenProtocolFactory();
}

void TlenProtocolFactory::destroyInstance()
{
	delete Instance;
	Instance = 0;
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

AccountDetails * TlenProtocolFactory::createAccountDetails(AccountShared *accountShared)
{
	return new TlenAccountDetails(accountShared);
}

ContactDetails * TlenProtocolFactory::createContactDetails(ContactShared *contactShared)
{
	return new TlenContactDetails(contactShared);
}

AccountAddWidget * TlenProtocolFactory::newAddAccountWidget(QWidget *parent)
{
	return new TlenAddAccountWidget(parent);
}

QWidget * TlenProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
	Q_UNUSED(parent); return 0;
    	//return new TlenCreateAccountWidget(parent);
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

QIcon TlenProtocolFactory::icon()
{
	return IconsManager::instance()->iconByPath("protocols/tlen/32x32/online");
}
