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
#include <memory>

class ConfigurationApi;
class DeprecatedConfigurationApi;

class KADUAPI Configuration final : public QObject
{
	Q_OBJECT

public:
	explicit Configuration(std::unique_ptr<ConfigurationApi> configurationApi, QObject *parent = nullptr);
	virtual ~Configuration();

	ConfigurationApi * api() const;
	DeprecatedConfigurationApi * deprecatedApi() const;

	void touch();
	QString content() const;

private:
	std::unique_ptr<ConfigurationApi> m_configurationApi;
	std::unique_ptr<DeprecatedConfigurationApi> m_deprecatedConfigurationApi;

};
