/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QPushButton>

#include "gui/widgets/gadu-add-account-widget.h"
#include "gui/widgets/gadu-contact-personal-info-widget.h"
#include "gui/widgets/gadu-create-account-widget.h"
#include "gui/widgets/gadu-edit-account-widget.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "icons-manager.h"

#include "gadu-account-details.h"
#include "gadu-contact-details.h"
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
	SupportedStatusTypes.append(statusTypeManager->statusType("FreeForChat"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Online"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Away"));
	SupportedStatusTypes.append(statusTypeManager->statusType("DoNotDisturb"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Invisible"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Offline"));

	qSort(SupportedStatusTypes.begin(), SupportedStatusTypes.end(), StatusType::lessThan);

	IdRegularExpression.setPattern("[0-9]{1,12}");
}

Protocol * GaduProtocolFactory::createProtocolHandler(Account account)
{
	return new GaduProtocol(account, this);
}

AccountDetails * GaduProtocolFactory::createAccountDetails(AccountShared *accountShared)
{
	return new GaduAccountDetails(accountShared);
}

ContactDetails * GaduProtocolFactory::createContactDetails(ContactShared *contactShared)
{
	return new GaduContactDetails(contactShared);
}

AccountAddWidget * GaduProtocolFactory::newAddAccountWidget(QWidget *parent)
{
	return new GaduAddAccountWidget(parent);
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

QWidget * GaduProtocolFactory::newContactPersonalInfoWidget(Contact contact, QWidget *parent)
{
	return new GaduContactPersonalInfoWidget(contact, parent);
}

QIcon GaduProtocolFactory::icon()
{
	return IconsManager::instance()->iconByPath("protocols/gadu-gadu/gadu-gadu");
}
