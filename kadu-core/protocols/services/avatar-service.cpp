/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "protocols/protocol.h"

#include "avatar-service.h"

AvatarService * AvatarService::fromAccount(Account account)
{
	if (!account)
		return 0;

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return 0;

	return protocol->avatarService();
}

AvatarService::AvatarService(Account account, QObject *parent) :
		AccountService(account, parent)
{
}

AvatarService::~AvatarService()
{
}

#include "moc_avatar-service.cpp"
