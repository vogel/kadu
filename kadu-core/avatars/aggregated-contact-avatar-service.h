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

#pragma once

#include "accounts/account.h"
#include "exports.h"

#include <QtCore/QObject>
#include <map>

class ContactAvatarService;
struct ContactGlobalId;
struct ContactId;

class KADUAPI AggregatedContactAvatarService : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AggregatedContactAvatarService(QObject *parent = nullptr) : QObject{parent}
    {
    }
    virtual ~AggregatedContactAvatarService() = default;

public slots:
    void addContactAvatarService(ContactAvatarService *contactAvatarService);
    void removeContactAvatarService(ContactAvatarService *contactAvatarService);

signals:
    void avatarAvailable(const ContactGlobalId &contactId, const QByteArray &id);
    void avatarDownloaded(const ContactGlobalId &contactId, const QByteArray &id, const QByteArray &content);
    void avatarRemoved(const ContactGlobalId &contactId);

private:
    std::map<Account, ContactAvatarService *> m_contactAvatarServices;

    void avatarAvailableTranslator(const ContactId &contactId, const QByteArray &id);
    void avatarDownloadedTranslator(const ContactId &contactId, const QByteArray &id, const QByteArray &content);
    void avatarRemovedTranslator(const ContactId &contactId);
};
