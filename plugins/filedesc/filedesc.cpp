/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "filedesc.h"

#include "filedesc-status-changer.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "misc/paths-provider.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>

FileDescription::FileDescription(QObject *parent) :
		QObject(parent)
{
	m_timer = new QTimer{this};
	m_timer->setSingleShot(false);
	m_timer->setInterval(500);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(checkTitle()));
	m_timer->start();
}

FileDescription::~FileDescription()
{
	m_timer->stop();
}

void FileDescription::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void FileDescription::setFileDescStatusChanger(FileDescStatusChanger *fileDescStatusChanger)
{
	m_fileDescStatusChanger = fileDescStatusChanger;
}

void FileDescription::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void FileDescription::init()
{
	createDefaultConfiguration();
	configurationUpdated();
}

void FileDescription::configurationUpdated()
{
	m_file = m_configuration->deprecatedApi()->readEntry("FileDesc", "file", m_pathsProvider->profilePath() + QStringLiteral("description.txt"));
	m_forceDesc = m_configuration->deprecatedApi()->readBoolEntry("FileDesc", "forceDescr", true);
	m_allowOther = m_configuration->deprecatedApi()->readBoolEntry("FileDesc", "allowOther", true);

	checkTitle();
}

void FileDescription::checkTitle()
{
	QFile file{m_file};

	if (!file.exists())
		return;

	if (!file.open(QIODevice::ReadOnly))
		return;

	QString description;
	QTextStream stream(&file);
	if (!stream.atEnd())
		description = stream.readLine();
	file.close();

	m_fileDescStatusChanger->setTitle(description);
}

void FileDescription::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("FileDesc", "file", m_pathsProvider->profilePath() + QStringLiteral("description.txt"));
	m_configuration->deprecatedApi()->addVariable("FileDesc", "forceDescr", true);
	m_configuration->deprecatedApi()->addVariable("FileDesc", "allowOther", true);
}

#include "moc_filedesc.cpp"
