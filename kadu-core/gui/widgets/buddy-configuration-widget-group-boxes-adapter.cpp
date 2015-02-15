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

#include "buddy-configuration-widget-group-boxes-adapter.h"

#include "gui/widgets/buddy-configuration-widget.h"
#include "gui/windows/buddy-data-window.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

BuddyConfigurationWidgetGroupBoxesAdapter::BuddyConfigurationWidgetGroupBoxesAdapter(BuddyDataWindow *buddyDataWindow, QWidget *widget) :
		QObject{buddyDataWindow},
		m_buddyDataWindow{buddyDataWindow},
		m_widget{widget}
{
	if (!m_buddyDataWindow || !m_widget)
		return;

	connect(m_buddyDataWindow, SIGNAL(widgetAdded(BuddyConfigurationWidget*)), this, SLOT(widgetAdded(BuddyConfigurationWidget*)));

	for (auto buddyConfigurationWidget : m_buddyDataWindow->buddyConfigurationWidgets())
		widgetAdded(buddyConfigurationWidget);
}

BuddyConfigurationWidgetGroupBoxesAdapter::~BuddyConfigurationWidgetGroupBoxesAdapter()
{
}

void BuddyConfigurationWidgetGroupBoxesAdapter::widgetAdded(BuddyConfigurationWidget *widget)
{
	auto groupBox = new QGroupBox{m_widget};
	connect(widget, SIGNAL(destroyed(QObject*)), groupBox, SLOT(deleteLater()));
	groupBox->setFlat(true);
	groupBox->setTitle(widget->windowTitle());

	auto groupBoxLayout = new QVBoxLayout{groupBox};
	groupBoxLayout->setMargin(0);
	groupBoxLayout->setSpacing(4);
	groupBoxLayout->addWidget(widget);

	auto layout = qobject_cast<QBoxLayout *>(m_widget->layout());
	if (layout)
		layout->insertWidget(layout->count() - 1, groupBox);
}

#include "moc_buddy-configuration-widget-group-boxes-adapter.cpp"
