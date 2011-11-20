/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "account-status-container.h"

AccountStatusContainer::AccountStatusContainer(AccountShared *account) :
		StorableStatusContainer(account), Account(account)
{
}

AccountStatusContainer::~AccountStatusContainer()
{
}

QString AccountStatusContainer::statusContainerName()
{
	return Account->Id;
}

void AccountStatusContainer::setStatus(Status newStatus)
{
	if (Account->ProtocolHandler)
		Account->ProtocolHandler->setStatus(newStatus);
}

Status AccountStatusContainer::status()
{
	if (Account->ProtocolHandler)
		return Account->ProtocolHandler->status();
	else
		return Status();
}

bool AccountStatusContainer::isStatusSettingInProgress()
{
	if (Account->ProtocolHandler)
		return Account->ProtocolHandler->isConnecting();
	else
		return false;
}

int AccountStatusContainer::maxDescriptionLength()
{
	if (Account->ProtocolHandler)
		return Account->ProtocolHandler->maxDescriptionLength();
	else
		return 0;
}

KaduIcon AccountStatusContainer::statusIcon()
{
	return statusIcon(status());
}

KaduIcon AccountStatusContainer::statusIcon(const Status &status)
{
	if (Account->ProtocolHandler)
		return Account->ProtocolHandler->statusIcon(status);
	else
		return KaduIcon();
}

QList<StatusType> AccountStatusContainer::supportedStatusTypes()
{
	if (Account->ProtocolHandler)
		return Account->ProtocolHandler->protocolFactory()->supportedStatusTypes();
	else
		return QList<StatusType>();
}

void AccountStatusContainer::triggerStatusUpdated()
{
	emit statusUpdated();
}
