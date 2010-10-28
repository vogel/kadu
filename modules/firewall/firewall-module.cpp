/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "firewall-configuration-ui-handler.h"
#include "firewall-notification.h"
#include "firewall.h"

extern "C" int firewall_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	Firewall::createInstance();
	FirewallNotification::registerNotifications();
	FirewallConfigurationUiHandler::registerUiHandler();

	return 0;
}

extern "C" void firewall_close()
{
	FirewallConfigurationUiHandler::unregisterUiHandler();
	FirewallNotification::unregisterNotifications();
	Firewall::destroyInstance();
}
