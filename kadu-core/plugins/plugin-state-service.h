/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>

enum class PluginState;

class KADUAPI PluginStateService : public QObject
{
	Q_OBJECT

public:
	explicit PluginStateService(QObject *parent = nullptr);
	virtual ~PluginStateService();

	QMap<QString, PluginState> pluginStates() const;
	void setPluginStates(const QMap<QString, PluginState> &pluginStates);

	PluginState pluginState(const QString &pluginName) const;
	void setPluginState(const QString &pluginName, PluginState state);

private:
	QMap<QString, PluginState> m_pluginStates;

};
