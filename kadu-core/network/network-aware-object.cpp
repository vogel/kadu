/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "network/network-manager.h"

#include "network-aware-object.h"

KADU_AWARE_CLASS(NetworkAwareObject)

NetworkAwareObject::NetworkAwareObject()
{
	NetworkManager::instance(); // initialize, just in case
}

void NetworkAwareObject::notifyOnlineStateChanged(bool available)
{
	foreach (NetworkAwareObject *object, Objects)
		object->onlineStateChanged(available);
}

bool NetworkAwareObject::isOnline()
{
	return NetworkManager::instance()->isOnline();
}

void NetworkAwareObject::triggerOnlineStateChanged()
{
	onlineStateChanged(NetworkManager::instance()->isOnline());
}
