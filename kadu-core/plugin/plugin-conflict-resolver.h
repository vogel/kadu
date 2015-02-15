/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <set>

class PluginDependencyHandler;

class KADUAPI PluginConflictResolver : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit PluginConflictResolver(QObject *parent = nullptr);
	virtual ~PluginConflictResolver();

	std::set<QString> conflictingPlugins(const std::set<QString> &activePluginSet, const QString &pluginName);

private:
	QPointer<PluginDependencyHandler> m_pluginDependencyHandler;

public slots: // TODO: make private, public only because of tests, should be done by injector
	INJEQT_SETTER void setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler);

};
