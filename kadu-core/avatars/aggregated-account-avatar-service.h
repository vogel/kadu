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

class AccountAvatarService;

class KADUAPI AggregatedAccountAvatarService : public QObject
{
    Q_OBJECT

public:
    enum class Availability
    {
        None,
        UploadOnly,
        Full
    };

    Q_INVOKABLE explicit AggregatedAccountAvatarService(QObject *parent = nullptr) : QObject{parent}
    {
    }
    virtual ~AggregatedAccountAvatarService() = default;

    void add(AccountAvatarService *service);
    void remove(AccountAvatarService *service);
    Availability availability(const Account &account) const;

    void upload(const Account &account, QPixmap avatar) const;

signals:
    void availabilityChanged();
    void finished(const Account &account, bool ok) const;

private:
    std::map<Account, AccountAvatarService *> m_services;

    void subFinished(bool ok);
};
