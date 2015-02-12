/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-configuration-widget-tab-adapter.h"

#include "gui/widgets/buddy-configuration-widget.h"
#include "gui/windows/buddy-data-window.h"

#include <QtWidgets/QTabWidget>

BuddyConfigurationWidgetTabAdapter::BuddyConfigurationWidgetTabAdapter(BuddyDataWindow *buddyDataWindow, QTabWidget *tabWidget, QObject *parent) :
		QObject{parent},
		m_buddyDataWindow{buddyDataWindow},
		m_tabWidget{tabWidget}
{
	if (!m_buddyDataWindow || !m_tabWidget)
		return;

	connect(m_buddyDataWindow, SIGNAL(widgetAdded(BuddyConfigurationWidget*)), this, SLOT(widgetAdded(BuddyConfigurationWidget*)));

	for (auto widget : m_buddyDataWindow->buddyConfigurationWidgets())
		widgetAdded(widget);
}

BuddyConfigurationWidgetTabAdapter::~BuddyConfigurationWidgetTabAdapter()
{
}

void BuddyConfigurationWidgetTabAdapter::widgetAdded(BuddyConfigurationWidget *widget)
{
	m_tabWidget->addTab(widget, widget->windowTitle());
}

#include "moc_buddy-configuration-widget-tab-adapter.cpp"
