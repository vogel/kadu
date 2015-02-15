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

#include <QtCore/QTimer>

#include "toolbar-configuration-manager.h"

ToolbarConfigurationManager::ToolbarConfigurationManager(QObject *parent) :
		QObject(parent)
{
}

ToolbarConfigurationManager::~ToolbarConfigurationManager()
{
}

void ToolbarConfigurationManager::notifyConfigurationUpdated()
{
	// do not use '0' here, with '50' there is no flicking
	QTimer::singleShot(50, this, SIGNAL(configurationUpdated()));
}

#include "moc_toolbar-configuration-manager.cpp"
