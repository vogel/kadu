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

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <map>
#include <memory>

class ActivePlugin;
class PluginActivationAction;

class KADUAPI PluginActivationService : public QObject
{
	Q_OBJECT

public:
	explicit PluginActivationService(QObject *parent = nullptr);
	virtual ~PluginActivationService();

	void performActivationAction(const PluginActivationAction &action) noexcept(false);
	bool isActive(const QString &name) const noexcept;
	QSet<QString> activePlugins() const noexcept;

private:
	using map = std::map<QString, std::unique_ptr<ActivePlugin>>;
	map m_activePlugins;

	void activatePlugin(const QString &name, bool firstTime) noexcept(false);
	void deactivatePlugin(const QString &name) noexcept;

};
