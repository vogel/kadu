/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "indicator-docking-plugin.h"

#include "indicator-docking.h"

#include "chat/chat-manager.h"
#include "core/core.h"
#include "status/status-container-manager.h"

IndicatorDockingPlugin::~IndicatorDockingPlugin()
{
}

bool IndicatorDockingPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	m_indicatorDocking.reset(new IndicatorDocking{});
	m_indicatorDocking->setChatManager(ChatManager::instance());
	m_indicatorDocking->setChatWidgetManager(Core::instance()->chatWidgetManager());
	m_indicatorDocking->setStatusContainer(StatusContainerManager::instance());
	m_indicatorDocking->setUnreadMessageRepository(Core::instance()->unreadMessageRepository());

	return true;
}

void IndicatorDockingPlugin::done()
{
	m_indicatorDocking.reset();
}

Q_EXPORT_PLUGIN2(indicator_docking, IndicatorDockingPlugin)

#include "moc_indicator-docking-plugin.cpp"
