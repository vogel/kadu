/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QPushButton>

#include "core/core.h"
#include "icons/kadu-icon.h"
#include "status/status-type.h"

#include "gui/widgets/gadu-add-account-widget.h"
#include "gui/widgets/gadu-contact-personal-info-widget.h"
#include "gui/widgets/gadu-edit-account-widget.h"
#include "gadu-account-details.h"
#include "gadu-id-validator.h"
#include "gadu-protocol.h"
#include "gadu-status-adapter.h"

#include "gadu-protocol-factory.h"

GaduProtocolFactory *GaduProtocolFactory::Instance = 0;

void GaduProtocolFactory::createInstance()
{
	if (!Instance)
		Instance = new GaduProtocolFactory();
}

void GaduProtocolFactory::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

GaduProtocolFactory::GaduProtocolFactory()
{
	MyStatusAdapter = make_unique<GaduStatusAdapter>();

	// already sorted
	SupportedStatusTypes.append(StatusTypeFreeForChat);
	SupportedStatusTypes.append(StatusTypeOnline);
	SupportedStatusTypes.append(StatusTypeAway);
	SupportedStatusTypes.append(StatusTypeDoNotDisturb);
	SupportedStatusTypes.append(StatusTypeInvisible);
	SupportedStatusTypes.append(StatusTypeOffline);
}

Protocol * GaduProtocolFactory::createProtocolHandler(Account account)
{
	return new GaduProtocol(account, this);
}

AccountDetails * GaduProtocolFactory::createAccountDetails(AccountShared *accountShared)
{
	return new GaduAccountDetails(accountShared);
}

AccountAddWidget * GaduProtocolFactory::newAddAccountWidget(bool showButtons, QWidget *parent)
{
	GaduAddAccountWidget *result = new GaduAddAccountWidget(showButtons, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountCreateWidget * GaduProtocolFactory::newCreateAccountWidget(bool, QWidget *)
{
	return nullptr;
}

AccountEditWidget * GaduProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
	GaduEditAccountWidget *result = new GaduEditAccountWidget(Core::instance()->accountConfigurationWidgetFactoryRepository(), account, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

QList<StatusType> GaduProtocolFactory::supportedStatusTypes()
{
	return SupportedStatusTypes;
}

QString GaduProtocolFactory::idLabel()
{
	return tr("Gadu-Gadu number:");
}

QValidator::State GaduProtocolFactory::validateId(QString id)
{
	int pos = 0;
	return GaduIdValidator::instance()->validate(id, pos);
}

bool GaduProtocolFactory::canRegister()
{
	return false;
}

QWidget * GaduProtocolFactory::newContactPersonalInfoWidget(Contact contact, QWidget *parent)
{
	return new GaduContactPersonalInfoWidget(contact, parent);
}

KaduIcon GaduProtocolFactory::icon()
{
	return KaduIcon("protocols/gadu-gadu/gadu-gadu");
}

#include "moc_gadu-protocol-factory.cpp"
