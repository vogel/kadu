/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

extern "C" {
#	include <libotr/instag.h>
}

#include "accounts/account-manager.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"

#include "otr-user-state-service.h"

#include "otr-context-converter.h"

OtrContextConverter::OtrContextConverter()
{
}

OtrContextConverter::~OtrContextConverter()
{
}

void OtrContextConverter::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

Chat OtrContextConverter::connectionContextToChat(ConnContext *context) const
{
	Contact contact = connectionContextToContact(context);
	return ChatTypeContact::findChat(contact, ActionCreateAndAdd);
}

Contact OtrContextConverter::connectionContextToContact(ConnContext *context) const
{
	Account account = AccountManager::instance()->byId(QString::fromUtf8(context->protocol), QString::fromUtf8(context->accountname));
	return ContactManager::instance()->byId(account, QString::fromUtf8(context->username), ActionReturnNull);
}

ConnContext * OtrContextConverter::chatToContextConverter(const Chat &chat, NotFoundAction notFoundAction) const
{
	if (!UserStateService || !chat)
		return 0;

	return contactToContextConverter(chat.contacts().toContact(), notFoundAction);
}

ConnContext * OtrContextConverter::contactToContextConverter(const Contact &contact, NotFoundAction notFoundAction) const
{
	if (!UserStateService || !contact)
		return 0;

	return otrl_context_find(UserStateService.data()->userState(), qPrintable(contact.id()), qPrintable(contact.contactAccount().id()),
							 qPrintable(contact.contactAccount().protocolName()), OTRL_INSTAG_BEST, notFoundAction == ActionCreateAndAdd,
							 0, 0, 0);
}
