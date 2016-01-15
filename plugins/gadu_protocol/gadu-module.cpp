/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-module.h"

#include "actions/gadu-protocol-menu-manager.h"
#include "server/gadu-servers-manager.h"
#include "helpers/gadu-list-helper.h"
#include "gadu-plugin-object.h"
#include "gadu-protocol-factory.h"
#include "gadu-url-dom-visitor-provider.h"
#include "gadu-url-handler.h"

GaduModule::GaduModule()
{
	add_type<GaduListHelper>();
	add_type<GaduPluginObject>();
	add_type<GaduProtocolFactory>();
	add_type<GaduProtocolMenuManager>();
	add_type<GaduServersManager>();
	add_type<GaduUrlDomVisitorProvider>();
	add_type<GaduUrlHandler>();
}
