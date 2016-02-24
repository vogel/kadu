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

#include "hints-module.h"

#include "hint-manager.h"
#include "hint-repository.h"
#include "hints-configuration.h"
#include "hints-plugin-object.h"
#include "hints-widget-positioner.h"
#include "hints-widget.h"

HintsModule::HintsModule()
{
	add_type<HintsConfiguration>();
	add_type<HintManager>();
	add_type<HintRepository>();
	add_type<HintsPluginObject>();
	add_type<HintsWidgetPositioner>();
	add_type<HintsWidget>();
}
