/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-list-helper.h"
#include "gadu-list-helper.moc"

#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-storage.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-storage.h"
#include "contacts/contact.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

GaduListHelper::GaduListHelper(QObject *parent) : QObject{parent}
{
}

GaduListHelper::~GaduListHelper()
{
}

void GaduListHelper::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void GaduListHelper::setBuddyStorage(BuddyStorage *buddyStorage)
{
    m_buddyStorage = buddyStorage;
}

void GaduListHelper::setContactStorage(ContactStorage *contactStorage)
{
    m_contactStorage = contactStorage;
}

void GaduListHelper::setGroupManager(GroupManager *groupManager)
{
    m_groupManager = groupManager;
}

QString GaduListHelper::contactToLine70(Contact contact)
{
    QStringList list;
    Buddy buddy = m_buddyManager->byContact(contact, ActionCreateAndAdd);

    list.append(buddy.firstName());
    list.append(buddy.lastName());
    list.append(buddy.nickName());
    list.append(buddy.display());
    list.append(buddy.mobile());

    QStringList groups;
    for (auto const &group : buddy.groups())
        groups.append(group.name());

    list.append(groups.join(","));
    list.append(contact.id());
    list.append(buddy.email());
    list.append(QString());   // alive sound
    list.append(QString());   // alive sound
    list.append(QString());   // message sound
    list.append(QString());   // message sound
    list.append(QString::number((int)buddy.isOfflineTo()));
    list.append(buddy.homePhone());

    return list.join(";");
}

QByteArray GaduListHelper::buddyListToByteArray(Account account, const BuddyList &buddies)
{
    auto contacts = QVector<Contact>{};
    for (auto &&buddy : buddies)
        contacts += buddy.contacts(account);
    return contactListToByteArray(contacts);
}

QByteArray GaduListHelper::contactListToByteArray(const QVector<Contact> &contacts)
{
    auto result = QStringList{};
    result.append("GG70ExportString");

    for (auto &&contact : contacts)
        result.append(contactToLine70(contact));

    return result.join("\n").toUtf8();
}

BuddyList GaduListHelper::byteArrayToBuddyList(Account account, QByteArray &content)
{
    QTextStream stream(&content, QIODevice::ReadOnly);

    return streamToBuddyList(account, stream);
}

BuddyList GaduListHelper::streamToBuddyList(Account account, QTextStream &content)
{
    BuddyList result;

    content.setCodec("UTF-8");

    QString line = content.readLine(70);

    if (line.startsWith(QStringLiteral("<ContactBook>")))
        result = streamPost70ToBuddyList(line, account, content);
    else if (line.startsWith(QStringLiteral("GG70ExportString")))
        result = stream70ToBuddyList(account, content);
    else
        result = streamPre70ToBuddyList(line, account, content);

    return result;
}

BuddyList GaduListHelper::streamPre70ToBuddyList(const QString &firstLine, Account account, QTextStream &content)
{
    BuddyList result;

    content.setCodec(QTextCodec::codecForName("CP1250"));

    if (firstLine.isEmpty())
        return result;

    QString line = firstLine;
    QStringList sections = line.split(';', QString::KeepEmptyParts);

    if (sections.count() > 6)
    {
        bool ok = false;
        sections[6].toULong(&ok);
        if (ok)
        {
            Buddy buddy = line70ToBuddy(account, sections);
            if (buddy)
                result.append(buddy);
            result.append(stream70ToBuddyList(account, content));
            return result;
        }
        else
        {
            Buddy buddy = linePre70ToBuddy(account, sections);
            if (buddy)
                result.append(buddy);
        }
    }

    while (!content.atEnd())
    {
        line = content.readLine();
        sections = line.split(';', QString::KeepEmptyParts);

        if (sections.count() < 7)
            continue;

        Buddy buddy = linePre70ToBuddy(account, sections);
        if (buddy)
            result.append(buddy);
    }

    return result;
}

BuddyList GaduListHelper::stream70ToBuddyList(Account account, QTextStream &content)
{
    BuddyList result;
    QString line;
    QStringList sections;

    while (!content.atEnd())
    {
        line = content.readLine();

        sections = line.split(';', QString::KeepEmptyParts);

        Buddy buddy = line70ToBuddy(account, sections);
        if (buddy)
            result.append(buddy);
    }

    return result;
}

BuddyList GaduListHelper::streamPost70ToBuddyList(const QString &line, Account account, QTextStream &content)
{
    BuddyList result;

    QString documentString = line + content.readAll();

    QDomDocument document;
    document.setContent(documentString);

    QDomElement docElement = document.documentElement();
    QMap<QString, Group> importedGroups;

    QDomNode groupsNode = docElement.firstChildElement("Groups");
    if (!groupsNode.isNull())
    {
        QDomElement groupElement = groupsNode.firstChildElement("Group");
        for (; !groupElement.isNull(); groupElement = groupElement.nextSiblingElement("Group"))
        {
            QDomElement idElement = groupElement.firstChildElement("Id");
            if (idElement.text().startsWith(QStringLiteral("00000000-0000-0000-0000-")))
                continue;

            QDomElement nameElement = groupElement.firstChildElement("Name");
            if (nameElement.text().isEmpty())
                continue;

            importedGroups.insert(idElement.text(), m_groupManager->byName(nameElement.text()));
        }
    }

    QDomNode contactsNode = docElement.firstChildElement("Contacts");
    if (!contactsNode.isNull())
    {
        QDomElement contactElement = contactsNode.firstChildElement("Contact");
        for (; !contactElement.isNull(); contactElement = contactElement.nextSiblingElement("Contact"))
        {
            Buddy buddy = m_buddyStorage->create();

            buddy.setFirstName(contactElement.firstChildElement("FirstName").text());
            buddy.setLastName(contactElement.firstChildElement("LastName").text());
            buddy.setDisplay(contactElement.firstChildElement("ShowName").text());
            buddy.setMobile(contactElement.firstChildElement("MobilePhone").text());
            buddy.setHomePhone(contactElement.firstChildElement("HomePhone").text());
            buddy.setEmail(contactElement.firstChildElement("Email").text());
            buddy.setCity(contactElement.firstChildElement("City").text());
            buddy.setWebsite(contactElement.firstChildElement("WwwAddress").text());
            buddy.setGender((BuddyGender)contactElement.firstChildElement("Gender").text().toInt());

            QSet<Group> groups;
            QDomElement groupsElement = contactsNode.firstChildElement("Groups");
            QDomElement groupElement = groupsElement.firstChildElement("GroupId");
            for (; !groupElement.isNull(); groupElement = groupElement.nextSiblingElement("GroupId"))
                if (importedGroups.contains(groupElement.text()))
                    groups.insert(importedGroups.value(groupElement.text()));

            if (!groups.isEmpty())
                buddy.setGroups(groups);

            QDomElement numberElement = contactElement.firstChildElement("GGNumber");
            if (!numberElement.text().isEmpty() && numberElement.text() != account.id())
            {
                Contact contact = m_contactStorage->create();
                contact.setContactAccount(account);
                contact.setId(numberElement.text());
                contact.data()->setState(StorableObject::StateNew);
                contact.setOwnerBuddy(buddy);
            }

            buddy.setAnonymous(false);
            result.append(buddy);
        }
    }

    return result;
}

Buddy GaduListHelper::linePre70ToBuddy(Account account, QStringList &sections)
{
    QSet<Group> groups;
    unsigned int i, secCount;
    bool ok = false;

    secCount = sections.count();

    if (secCount < 5)
        return Buddy::null;

    Buddy buddy = m_buddyStorage->create();

    buddy.setFirstName(sections[0]);
    buddy.setLastName(sections[1]);
    buddy.setNickName(sections[2]);
    buddy.setDisplay(sections[3]);
    buddy.setMobile(sections[4]);

    groups.clear();
    if (!sections[5].isEmpty())
        groups.insert(m_groupManager->byName(sections[5]));

    i = 6;
    while (!ok && i < secCount)
    {
        sections[i].toULong(&ok);
        ok = ok || sections[i].isEmpty();
        if (!ok)
            groups.insert(m_groupManager->byName(sections[i]));
        ++i;
    }
    buddy.setGroups(groups);
    --i;

    if (i < secCount)
    {
        UinType uin = sections[i++].toULong(&ok);
        if (!ok)
            uin = 0;
        if (uin && QString::number(uin) != account.id())
        {
            Contact contact = m_contactStorage->create();
            contact.setContactAccount(account);
            contact.setId(QString::number(uin));
            contact.data()->setState(StorableObject::StateNew);
            contact.setOwnerBuddy(buddy);
        }
    }

    if (i < secCount)
        buddy.setEmail(sections[i++]);

    if (i + 1 < secCount)
    {
        i += 2;
    }
    if (i + 1 < secCount)
    {
        i += 2;
    }

    if (i < secCount)
    {
        buddy.setOfflineTo(bool(sections[i].toInt()));
        i++;
    }

    if (i < secCount)
        buddy.setHomePhone(sections[i++]);

    buddy.setAnonymous(false);
    return buddy;
}

Buddy GaduListHelper::line70ToBuddy(Account account, QStringList &sections)
{
    QSet<Group> groups;
    unsigned int i, secCount;
    bool ok = false;

    secCount = sections.count();

    if (secCount < 6)
        return Buddy::null;

    Buddy buddy = m_buddyStorage->create();

    buddy.setFirstName(sections[0]);
    buddy.setLastName(sections[1]);
    buddy.setNickName(sections[2]);
    buddy.setDisplay(sections[3]);
    buddy.setMobile(sections[4]);

    if (!sections[5].isEmpty())
    {
        for (auto const &group : sections[5].split(',', QString::SkipEmptyParts))
            groups.insert(m_groupManager->byName(group));

        buddy.setGroups(groups);
    }

    i = 6;
    if (i < secCount)
    {
        UinType uin = sections[i++].toULong(&ok);
        if (!ok)
            uin = 0;
        if (uin && QString::number(uin) != account.id())
        {
            Contact contact = m_contactStorage->create();
            contact.setContactAccount(account);
            contact.setId(QString::number(uin));
            contact.data()->setState(StorableObject::StateNew);
            contact.setOwnerBuddy(buddy);
        }
    }

    if (i < secCount)
        buddy.setEmail(sections[i++]);

    if (i + 1 < secCount)
    {
        i += 2;
    }
    if (i + 1 < secCount)
    {
        i += 2;
    }

    if (i < secCount)
    {
        buddy.setOfflineTo(bool(sections[i].toInt()));
        i++;
    }

    if (i < secCount)
        buddy.setHomePhone(sections[i++]);

    buddy.setAnonymous(false);
    return buddy;
}
