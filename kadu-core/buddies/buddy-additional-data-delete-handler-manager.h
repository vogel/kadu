/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QObject>

class BuddyAdditionalDataDeleteHandler;

class KADUAPI BuddyAdditionalDataDeleteHandlerManager : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit BuddyAdditionalDataDeleteHandlerManager(QObject *parent = nullptr);
    virtual ~BuddyAdditionalDataDeleteHandlerManager();

    const QList<BuddyAdditionalDataDeleteHandler *> &items() const
    {
        return m_items;
    }
    BuddyAdditionalDataDeleteHandler *byName(const QString &name);

    void registerAdditionalDataDeleteHandler(BuddyAdditionalDataDeleteHandler *handler);
    void unregisterAdditionalDataDeleteHandler(BuddyAdditionalDataDeleteHandler *handler);

signals:
    void additionalDataDeleteHandlerRegistered(BuddyAdditionalDataDeleteHandler *handler);
    void additionalDataDeleteHandlerUnregistered(BuddyAdditionalDataDeleteHandler *handler);

private:
    QList<BuddyAdditionalDataDeleteHandler *> m_items;
};
