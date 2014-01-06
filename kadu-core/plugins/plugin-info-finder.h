/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <map>
#include <QtCore/QObject>
#include <QtCore/QPointer>

class PluginInfo;
class PluginInfoReader;

class PluginInfoFinder : public QObject
{
	Q_OBJECT

public:
	explicit PluginInfoFinder(QObject *parent = nullptr) noexcept;
	virtual ~PluginInfoFinder() noexcept;

	void setPluginInfoReader(PluginInfoReader *pluginInfoReader) noexcept;

	std::map<QString, PluginInfo> readPluginInfos(const QString &directory) noexcept;

private:
	QPointer<PluginInfoReader> m_pluginInfoReader;

};
