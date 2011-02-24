/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/actions/actions.h"

#include "toolbar-configuration-manager.h"

ToolbarConfigurationManager::ToolbarConfigurationManager(QObject *parent) :
		QObject(parent)
{
	connect(Actions::instance(), SIGNAL(actionLoaded(QString)), this, SIGNAL(configurationUpdated()));
	connect(Actions::instance(), SIGNAL(actionUnloaded(QString)), this, SIGNAL(configurationUpdated()));
}

ToolbarConfigurationManager::~ToolbarConfigurationManager()
{
	disconnect(Actions::instance(), SIGNAL(actionLoaded(QString)), this, SIGNAL(configurationUpdated()));
	disconnect(Actions::instance(), SIGNAL(actionUnloaded(QString)), this, SIGNAL(configurationUpdated()));
}

void ToolbarConfigurationManager::notifyConfigurationUpdated()
{
	// do not use '0' here, with '50' there is no flicking
	QTimer::singleShot(50, this, SIGNAL(configurationUpdated()));
}
