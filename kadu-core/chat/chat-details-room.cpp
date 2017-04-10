/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-details-room.h"

#include "buddies/buddy-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "core/core.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

/**
 * @short Creates empty ChatDetailsRoom object.
 * @param chatData Chat object that will be decribed by this object
 *
 * Creates empty ChatDetailsRoom object assigned to chatData object.
 */
ChatDetailsRoom::ChatDetailsRoom(ChatShared *chatData)
        : ChatDetails{chatData}, m_stayInRoomAfterClosingWindow{false}, m_connected{false}
{
    Protocol *protocol = mainData()->chatAccount().protocolHandler();
    if (protocol)
        connect(protocol, SIGNAL(disconnected(Account)), this, SLOT(updateConnected()));
}

void ChatDetailsRoom::setChatTypeManager(ChatTypeManager *chatTypeManager)
{
    m_chatTypeManager = chatTypeManager;
}

ChatDetailsRoom::~ChatDetailsRoom()
{
}

/**
 * @short Loads ChatDetailsRoom object from storage.
 *
 * Loads ChatDetailsRoom object from the same storage assigned Chat object is
 * using. This loads room name of contacts from 'Chat' node
 */
void ChatDetailsRoom::load()
{
    if (!isValidStorage())
        return;

    ChatDetails::load();

    m_room = loadValue<QString>("Room");
    m_nick = loadValue<QString>("Nick");
    m_password = pwHash(loadValue<QString>("Password"));
    m_stayInRoomAfterClosingWindow = loadValue<bool>("StayInRoomAfterClosingWindow", false);
}

/**
 * @short Stores ChatDetailsRoom object to storage.
 *
 * Stores ChatDetailsRoom object to the same storage assigned Chat object is
 * using. This stores room name into 'Chat' subnode.
 */
void ChatDetailsRoom::store()
{
    if (!isValidStorage())
        return;

    ensureLoaded();

    storeValue("Room", m_room);
    storeValue("Nick", m_nick);
    storeValue("Password", pwHash(m_password));
    storeValue("StayInRoomAfterClosingWindow", m_stayInRoomAfterClosingWindow);
}

/**
 * @short Returns true if room name is not empty.
 * @return true if room name is not empty
 *
 * Returns true if assigned room name is not empty is not empty.
 */
bool ChatDetailsRoom::shouldStore()
{
    ensureLoaded();

    return StorableObject::shouldStore() && !m_room.isEmpty();
}

/**
 * @short Returns type of this chat - 'Room'.
 * @return 'Room' ChatType object
 *
 * Returns type of this chat - 'Room'.
 */
ChatType *ChatDetailsRoom::type() const
{
    return m_chatTypeManager->chatType("Room");
}

void ChatDetailsRoom::setRoom(const QString &room)
{
    if (room != m_room)
    {
        m_room = room;
        notifyChanged();
    }
}

QString ChatDetailsRoom::room() const
{
    return m_room;
}

void ChatDetailsRoom::setNick(const QString &nick)
{
    if (m_nick != nick)
    {
        m_nick = nick;
        notifyChanged();
    }
}

QString ChatDetailsRoom::nick() const
{
    return m_nick;
}

void ChatDetailsRoom::setPassword(const QString &password)
{
    if (m_password != password)
    {
        m_password = password;
        notifyChanged();
    }
}

QString ChatDetailsRoom::password() const
{
    return m_password;
}

void ChatDetailsRoom::setStayInRoomAfterClosingWindow(bool stayInRoomAfterClosingWindow)
{
    if (m_stayInRoomAfterClosingWindow != stayInRoomAfterClosingWindow)
    {
        m_stayInRoomAfterClosingWindow = stayInRoomAfterClosingWindow;
        notifyChanged();
    }
}

bool ChatDetailsRoom::stayInRoomAfterClosingWindow() const
{
    return m_stayInRoomAfterClosingWindow;
}

/**
 * @short Returns name of this chat.
 * @return name of this chat
 *
 * Returns name of this chat (which is name of room).
 */
QString ChatDetailsRoom::name() const
{
    return m_room;
}

void ChatDetailsRoom::updateConnected()
{
    Protocol *protocol = mainData()->chatAccount().protocolHandler();
    if (!protocol || !protocol->isConnected())
        setConnected(false);
}

void ChatDetailsRoom::setConnected(bool newConnected)
{
    Protocol *protocol = mainData()->chatAccount().protocolHandler();
    if (protocol && !protocol->isConnected())
        newConnected = false;

    if (m_connected == newConnected)
        return;

    m_connected = newConnected;
    if (m_connected)
        emit connected();
    else
        emit disconnected();
}

bool ChatDetailsRoom::isConnected() const
{
    Protocol *protocol = mainData()->chatAccount().protocolHandler();

    return protocol && protocol->isConnected() && m_connected;
}

void ChatDetailsRoom::addContact(const Contact &contact)
{
    ensureLoaded();

    if (m_contacts.contains(contact))
        return;

    emit contactAboutToBeAdded(contact);
    m_contacts.insert(contact);
    emit contactAdded(contact);
}

void ChatDetailsRoom::removeContact(const Contact &contact)
{
    ensureLoaded();

    if (!m_contacts.contains(contact))
        return;

    emit contactAboutToBeRemoved(contact);
    m_contacts.remove(contact);
    emit contactRemoved(contact);
}

#include "moc_chat-details-room.cpp"
