/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-protocol-factory.h"

#include "facebook-protocol.h"
#include "widgets/facebook-add-account-widget.h"
#include "widgets/facebook-edit-account-widget.h"

#include "accounts/account.h"
#include "icons/kadu-icon.h"
#include "plugin/plugin-injected-factory.h"

FacebookProtocolFactory::FacebookProtocolFactory(QObject *parent) : ProtocolFactory{}
{
    Q_UNUSED(parent);

    // already sorted
    m_supportedStatusTypes.append(StatusType::Online);
    m_supportedStatusTypes.append(StatusType::Offline);
}

FacebookProtocolFactory::~FacebookProtocolFactory()
{
}

void FacebookProtocolFactory::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

KaduIcon FacebookProtocolFactory::icon()
{
    return KaduIcon("protocols/facebook/facebook", "16x16");
}

Protocol *FacebookProtocolFactory::createProtocolHandler(Account account)
{
    return m_pluginInjectedFactory->makeInjected<FacebookProtocol>(account, this);
}

AccountAddWidget *FacebookProtocolFactory::newAddAccountWidget(bool showButtons, QWidget *parent)
{
    auto result = m_pluginInjectedFactory->makeInjected<FacebookAddAccountWidget>(showButtons, parent);
    connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
    return result;
}

AccountEditWidget *FacebookProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
    auto result = m_pluginInjectedFactory->makeInjected<FacebookEditAccountWidget>(account, parent);
    connect(this, SIGNAL(destroyed()), result, SLOT(deleteLater()));
    return result;
}

QList<StatusType> FacebookProtocolFactory::supportedStatusTypes()
{
    return m_supportedStatusTypes;
}

Status FacebookProtocolFactory::adaptStatus(Status status) const
{
    return status;
}

QString FacebookProtocolFactory::idLabel()
{
    return tr("User ID:");
}

bool FacebookProtocolFactory::canRegister()
{
    return false;
}

#include "facebook-protocol-factory.moc"
