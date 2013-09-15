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

#include "accounts/account-manager.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"

#include "otr-context-converter.h"

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
