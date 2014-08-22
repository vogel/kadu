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

#include "otr-path-service.h"

#include "core/application.h"
#include "misc/paths-provider.h"

#include <QtCore/QDir>

OtrPathService::OtrPathService(QObject *parent) :
		QObject{parent}
{
	m_dir = Application::instance()->pathsProvider()->profilePath() + QString("/keys/");

	QDir dir{m_dir};
	if (!dir.exists())
		dir.mkpath(".");
}

OtrPathService::~OtrPathService()
{
}

QString OtrPathService::fingerprintsStoreFilePath() const
{
	return m_dir + QString{"otr_fingerprints"};
}

QString OtrPathService::instanceTagsStoreFilePath() const
{
	return m_dir + QString{"otr_instance_tags"};
}

QString OtrPathService::privateKeysStoreFilePath() const
{
	return m_dir + QString{"otr_private"};
}
