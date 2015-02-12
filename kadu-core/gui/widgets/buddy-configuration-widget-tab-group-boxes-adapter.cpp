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

#include "buddy-configuration-widget-tab-group-boxes-adapter.h"

#include "gui/widgets/buddy-configuration-widget.h"
#include "gui/windows/buddy-data-window.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

BuddyConfigurationWidgetTabGroupBoxesAdapter::BuddyConfigurationWidgetTabGroupBoxesAdapter(BuddyDataWindow *buddyDataWindow, QTabWidget *tabWidget, QObject *parent) :
		QObject{parent},
		m_buddyDataWindow{buddyDataWindow},
		m_tabWidget{tabWidget}
{
	if (!m_buddyDataWindow || !m_tabWidget)
		return;

	connect(m_buddyDataWindow, SIGNAL(widgetAdded(BuddyConfigurationWidget*)), this, SLOT(widgetAdded(BuddyConfigurationWidget*)));
	connect(m_buddyDataWindow, SIGNAL(widgetRemoved(BuddyConfigurationWidget*)), this, SLOT(widgetRemoved(BuddyConfigurationWidget*)));

	for (auto widget : m_buddyDataWindow->buddyConfigurationWidgets())
		widgetAdded(widget);
}

BuddyConfigurationWidgetTabGroupBoxesAdapter::~BuddyConfigurationWidgetTabGroupBoxesAdapter()
{
}

void BuddyConfigurationWidgetTabGroupBoxesAdapter::widgetAdded(BuddyConfigurationWidget *widget)
{
	if (!m_widget)
	{
		m_widget = new QWidget{m_buddyDataWindow};
		m_widgetLayout = new QVBoxLayout{m_widget};
		m_widgetLayout->setMargin(8);
		m_widgetLayout->setSpacing(4);
		m_widgetLayout->addStretch(100);

		m_tabWidget->addTab(m_widget, tr("Other"));
	}

	auto groupBox = new QGroupBox{m_widget};
	connect(widget, SIGNAL(destroyed(QObject*)), groupBox, SLOT(deleteLater()));
	groupBox->setFlat(true);
	groupBox->setTitle(widget->windowTitle());

	auto groupBoxLayout = new QVBoxLayout{groupBox};
	groupBoxLayout->setMargin(0);
	groupBoxLayout->setSpacing(4);
	groupBoxLayout->addWidget(widget);

	m_widgetLayout->insertWidget(m_widgetLayout->count() - 1, groupBox);
}

void BuddyConfigurationWidgetTabGroupBoxesAdapter::widgetRemoved(BuddyConfigurationWidget *widget)
{
	Q_UNUSED(widget);

	if (m_buddyDataWindow->buddyConfigurationWidgets().isEmpty() && m_widget)
	{
		m_tabWidget->removeTab(m_tabWidget->indexOf(m_widget));
		m_widget->deleteLater();
	}
}

#include "moc_buddy-configuration-widget-tab-group-boxes-adapter.cpp"
