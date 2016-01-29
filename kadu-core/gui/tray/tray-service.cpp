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

#include "tray-service.h"

#include "gui/tray/tray-position-provider.h"

#include <QtCore/QPoint>

TrayService::TrayService(QObject *parent) :
		QObject{parent}
{
}

TrayService::~TrayService()
{
}

void TrayService::setTrayPositionProvider(TrayPositionProvider *trayPositionProvider)
{
	m_trayPositionProvider = trayPositionProvider;
}

QPoint TrayService::trayPosition() const
{
	return m_trayPositionProvider
			? m_trayPositionProvider->trayPosition()
			: QPoint{0, 0};
}
