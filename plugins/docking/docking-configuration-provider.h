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

#pragma once

#include "docking-configuration.h"

#include "configuration/configuration-aware-object.h"

#include <QtCore/QObject>

class DockingConfigurationProvider final : public QObject, private ConfigurationAwareObject
{
	Q_OBJECT

public:
	explicit DockingConfigurationProvider(QObject *parent = nullptr);
	virtual ~DockingConfigurationProvider();

	const DockingConfiguration & configuration() const;

signals:
	void updated();

protected:
	virtual void configurationUpdated() override;

private:
	DockingConfiguration m_configuration;

	void createDefaultConfiguration();

};
