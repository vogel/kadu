/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "identities-aware-object.h"

#include "identity-manager.h"

QList<IdentitiesAwareObject *> IdentitiesAwareObject::Objects;

void IdentitiesAwareObject::notifyIdentityAdded(Identity identity)
{
    for (IdentitiesAwareObject *object : Objects)
        object->identityAdded(identity);
}

void IdentitiesAwareObject::notifyIdentityRemoved(Identity identity)
{
    for (IdentitiesAwareObject *object : Objects)
        object->identityRemoved(identity);
}

IdentitiesAwareObject::IdentitiesAwareObject()
{
    Objects.append(this);
}

IdentitiesAwareObject::~IdentitiesAwareObject()
{
    Objects.removeAll(this);
}

void IdentitiesAwareObject::triggerAllIdentitiesAdded(IdentityManager *identityManager)
{
    for (const Identity &identity : identityManager->items())
        identityAdded(identity);
}

void IdentitiesAwareObject::triggerAllIdentitiesRemoved(IdentityManager *identityManager)
{
    for (const Identity &identity : identityManager->items())
        identityRemoved(identity);
}
