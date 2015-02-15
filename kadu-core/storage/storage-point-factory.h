/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class StoragePoint;
class ConfigurationApi;

class KADUAPI StoragePointFactory : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit StoragePointFactory(QObject *parent = 0);
	virtual ~StoragePointFactory();

	void setConfigurationFile(ConfigurationApi *configurationFile);

	std::unique_ptr<StoragePoint> createStoragePoint(const QString &nodeName, StoragePoint *parent = 0);

private:
	ConfigurationApi *m_configurationFile;

};
