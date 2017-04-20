/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;
class BuddyList;
class BuddyManager;
class BuddyStorage;
class Buddy;
class ContactStorage;
class Contact;
class GroupManager;

class QByteArray;
class QString;
class QTextStream;

class GaduListHelper : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GaduListHelper(QObject *parent = nullptr);
    virtual ~GaduListHelper();

    QByteArray contactListToByteArray(const QVector<Contact> &contacts);
    QString contactToLine70(Contact contact);
    BuddyList byteArrayToBuddyList(Account account, QByteArray &content);
    BuddyList streamToBuddyList(Account account, QTextStream &content);
    Buddy linePre70ToBuddy(Account account, QStringList &sections);
    Buddy line70ToBuddy(Account account, QStringList &sections);
    BuddyList streamPost70ToBuddyList(const QString &line, Account account, QTextStream &content);
    BuddyList stream70ToBuddyList(Account account, QTextStream &content);
    BuddyList streamPre70ToBuddyList(const QString &firstLine, Account account, QTextStream &content);

private:
    QPointer<BuddyManager> m_buddyManager;
    QPointer<BuddyStorage> m_buddyStorage;
    QPointer<ContactStorage> m_contactStorage;
    QPointer<GroupManager> m_groupManager;

private slots:
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);
    INJEQT_SET void setContactStorage(ContactStorage *contactStorage);
    INJEQT_SET void setGroupManager(GroupManager *groupManager);
};
