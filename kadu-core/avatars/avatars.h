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

#include "exports.h"

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class PathsProvider;
struct AvatarId;

class QPixmap;

const auto AVATAR_SIZE = 96;

class KADUAPI Avatars : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit Avatars(QObject *parent = nullptr);
    virtual ~Avatars();

    bool contains(const AvatarId &id) const;

    QPixmap pixmap(const std::vector<AvatarId> &ids);
    QPixmap pixmap(const AvatarId &id);

    QString path(const std::vector<AvatarId> &ids) const;
    QString path(const AvatarId &id) const;

    void update(const AvatarId &id, QPixmap avatar);
    void remove(const AvatarId &id);

signals:
    void updated(const AvatarId &id);

private:
    QPointer<PathsProvider> m_pathsProvider;

    QDir m_dir;
    std::map<AvatarId, QPixmap> m_avatars;

    bool ensureDirExists() const;
    QPixmap load(const AvatarId &id) const;

private slots:
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_INIT void init();
};
