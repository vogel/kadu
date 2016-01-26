/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gtalk-protocol-factory.h"
#include "jabber-protocol.h"
#include "jabber-status-adapter.h"

GTalkProtocolFactory::GTalkProtocolFactory(QObject *parent) :
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

GTalkProtocolFactory::~GTalkProtocolFactory()
{
}

void GTalkProtocolFactory::setFacebookDepreceatedMessage(FacebookDepreceatedMessage *facebookDepreceatedMessage)
{
	m_facebookDepreceatedMessage = facebookDepreceatedMessage;
}

void GTalkProtocolFactory::setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager)
{
	m_jabberProtocolMenuManager = jabberProtocolMenuManager;
}

KaduIcon GTalkProtocolFactory::icon()
{
	return KaduIcon("protocols/xmpp/brand_name/GmailGoogleTalk", "16x16");
}

Protocol * GTalkProtocolFactory::createProtocolHandler(Account account)
{
	if (account.id().toLower().endsWith("@chat.facebook.com"))
		m_facebookDepreceatedMessage->showIfNotSeen();

	return Core::instance()->injectedFactory()->makeInjected<JabberProtocol>(account, this);
}

AccountDetails * GTalkProtocolFactory::createAccountDetails(AccountShared *accountShared)
{
	return Core::instance()->injectedFactory()->makeInjected<JabberAccountDetails>(accountShared);
}

AccountAddWidget * GTalkProtocolFactory::newAddAccountWidget(bool showButtons, QWidget *parent)
{
	Q_UNUSED(showButtons);
	Q_UNUSED(parent);

	return nullptr;
}

AccountCreateWidget * GTalkProtocolFactory::newCreateAccountWidget(bool showButtons, QWidget *parent)
{
	auto result = Core::instance()->injectedFactory()->makeInjected<JabberCreateAccountWidget>(showButtons, parent);
	result->setJabberServersService(new JabberServersService{result});
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

AccountEditWidget * GTalkProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
	JabberEditAccountWidget *result = Core::instance()->injectedFactory()->makeInjected<JabberEditAccountWidget>(account, parent);
	connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
	return result;
}

QList<StatusType> GTalkProtocolFactory::supportedStatusTypes()
{
	return m_supportedStatusTypes;
}

QString GTalkProtocolFactory::idLabel()
{
	return tr("Gmail/Google Talk ID:");
}

QValidator::State GTalkProtocolFactory::validateId(QString id)
{
	int pos = 0;
	JabberIdValidator validator;
	return validator.validate(id, pos);
}

bool GTalkProtocolFactory::canRegister()
{
	return false;
}

bool GTalkProtocolFactory::allowChangeServer()
{
	return true;
}

QString GTalkProtocolFactory::defaultServer()
{
    return QLatin1String("gmail.com");
}

QWidget * GTalkProtocolFactory::newContactPersonalInfoWidget(Contact contact, QWidget *parent)
{
	return Core::instance()->injectedFactory()->makeInjected<JabberContactPersonalInfoWidget>(contact, parent);
}

ProtocolMenuManager * GTalkProtocolFactory::protocolMenuManager()
{
	return m_jabberProtocolMenuManager;
}

#include "moc_gtalk-protocol-factory.cpp"
