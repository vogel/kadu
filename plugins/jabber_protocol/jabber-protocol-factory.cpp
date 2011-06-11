/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
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

#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "actions/jabber-protocol-menu-manager.h"
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

void JabberProtocolFactory::createInstance()
{
	if (!Instance)
		Instance = new JabberProtocolFactory();
}

void JabberProtocolFactory::destroyInstance()
{
	delete Instance;
	Instance = 0;
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

KaduIcon JabberProtocolFactory::icon()
{
	return KaduIcon("protocols/xmpp/xmpp", "16x16");
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
	return new JabberContactDetails(contactShared, this);
}

AccountAddWidget * JabberProtocolFactory::newAddAccountWidget(bool showButtons, QWidget *parent)
{
	JabberAddAccountWidget *result = new JabberAddAccountWidget(this, showButtons, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountCreateWidget * JabberProtocolFactory::newCreateAccountWidget(bool showButtons, QWidget *parent)
{
	JabberCreateAccountWidget *result = new JabberCreateAccountWidget(showButtons, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountEditWidget * JabberProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
	JabberEditAccountWidget *result = new JabberEditAccountWidget(account, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
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
	return QString();
}

QWidget * JabberProtocolFactory::newContactPersonalInfoWidget(Contact contact, QWidget *parent)
{
	return new JabberContactPersonalInfoWidget(contact, parent);
}

ProtocolMenuManager * JabberProtocolFactory::protocolMenuManager()
{
	return JabberProtocolMenuManager::instance();
}
