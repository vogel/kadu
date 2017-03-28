/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy-additional-data-delete-handler.h"

#include "buddy-additional-data-delete-handler-manager.h"

BuddyAdditionalDataDeleteHandlerManager::BuddyAdditionalDataDeleteHandlerManager(QObject *parent) : QObject(parent)
{
}

BuddyAdditionalDataDeleteHandlerManager::~BuddyAdditionalDataDeleteHandlerManager()
{
}

BuddyAdditionalDataDeleteHandler *BuddyAdditionalDataDeleteHandlerManager::byName(const QString &name)
{
    for (auto handler : m_items)
        if (name == handler->name())
            return handler;

    return 0;
}

void BuddyAdditionalDataDeleteHandlerManager::registerAdditionalDataDeleteHandler(
    BuddyAdditionalDataDeleteHandler *handler)
{
    m_items.append(handler);
    emit additionalDataDeleteHandlerRegistered(handler);
}

void BuddyAdditionalDataDeleteHandlerManager::unregisterAdditionalDataDeleteHandler(
    BuddyAdditionalDataDeleteHandler *handler)
{
    m_items.removeAll(handler);
    emit additionalDataDeleteHandlerUnregistered(handler);
}

#include "moc_buddy-additional-data-delete-handler-manager.cpp"
