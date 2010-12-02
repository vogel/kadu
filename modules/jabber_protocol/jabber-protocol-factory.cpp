/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/misc.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "icons-manager.h"

#include "gui/widgets/jabber-contact-personal-info-widget.h"
#include "gui/widgets/jabber-add-account-widget.h"
#include "gui/widgets/jabber-create-account-widget.h"
#include "gui/widgets/jabber-edit-account-widget.h"
#include "jabber-account-details.h"
#include "jabber-contact-details.h"
#include "jabber-id-validator.h"
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
	SupportedStatusTypes.append(statusTypeManager->statusType("Offline"));

	qSort(SupportedStatusTypes.begin(), SupportedStatusTypes.end(), StatusType::lessThan);
}

QIcon JabberProtocolFactory::icon()
{
	return IconsManager::instance()->iconByPath("protocols/xmpp/xmpp");
}

QString JabberProtocolFactory::iconPath()
{
	return IconsManager::instance()->iconPath("protocols/xmpp/xmpp", "16x16");
}

Protocol * JabberProtocolFactory::createProtocolHandler(Account account)
{
	return new JabberProtocol(account, this);
}

AccountDetails * JabberProtocolFactory::createAccountDetails(AccountShared *accountShared)
{
	return new JabberAccountDetails(accountShared);
}

ContactDetails * JabberProtocolFactory::createContactDetails(ContactShared *contactShared)
{
	return new JabberContactDetails(contactShared);
}

AccountAddWidget * JabberProtocolFactory::newAddAccountWidget(QWidget *parent)
{
	return new JabberAddAccountWidget(this, parent);
}

AccountCreateWidget * JabberProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
	return new JabberCreateAccountWidget(parent);
}

AccountEditWidget * JabberProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
	return new JabberEditAccountWidget(account, parent);
}

QList<StatusType *> JabberProtocolFactory::supportedStatusTypes()
{
	return SupportedStatusTypes;
}

QString JabberProtocolFactory::idLabel()
{
	return tr("User JID:");
}

QValidator::State JabberProtocolFactory::validateId(QString id)
{
	int pos = 0;
	return JabberIdValidator::instance()->validate(id, pos);
}

bool JabberProtocolFactory::allowChangeServer()
{
	return true;
}

QString JabberProtocolFactory::defaultServer()
{
	return QString::null;
}

QWidget * JabberProtocolFactory::newContactPersonalInfoWidget(Contact contact, QWidget *parent)
{
	return new JabberContactPersonalInfoWidget(contact, parent);
}
