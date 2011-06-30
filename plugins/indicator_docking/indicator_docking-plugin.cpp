/*
 * %kadu copyright begin%
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
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

//#include "exports.h"

#include "indicator_docking.h"
#include "indicator_docking-plugin.h"

IndicatorDockingPlugin::~IndicatorDockingPlugin()
{
}

int IndicatorDockingPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	IndicatorDocking::createInstance();
	return 0;
}

void IndicatorDockingPlugin::done()
{
	IndicatorDocking::destroyInstance();
}

Q_EXPORT_PLUGIN2(indicator_docking, IndicatorDockingPlugin)
