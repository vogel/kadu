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

#include "services/facebook-roster-service.h"
#include "services/facebook-roster-service.moc"

#include "facebook-account-data.h"
#include "facebook-protocol.h"
#include "qfacebook/http/qfacebook-download-contacts-delta-result.h"
#include "qfacebook/http/qfacebook-download-contacts-result.h"
#include "qfacebook/qfacebook-contact.h"
#include "qfacebook/session/qfacebook-session.h"

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "roster/roster-entry.h"

#include <QtCore/QTimer>

FacebookRosterService::FacebookRosterService(
    QVector<Contact> contacts, Protocol &protocol, QFacebookSession &facebookSession)
        : RosterService{std::move(contacts), &protocol}, m_facebookSession{facebookSession}
{
    connect(&facebookSession, &QFacebookSession::contactsReceived, this, &FacebookRosterService::contactsReceived);
    connect(
        &facebookSession, &QFacebookSession::contactsDeltaReceived, this,
        &FacebookRosterService::contactsDeltaReceived);
    downloadRoster();
}

FacebookRosterService::~FacebookRosterService()
{
}

void FacebookRosterService::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void FacebookRosterService::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void FacebookRosterService::downloadRoster()
{
    auto accountData = FacebookAccountData{protocol()->account()};
    auto deltaCursor = accountData.deltaCursor();

    if (deltaCursor.isEmpty())
        m_facebookSession.downloadContacts();
    else
        m_facebookSession.downloadContactsDelta(deltaCursor);
}

void FacebookRosterService::downloadFullRoster()
{
    auto accountData = FacebookAccountData{protocol()->account()};
    accountData.setDeltaCursor({});
    downloadRoster();
}

void FacebookRosterService::scheduleDownloadRoster()
{
    QTimer::singleShot(60 * 1000, this, &FacebookRosterService::downloadRoster);
}

void FacebookRosterService::contactsReceived(const QFacebookDownloadContactsResult &downloadContactsResult)
{
    replaceContacts(downloadContactsResult.contacts);

    auto accountData = FacebookAccountData{protocol()->account()};
    accountData.setDeltaCursor(downloadContactsResult.deltaCursor);

    scheduleDownloadRoster();
}

void FacebookRosterService::contactsDeltaReceived(
    const QFacebookDownloadContactsDeltaResult &downloadContactsDeltaResult)
{
    if (downloadContactsDeltaResult.status == QFacebookDownloadContactsDeltaStatus::ErrorManyPages)
    {
        downloadFullRoster();
        return;
    }

    auto accountData = FacebookAccountData{protocol()->account()};
    accountData.setDeltaCursor(downloadContactsDeltaResult.deltaCursor);

    for (auto const &c : downloadContactsDeltaResult.added)
        addContact(c);

    for (auto const &r : downloadContactsDeltaResult.removed)
        removeContact(r);

    scheduleDownloadRoster();
}

void FacebookRosterService::replaceContacts(std::vector<QFacebookContact> newContacts)
{
    auto currentContacts = m_contactManager->contacts(protocol()->account());
    for (auto const &c : newContacts)
        addContact(c);

    std::vector<QString> currentIds;
    std::transform(
        std::begin(currentContacts), std::end(currentContacts), std::back_inserter(currentIds),
        [](const Contact &c) { return c.id(); });
    std::sort(std::begin(currentIds), std::end(currentIds));

    std::vector<QString> newIds;
    std::transform(
        std::begin(newContacts), std::end(newContacts), std::back_inserter(newIds),
        [](const QFacebookContact &c) { return QString::fromUtf8(c.id()); });
    std::sort(std::begin(newIds), std::end(newIds));

    std::vector<QString> toRemove;
    std::set_difference(
        std::begin(currentIds), std::end(currentIds), std::begin(newIds), std::end(newIds),
        std::back_inserter(toRemove));
    for (auto const &id : toRemove)
        removeContact(id);
}

void FacebookRosterService::addContact(const QFacebookContact &c)
{
    auto contact = m_contactManager->byId(protocol()->account(), c.id(), ActionCreateAndAdd);
    if (!contact || contact == account().accountContact())
        return;

    contact.rosterEntry()->setSynchronizingFromRemote();
    if (contact.isAnonymous())   // contact has anonymous buddy, we should search for other
    {
        contact.setOwnerBuddy(m_buddyManager->byDisplay(c.name(), ActionCreateAndAdd));
        contact.ownerBuddy().setAnonymous(false);
    }
    else
        contact.ownerBuddy().setDisplay(c.name());

    auto buddy = m_buddyManager->byContact(contact, ActionCreateAndAdd);
    m_buddyManager->addItem(buddy);
    contact.rosterEntry()->setSynchronized();

    RosterService::addContact(contact);
    emit added(c);
}

void FacebookRosterService::removeContact(const QString &id)
{
    auto contact = m_contactManager->byId(protocol()->account(), id, ActionReturnNull);
    if (contact)
    {
        m_buddyManager->clearOwnerAndRemoveEmptyBuddy(contact);
        RosterService::removeContact(contact);
        contact.rosterEntry()->setSynchronized();
        m_contactManager->removeItem(contact);
    }
}
