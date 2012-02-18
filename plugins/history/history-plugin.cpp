/*
 * %kadu copyright begin%
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "gui/history-buddy-data-window-addons.h"
#include "gui/history-chat-data-window-addons.h"
#include "gui/windows/history-window.h"
#include "misc/kadu-paths.h"

#include "buddy-history-delete-handler.h"
#include "history.h"

#include "history-plugin.h"

HistoryPlugin::HistoryPlugin()
{
}

HistoryPlugin::~HistoryPlugin()
{
}

int HistoryPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	History::createInstance();
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/history.ui"));
	MainConfigurationWindow::registerUiHandler(History::instance());

	BuddyHistoryDeleteHandler::createInstance();
	BuddyAdditionalDataDeleteHandlerManager::instance()->registerAdditionalDataDeleteHandler(BuddyHistoryDeleteHandler::instance());

	new HistoryBuddyDataWindowAddons(this);
	new HistoryChatDataWindowAddons(this);

	return 0;
}

void HistoryPlugin::done()
{
	BuddyAdditionalDataDeleteHandlerManager::instance()->unregisterAdditionalDataDeleteHandler(BuddyHistoryDeleteHandler::instance());
	BuddyHistoryDeleteHandler::destroyInstance();

	if (HistoryWindow::instance())
		delete HistoryWindow::instance();

	MainConfigurationWindow::unregisterUiHandler(History::instance());
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/history.ui"));
	History::destroyInstance();
}

Q_EXPORT_PLUGIN2(history, HistoryPlugin)
