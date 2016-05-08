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

#include "main-window-repository.h"

MainWindowRepository::MainWindowRepository(QObject *parent) :
		QObject{parent}
{
}

MainWindowRepository::~MainWindowRepository()
{
}

void MainWindowRepository::addMainWindow(QWidget *mainWindow)
{
	auto it = std::find(begin(), end(), mainWindow);
	if (it == end())
	{
		m_data.push_back(mainWindow);
		emit mainWindowAdded(mainWindow);
	}
}

void MainWindowRepository::removeMainWindow(QWidget *mainWindow)
{
	auto it = std::find(begin(), end(), mainWindow);
	if (it != end())
	{
		m_data.erase(it);
		emit mainWindowRemoved(mainWindow);
	}
}

MainWindowRepository::Iterator MainWindowRepository::begin()
{
	return std::begin(m_data);
}

MainWindowRepository::Iterator MainWindowRepository::end()
{
	return std::end(m_data);
}

#include "main-window-repository.moc"
