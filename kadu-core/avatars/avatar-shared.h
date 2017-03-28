/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "storage/shared.h"

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QPixmap>
#include <injeqt/injeqt.h>

class AvatarManager;
class PathsProvider;

class KADUAPI AvatarShared : public Shared
{
    Q_OBJECT

public:
    explicit AvatarShared(const QUuid &uuid = QUuid());
    virtual ~AvatarShared();

    virtual StorableObject *storageParent();
    virtual QString storageNodeName();

    virtual void storeAvatar();
    virtual void aboutToBeRemoved();

    QString filePath();
    QString smallFilePath();
    void setFilePath(const QString &filePath);

    bool isEmpty();

    void setPixmap(const QPixmap &pixmap);
    KaduShared_PropertyRead(const QPixmap &, pixmap, Pixmap)

        KaduShared_Property(const QDateTime &, lastUpdated, LastUpdated)
            KaduShared_Property(const QDateTime &, nextUpdate, NextUpdate)

                signals : void updated();
    void pixmapUpdated();

protected:
    virtual void load();
    virtual void store();
    virtual bool shouldStore();

private:
    QPointer<AvatarManager> m_avatarManager;
    QPointer<PathsProvider> m_pathsProvider;

    QDateTime LastUpdated;
    QDateTime NextUpdate;
    QString FilePath;
    QString SmallFilePath;
    QPixmap Pixmap;

    QString AvatarsDir;

    QString filePathToSmallFilePath(const QString &fileName);
    void ensureSmallPixmapExists();
    bool isPixmapSmall();
    void storeSmallPixmap();

private slots:
    INJEQT_SET void setAvatarManager(AvatarManager *avatarManager);
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_INIT void init();
};
