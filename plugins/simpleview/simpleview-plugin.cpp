/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
 *
 * Objectives:
 * - simple view hides everything from the main window except the contact list,
 *   which can be still used to contact buddies. For any other action
 *   switching back to the normal view is required
 * - quickly accessible switching mechanism between views
 *
 */

#include "simpleview.h"

#include "simpleview-plugin.h"

SimpleViewPlugin::~SimpleViewPlugin()
{
}

int SimpleViewPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	SimpleView::createInstance();

	return 0;
}

void SimpleViewPlugin::done()
{
	SimpleView::destroyInstance();
}

Q_EXPORT_PLUGIN2(simpleview, SimpleViewPlugin)
