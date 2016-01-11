/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "network-module.h"

#include "network/kadu-network-config.h"
#include "network/proxy/network-proxy-manager.h"
#include NETWORK_IMPLEMENTATION_INCLUDE

NetworkModule::NetworkModule()
{
	add_type<NetworkProxyManager>();
	add_type<NETWORK_IMPLEMENTATION_CLASS_NAME>();
}

NetworkModule::~NetworkModule()
{
}
