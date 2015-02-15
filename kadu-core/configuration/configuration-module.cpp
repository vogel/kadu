/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration-module.h"

#include "configuration/configuration-factory.h"
#include "configuration/configuration-path-provider.h"
#include "configuration/configuration-writer.h"
#include "configuration/configuration.h"

ConfigurationModule::ConfigurationModule()
{
	add_type<ConfigurationFactory>();
	add_type<ConfigurationPathProvider>();
	add_type<ConfigurationWriter>();

	add_factory<Configuration, ConfigurationFactory>();
}
