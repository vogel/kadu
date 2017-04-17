/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "multilogon/multilogon-session.h"
#include "protocols/protocol.h"

#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"

#include "gadu-multilogon-service.h"
#include "gadu-multilogon-service.moc"

// for Q_OS_WIN macro
#include <QtCore/QDataStream>
#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <libgadu.h>

static bool operator==(const gg_multilogon_id_t &left, const gg_multilogon_id_t &right)
{
    for (int i = 0; i < 8; i++)
        if (left.id[i] != right.id[i])
            return false;

    return true;
}

GaduMultilogonService::GaduMultilogonService(Account account, QObject *parent) : MultilogonService(account, parent)
{
}

GaduMultilogonService::~GaduMultilogonService()
{
}

void GaduMultilogonService::setConnection(GaduConnection *connection)
{
    Connection = connection;
}

const QList<MultilogonSession> &GaduMultilogonService::sessions() const
{
    return Sessions;
}

void GaduMultilogonService::killSession(MultilogonSession session)
{
    if (!Connection || !Connection->hasSession())
        return;

    auto sessionId = toMultilogonId(session.id);
    auto writableSessionToken = Connection->writableSessionToken();
    gg_multilogon_disconnect(writableSessionToken.rawSession(), sessionId);
}

QByteArray GaduMultilogonService::toByteArray(const gg_multilogon_id_t &multilogonId)
{
    auto result = QByteArray{};
    QDataStream stream{&result, QIODevice::WriteOnly};

    stream.writeRawData(reinterpret_cast<const char *>(&multilogonId), sizeof(gg_multilogon_id_t));
    return result;
}

gg_multilogon_id_t GaduMultilogonService::toMultilogonId(QByteArray byteArray)
{
    auto result = gg_multilogon_id_t{};
    QDataStream stream{&byteArray, QIODevice::ReadOnly};

    stream.readRawData(reinterpret_cast<char *>(&result), sizeof(gg_multilogon_id_t));
    return result;
}

bool GaduMultilogonService::containsSession(const gg_multilogon_session &session)
{
    auto sessionId = toByteArray(session.id);
    for (auto const &multilogonSession : Sessions)
        if (sessionId == multilogonSession.id)
            return true;

    return false;
}

bool GaduMultilogonService::containsSession(
    const gg_event_multilogon_info &multilogonInfo, const gg_multilogon_id_t &id)
{
    for (int i = 0; i < multilogonInfo.count; i++)
        if (multilogonInfo.sessions[i].id == id)
            return true;

    return false;
}

void GaduMultilogonService::addNewSessions(const gg_event_multilogon_info &multilogonInfo)
{
    // this does not scale above 100 connections
    // but anyone will ever have that many?
    for (int i = 0; i < multilogonInfo.count; i++)
        if (!containsSession(multilogonInfo.sessions[i]))
        {
            auto remoteAddress = QHostAddress{};
            remoteAddress.setAddress(ntohl(multilogonInfo.sessions[i].remote_addr));

            auto logonTime = QDateTime{};
            logonTime.setTime_t(multilogonInfo.sessions[i].logon_time);

            auto session = MultilogonSession{account(), toByteArray(multilogonInfo.sessions[i].id),
                                             multilogonInfo.sessions[i].name, remoteAddress, logonTime};

            emit multilogonSessionAboutToBeConnected(session);
            Sessions.append(session);
            emit multilogonSessionConnected(session);
        }
}

void GaduMultilogonService::removeOldSessions(const gg_event_multilogon_info &multilogonInfo)
{
    // this does not scale above 100 connections
    // but anyone will ever have that many?
    auto i = Sessions.begin();

    while (i != Sessions.end())
    {
        auto session = *i;

        if (!containsSession(multilogonInfo, toMultilogonId(session.id)))
        {
            emit multilogonSessionAboutToBeDisconnected(session);
            i = Sessions.erase(i);
            emit multilogonSessionDisconnected(session);
        }
        else
            ++i;
    }
}

void GaduMultilogonService::handleEventMultilogonInfo(gg_event *e)
{
    addNewSessions(e->event.multilogon_info);
    removeOldSessions(e->event.multilogon_info);
}

void GaduMultilogonService::removeAllSessions()
{
    auto i = Sessions.begin();

    while (i != Sessions.end())
    {
        auto session = *i;
        emit multilogonSessionAboutToBeDisconnected(session);
        i = Sessions.erase(i);
        emit multilogonSessionDisconnected(session);
    }
}
