/*
 * %kadu copyright begin%
 * Copyright 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contact-avatar-downloader.h"
#include "contact-avatar-downloader.moc"

#include "avatars/aggregated-contact-avatar-service.h"
#include "avatars/avatar-id.h"
#include "avatars/avatars.h"
#include "avatars/contact-avatar-global-id.h"
#include "avatars/serial-avatar-downloader.h"
#include "configuration/configuration-manager.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-global-id.h"
#include "contacts/contact-manager.h"

#include <QtGui/QPixmap>

ContactAvatarDownloader::ContactAvatarDownloader(QObject *parent) : QObject{parent}
{
}

ContactAvatarDownloader::~ContactAvatarDownloader() = default;

void ContactAvatarDownloader::setAggregatedContactAvatarService(
    AggregatedContactAvatarService *aggregatedContactAvatarService)
{
    m_aggregatedContactAvatarService = aggregatedContactAvatarService;
}

void ContactAvatarDownloader::setAvatars(Avatars *avatars)
{
    m_avatars = avatars;
}

void ContactAvatarDownloader::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ContactAvatarDownloader::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void ContactAvatarDownloader::setSerialAvatarDownloader(SerialAvatarDownloader *serialAvatarDownloader)
{
    m_serialAvatarDownloader = serialAvatarDownloader;
}

void ContactAvatarDownloader::init()
{
    connect(
        m_aggregatedContactAvatarService, &AggregatedContactAvatarService::available, this,
        &ContactAvatarDownloader::downloadIfNeeded);
    connect(
        m_aggregatedContactAvatarService, &AggregatedContactAvatarService::downloaded, this,
        &ContactAvatarDownloader::store);
    connect(
        m_aggregatedContactAvatarService, &AggregatedContactAvatarService::removed, this,
        &ContactAvatarDownloader::remove);
    connect(m_contactManager, &ContactManager::contactRemoved, this, &ContactAvatarDownloader::removeFor);

    configurationUpdated();
}

void ContactAvatarDownloader::configurationUpdated()
{
    m_downloadAvatars = m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowAvatars", true);
}

void ContactAvatarDownloader::downloadIfNeeded(const ContactAvatarGlobalId &id) const
{
    if (!m_downloadAvatars)
        return;

    if (downloadRequired(id))
        m_serialAvatarDownloader->downloadAvatar({id.contact, id.id});
}

bool ContactAvatarDownloader::downloadRequired(const ContactAvatarGlobalId &id) const
{
    auto contact = m_contactManager->byId(id.contact.account, id.contact.id.value, ActionReturnNull);
    if (!contact)
        return false;

    if (id.id.isEmpty())
        return true;

    if (!m_avatars->contains(avatarId(id.contact)))
        return true;

    auto oldId = QByteArray::fromHex(contact.property("avatar:id", QByteArray{}).toByteArray());
    return oldId != id.id;
}

void ContactAvatarDownloader::store(const ContactAvatarGlobalId &id, const QByteArray &content)
{
    auto contact = m_contactManager->byId(id.contact.account, id.contact.id.value, ActionReturnNull);
    if (!contact)
        return;

    contact.addProperty("avatar:id", id.id.toHex(), CustomProperties::Storable);

    auto pixmap = QPixmap{};
    if (pixmap.loadFromData(content))
        m_avatars->update(avatarId(id.contact), pixmap);
}

void ContactAvatarDownloader::remove(const ContactGlobalId &id)
{
    auto contact = m_contactManager->byId(id.account, id.id.value, ActionReturnNull);
    removeFor(contact);
}

void ContactAvatarDownloader::removeFor(const Contact &contact)
{
    contact.removeProperty("avatar:id");
    m_avatars->remove(avatarId(contact));
}
