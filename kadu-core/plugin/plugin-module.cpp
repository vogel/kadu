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

#include "plugin-module.h"

#include "plugin/activation/plugin-activation-error-handler.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugin/metadata/plugin-metadata-finder.h"
#include "plugin/metadata/plugin-metadata-reader.h"
#include "plugin/plugin-conflict-resolver.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/plugin-manager.h"
#include "plugin/state/plugin-state-manager.h"
#include "plugin/state/plugin-state-service.h"

PluginModule::PluginModule()
{
	add_type<PluginActivationErrorHandler>();
	add_type<PluginActivationService>();
	add_type<PluginConflictResolver>();
	add_type<PluginDependencyHandler>();
	add_type<PluginDependencyGraphBuilder>();
	add_type<PluginManager>();
	add_type<PluginMetadataFinder>();
	add_type<PluginMetadataReader>();
	add_type<PluginStateManager>();
	add_type<PluginStateService>();
}
