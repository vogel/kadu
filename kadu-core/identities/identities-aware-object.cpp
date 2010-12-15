/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "identity-manager.h"

#include "identities-aware-object.h"

KADU_AWARE_CLASS(IdentitiesAwareObject)

void IdentitiesAwareObject::notifyIdentityAdded(Identity identity)
{
	foreach (IdentitiesAwareObject *object, Objects)
		object->identityAdded(identity);
}

void IdentitiesAwareObject::notifyIdentityRemoved(Identity identity)
{
	foreach (IdentitiesAwareObject *object, Objects)
		object->identityRemoved(identity);
}

void IdentitiesAwareObject::triggerAllIdentitiesAdded()
{
	foreach (const Identity &identity, IdentityManager::instance()->items())
		identityAdded(identity);
}

void IdentitiesAwareObject::triggerAllIdentitiesRemoved()
{
	foreach (const Identity &identity, IdentityManager::instance()->items())
		identityRemoved(identity);
}
