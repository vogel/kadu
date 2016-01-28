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
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"

#include "otr-user-state-service.h"

#include "otr-context-converter.h"

OtrContextConverter::OtrContextConverter(QObject *parent) :
		QObject{parent}
{
}

OtrContextConverter::~OtrContextConverter()
{
}

void OtrContextConverter::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void OtrContextConverter::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void OtrContextConverter::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void OtrContextConverter::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void OtrContextConverter::setUserStateService(OtrUserStateService *userStateService)
{
	m_userStateService = userStateService;
}

Chat OtrContextConverter::connectionContextToChat(ConnContext *context) const
{
	auto contact = connectionContextToContact(context);
	return ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
}

Contact OtrContextConverter::connectionContextToContact(ConnContext *context) const
{
	auto account = m_accountManager->byId(QString::fromUtf8(context->protocol), QString::fromUtf8(context->accountname));
	return m_contactManager->byId(account, QString::fromUtf8(context->username), ActionReturnNull);
}

ConnContext * OtrContextConverter::chatToContextConverter(const Chat &chat, NotFoundAction notFoundAction) const
{
	return chat
			? contactToContextConverter(chat.contacts().toContact(), notFoundAction)
			: nullptr;
}

ConnContext * OtrContextConverter::contactToContextConverter(const Contact &contact, NotFoundAction notFoundAction) const
{
	if (!contact)
		return 0;

	return otrl_context_find(m_userStateService->userState(), qPrintable(contact.id()), qPrintable(contact.contactAccount().id()),
							 qPrintable(contact.contactAccount().protocolName()), OTRL_INSTAG_BEST, notFoundAction == ActionCreateAndAdd,
							 0, 0, 0);
}

#include "moc_otr-context-converter.cpp"
