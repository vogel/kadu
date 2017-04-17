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

#include "gtalk-protocol-factory.h"
#include "gtalk-protocol-factory.moc"

#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "plugin/plugin-injected-factory.h"
#include "status/status-type.h"

#include "actions/jabber-protocol-menu-manager.h"
#include "facebook-depreceated-message.h"
#include "gui/widgets/jabber-add-account-widget.h"
#include "gui/widgets/jabber-contact-personal-info-widget.h"
#include "gui/widgets/jabber-create-account-widget.h"
#include "gui/widgets/jabber-edit-account-widget.h"
#include "jabber-id-validator.h"
#include "jabber-protocol.h"
#include "services/jabber-servers-service.h"

GTalkProtocolFactory::GTalkProtocolFactory(QObject *parent) : ProtocolFactory{}
{
    Q_UNUSED(parent);

    // already sorted
    m_supportedStatusTypes.append(StatusType::FreeForChat);
    m_supportedStatusTypes.append(StatusType::Online);
    m_supportedStatusTypes.append(StatusType::Away);
    m_supportedStatusTypes.append(StatusType::NotAvailable);
    m_supportedStatusTypes.append(StatusType::DoNotDisturb);
    m_supportedStatusTypes.append(StatusType::Offline);
}

GTalkProtocolFactory::~GTalkProtocolFactory()
{
}

void GTalkProtocolFactory::setFacebookDepreceatedMessage(FacebookDepreceatedMessage *facebookDepreceatedMessage)
{
    m_facebookDepreceatedMessage = facebookDepreceatedMessage;
}

void GTalkProtocolFactory::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void GTalkProtocolFactory::setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager)
{
    m_jabberProtocolMenuManager = jabberProtocolMenuManager;
}

KaduIcon GTalkProtocolFactory::icon()
{
    return KaduIcon("protocols/xmpp/brand_name/GmailGoogleTalk", "16x16");
}

Protocol *GTalkProtocolFactory::createProtocolHandler(Account account)
{
    if (account.id().toLower().endsWith("@chat.facebook.com"))
        m_facebookDepreceatedMessage->showIfNotSeen();

    return m_pluginInjectedFactory->makeInjected<JabberProtocol>(account, this);
}

AccountAddWidget *GTalkProtocolFactory::newAddAccountWidget(bool showButtons, QWidget *parent)
{
    auto result =
        m_pluginInjectedFactory->makeInjected<JabberAddAccountWidget>(false, "gmail.com", showButtons, parent);
    result->setJabberServersService(new JabberServersService{result});
    connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
    return result;
}

AccountCreateWidget *GTalkProtocolFactory::newCreateAccountWidget(bool showButtons, QWidget *parent)
{
    Q_UNUSED(showButtons);
    Q_UNUSED(parent);

    return nullptr;
}

AccountEditWidget *GTalkProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
    JabberEditAccountWidget *result = m_pluginInjectedFactory->makeInjected<JabberEditAccountWidget>(account, parent);
    connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
    return result;
}

QList<StatusType> GTalkProtocolFactory::supportedStatusTypes()
{
    return m_supportedStatusTypes;
}

Status GTalkProtocolFactory::adaptStatus(Status status) const
{
    Status adapted = status;

    if (adapted.type() == StatusType::Invisible)
        adapted.setType(StatusType::DoNotDisturb);

    return adapted;
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

QWidget *GTalkProtocolFactory::newContactPersonalInfoWidget(Contact contact, QWidget *parent)
{
    return m_pluginInjectedFactory->makeInjected<JabberContactPersonalInfoWidget>(contact, parent);
}

ProtocolMenuManager *GTalkProtocolFactory::protocolMenuManager()
{
    return m_jabberProtocolMenuManager;
}
