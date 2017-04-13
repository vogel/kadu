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

#include "avatars.h"
#include "avatars.moc"

#include "avatars/avatar-id.h"
#include "contacts/contact-global-id.h"
#include "misc/paths-provider.h"

#include <QtGui/QPixmap>

Avatars::Avatars(QObject *parent) : QObject{parent}
{
}

Avatars::~Avatars() = default;

void Avatars::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void Avatars::init()
{
    m_dir = QDir{m_pathsProvider->profilePath() + QStringLiteral("avatars")};
}

bool Avatars::ensureDirExists() const
{
    if (m_dir.exists())
        return true;

    return m_dir.mkpath(QStringLiteral("."));
}

bool Avatars::contains(const AvatarId &id) const
{
    auto it = m_avatars.find(id);
    return (it != std::end(m_avatars) && !it->second.isNull()) || QFileInfo{path(id)}.isReadable();
}

QPixmap Avatars::pixmap(const std::vector<AvatarId> &ids)
{
    for (auto const &id : ids)
    {
        auto result = pixmap(id);
        if (!result.isNull())
            return result;
    }

    return {};
}

QPixmap Avatars::pixmap(const AvatarId &id)
{
    auto it = m_avatars.find(id);
    if (it != std::end(m_avatars))
        return it->second;

    auto pixmap = load(id);
    m_avatars.emplace(id, pixmap);
    emit updated(id);
    return pixmap;
}

QPixmap Avatars::load(const AvatarId &id) const
{
    auto result = QPixmap{};
    if (!contains(id))
        return {};

    result.load(path(id));
    return result;
}

QString Avatars::path(const std::vector<AvatarId> &ids) const
{
    auto it = std::find_if(std::begin(ids), std::end(ids), [this](const AvatarId &id) { return contains(id); });
    if (it != std::end(ids))
        return path(*it);
    else
        return {};
}

QString Avatars::path(const AvatarId &id) const
{
    return QString{"%1/%2.png"}.arg(m_dir.absolutePath()).arg(id.value);
}

void Avatars::update(const AvatarId &id, QPixmap avatar)
{
    if (avatar.isNull())
    {
        remove(id);
        return;
    }

    avatar = avatar.scaled(AVATAR_SIZE, AVATAR_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (ensureDirExists())
        avatar.save(path(id), "PNG");
    m_avatars[id] = std::move(avatar);
    emit updated(id);
}

void Avatars::remove(const AvatarId &id)
{
    m_avatars.erase(id);
    QFile::remove(path(id));
    emit updated(id);
}
