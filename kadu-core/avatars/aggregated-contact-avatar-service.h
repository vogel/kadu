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
struct ContactAvatarGlobalId;
struct ContactAvatarId;
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

    void add(ContactAvatarService *service);
    void remove(ContactAvatarService *service);
    bool download(const ContactAvatarGlobalId &id) const;

signals:
    void available(const ContactAvatarGlobalId &id);
    void downloaded(const ContactAvatarGlobalId &id, const QByteArray &content);
    void removed(const ContactGlobalId &id);

private:
    std::map<Account, ContactAvatarService *> m_services;

    void subAvailable(const ContactAvatarId &id);
    void subDownloaded(const ContactAvatarId &, const QByteArray &content);
    void subRemoved(const ContactId &id);
};
