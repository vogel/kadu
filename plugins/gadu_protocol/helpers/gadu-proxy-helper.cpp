/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <libgadu.h>

#include "network/proxy/network-proxy.h"
#include "debug.h"

#include "gadu-proxy-helper.h"

void GaduProxyHelper::cleanUpProxySettings()
{
	delete [] gg_proxy_host;
	gg_proxy_host = 0;

	delete [] gg_proxy_username;
	gg_proxy_username = 0;

	delete [] gg_proxy_password;
	gg_proxy_password = 0;
}

void GaduProxyHelper::setupProxy(NetworkProxy networkProxy)
{
	kdebugf();

	GaduProxyHelper::cleanUpProxySettings();

	gg_proxy_enabled = !networkProxy.isNull() && !networkProxy.address().isEmpty();
	if (!gg_proxy_enabled)
		return;

	gg_proxy_host = qstrdup(networkProxy.address().toLatin1().constData());
	gg_proxy_port = networkProxy.port();

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_host = %s\n", gg_proxy_host);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_port = %d\n", gg_proxy_port);

	if (!networkProxy.user().isEmpty())
	{
		gg_proxy_username = qstrdup(networkProxy.user().toLatin1().constData());
		gg_proxy_password = qstrdup(networkProxy.password().toLatin1().constData());
	}
}
