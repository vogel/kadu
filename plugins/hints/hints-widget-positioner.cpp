/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hints-widget-positioner.h"

#include "hints-configuration.h"
#include "hints-widget.h"

#include <QtCore/QRect>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

HintsWidgetPositioner::HintsWidgetPositioner(QObject *parent) :
		QObject{parent}
{
}

HintsWidgetPositioner::~HintsWidgetPositioner()
{
}

void HintsWidgetPositioner::setHintsConfiguration(HintsConfiguration *hintsConfiguration)
{
	m_hintsConfiguration = hintsConfiguration;
}

void HintsWidgetPositioner::setHintsWidget(HintsWidget *hintsWidget)
{
	m_hintsWidget = hintsWidget;
}

void HintsWidgetPositioner::init()
{
	connect(m_hintsConfiguration, &HintsConfiguration::updated, this, &HintsWidgetPositioner::update);
	connect(m_hintsWidget, &HintsWidget::sizeChanged, this, &HintsWidgetPositioner::update);
	connect(m_hintsWidget, &HintsWidget::shown, this, &HintsWidgetPositioner::update);
	update();
}

void HintsWidgetPositioner::update()
{
	auto minimumWidth = m_hintsConfiguration->minimumWidth();
	auto maximumWidth = m_hintsConfiguration->maximumWidth();

	auto width = qBound(minimumWidth, m_hintsWidget->sizeHint().width(), maximumWidth);

	m_hintsWidget->setFixedWidth(width);
	m_hintsWidget->move(positionForSize(m_hintsWidget->size()));
}

QPoint HintsWidgetPositioner::positionForSize(QSize size)
{
	auto availableGeometry = QApplication::desktop()->availableGeometry(m_hintsWidget);
	switch (m_hintsConfiguration->corner())
	{
		case HintsConfiguration::Corner::TopLeft:
			return availableGeometry.topLeft() + QPoint{8, 8};
		case HintsConfiguration::Corner::TopRight:
			return availableGeometry.topRight() + QPoint{-8 - size.width(), 8};
		case HintsConfiguration::Corner::BottomLeft:
			return availableGeometry.bottomLeft() + QPoint{8, -8 - size.height()};
		case HintsConfiguration::Corner::BottomRight:
		default:
			return availableGeometry.bottomRight() + QPoint{-8 - size.width(), -8 - size.height()};
	}
}
