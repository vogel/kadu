/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy.h"
#include "contacts/contact.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyStorage;
class ContactStorage;

class KADUAPI BuddySearchCriteria : public QObject
{
    Q_OBJECT

public:
    explicit BuddySearchCriteria(QObject *parent = nullptr);
    virtual ~BuddySearchCriteria();

    Buddy SearchBuddy;
    QString BirthYearFrom;
    QString BirthYearTo;
    bool Active;
    bool IgnoreResults;

    void reqUin(Account account, const QString &uin);
    void reqFirstName(const QString &firstName);
    void reqLastName(const QString &lastName);
    void reqNickName(const QString &nickName);
    void reqCity(const QString &city);
    void reqBirthYear(const QString &birthYearFrom, const QString &birthYearTo);
    void reqGender(bool female);
    void reqActive();

    void clearData();

private:
    QPointer<BuddyStorage> m_buddyStorage;
    QPointer<ContactStorage> m_contactStorage;

private slots:
    INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);
    INJEQT_SET void setContactStorage(ContactStorage *contactStorage);
    INJEQT_INIT void init();
};
