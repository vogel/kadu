/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-open-chat-with-runner.h"

#include "jabber-id-validator.h"

#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "contacts/contact-manager.h"

JabberOpenChatWithRunner::JabberOpenChatWithRunner(Account account, QObject *parent)
        : QObject{parent}, m_account{account}
{
}

JabberOpenChatWithRunner::~JabberOpenChatWithRunner()
{
}

void JabberOpenChatWithRunner::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void JabberOpenChatWithRunner::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

BuddyList JabberOpenChatWithRunner::matchingContacts(const QString &query)
{
    BuddyList matchedContacts;
    QString queryCopy(query);
    int pos = 0;
    JabberIdValidator validator;
    if (validator.validate(queryCopy, pos) != QValidator::Acceptable)
        return matchedContacts;

    auto contact = m_contactManager->byId(m_account, query, ActionCreate);
    auto buddy = m_buddyManager->byContact(contact, ActionCreate);
    matchedContacts.append(buddy);

    return matchedContacts;
}

void JabberOpenChatWithRunner::setAccount(Account account)
{
    m_account = account;
}

#include "moc_jabber-open-chat-with-runner.cpp"
