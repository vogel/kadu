/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "core/injected-factory.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "status/status-type.h"

#include "actions/jabber-protocol-menu-manager.h"
#include "gui/widgets/jabber-add-account-widget.h"
#include "gui/widgets/jabber-contact-personal-info-widget.h"
#include "gui/widgets/jabber-create-account-widget.h"
#include "gui/widgets/jabber-edit-account-widget.h"
#include "services/jabber-servers-service.h"
#include "facebook-depreceated-message.h"
#include "jabber-account-details.h"
#include "jabber-id-validator.h"
#include "jabber-protocol-factory.h"
#include "jabber-protocol.h"
#include "jabber-status-adapter.h"

JabberProtocolFactory::JabberProtocolFactory(QObject *parent) :
		ProtocolFactory{}
{
	Q_UNUSED(parent);

	m_statusAdapter = make_unique<JabberStatusAdapter>();

	// already sorted
	m_supportedStatusTypes.append(StatusTypeFreeForChat);
	m_supportedStatusTypes.append(StatusTypeOnline);
	m_supportedStatusTypes.append(StatusTypeAway);
	m_supportedStatusTypes.append(StatusTypeNotAvailable);
	m_supportedStatusTypes.append(StatusTypeDoNotDisturb);
	m_supportedStatusTypes.append(StatusTypeOffline);
}

JabberProtocolFactory::~JabberProtocolFactory()
{
}

void JabberProtocolFactory::setFacebookDepreceatedMessage(FacebookDepreceatedMessage *facebookDepreceatedMessage)
{
	m_facebookDepreceatedMessage = facebookDepreceatedMessage;
}

void JabberProtocolFactory::setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager)
{
	m_jabberProtocolMenuManager = jabberProtocolMenuManager;
}

KaduIcon JabberProtocolFactory::icon()
{
	return KaduIcon("protocols/xmpp/xmpp", "16x16");
}

Protocol * JabberProtocolFactory::createProtocolHandler(Account account)
{
	if (account.id().toLower().endsWith("@chat.facebook.com"))
		m_facebookDepreceatedMessage->showIfNotSeen();

	return Core::instance()->injectedFactory()->makeInjected<JabberProtocol>(account, this);
}

AccountDetails * JabberProtocolFactory::createAccountDetails(AccountShared *accountShared)
{
	return new JabberAccountDetails(accountShared);
}

AccountAddWidget * JabberProtocolFactory::newAddAccountWidget(bool showButtons, QWidget *parent)
{
	auto result = new JabberAddAccountWidget(this, showButtons, parent);
	result->setJabberServersService(new JabberServersService{result});
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountCreateWidget * JabberProtocolFactory::newCreateAccountWidget(bool showButtons, QWidget *parent)
{
	auto result = new JabberCreateAccountWidget(showButtons, parent);
	result->setJabberServersService(new JabberServersService{result});
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountEditWidget * JabberProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
	JabberEditAccountWidget *result = Core::instance()->injectedFactory()->makeInjected<JabberEditAccountWidget>(Core::instance()->accountConfigurationWidgetFactoryRepository(), account, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

QList<StatusType> JabberProtocolFactory::supportedStatusTypes()
{
	return m_supportedStatusTypes;
}

QString JabberProtocolFactory::idLabel()
{
	return tr("User JID:");
}

QValidator::State JabberProtocolFactory::validateId(QString id)
{
	int pos = 0;
	JabberIdValidator validator;
	return validator.validate(id, pos);
}

bool JabberProtocolFactory::canRegister()
{
	return true;
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
	return m_jabberProtocolMenuManager;
}

#include "moc_jabber-protocol-factory.cpp"
