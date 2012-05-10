/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "icons/kadu-icon.h"
#include "status/status-type.h"

#include "gadu-account-details.h"
#include "gadu-contact-details.h"
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
	MyStatusAdapter = new GaduStatusAdapter();

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

ContactDetails * GaduProtocolFactory::createContactDetails(ContactShared *contactShared)
{
	return new GaduContactDetails(contactShared);
}

AccountAddWidget * GaduProtocolFactory::newAddAccountWidget(bool showButtons, QWidget *parent)
{
	GaduAddAccountWidget *result = new GaduAddAccountWidget(showButtons, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountCreateWidget * GaduProtocolFactory::newCreateAccountWidget(bool showButtons, QWidget *parent)
{
	GaduCreateAccountWidget *result = new GaduCreateAccountWidget(showButtons, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountEditWidget * GaduProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
	GaduEditAccountWidget *result = new GaduEditAccountWidget(account, parent);
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

QWidget * GaduProtocolFactory::newContactPersonalInfoWidget(Contact contact, QWidget *parent)
{
	return new GaduContactPersonalInfoWidget(contact, parent);
}

KaduIcon GaduProtocolFactory::icon()
{
	return KaduIcon("protocols/gadu-gadu/gadu-gadu");
}
