/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"

#include "otr-app-ops-wrapper.h"
#include "otr-op-data.h"
#include "otr-private-key-service.h"

#include "otr-op-data-factory.h"

OtrOpDataFactory::OtrOpDataFactory(QObject *parent) :
		QObject(parent)
{
}

OtrOpDataFactory::~OtrOpDataFactory()
{
}

void OtrOpDataFactory::setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper)
{
	AppOpsWrapper = appOpsWrapper;
}

void OtrOpDataFactory::setPrivateKeyService(OtrPrivateKeyService *privateKeyService)
{
	PrivateKeyService = privateKeyService;
}

OtrOpData OtrOpDataFactory::opDataForContact(const Contact &contact)
{
	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);

	OtrOpData result;
	result.setAppOpsWrapper(AppOpsWrapper.data());
	result.setChat(chat);
	result.setPeerDisplay(contact.display(true));
	result.setPrivateKeyService(PrivateKeyService.data());

	return result;
}
