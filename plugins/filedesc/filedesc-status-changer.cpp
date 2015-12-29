/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "filedesc-status-changer.h"

#include "filedesc.h"

#include "status/status.h"

FileDescStatusChanger::FileDescStatusChanger(QObject *parent) :
		StatusChanger{900, parent}
{
}

FileDescStatusChanger::~FileDescStatusChanger()
{
}

void FileDescStatusChanger::setFileDescription(FileDescription *fileDescription)
{
	m_fileDescription = fileDescription;
}

void FileDescStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	Q_UNUSED(container)

	if (status.isDisconnected())
		return;

	if (status.description().isEmpty() && !m_fileDescription->forceDesc())
		return;

	if (!status.description().isEmpty() && m_fileDescription->allowOther())
		return;

	status.setDescription(m_title);
}

void FileDescStatusChanger::setTitle(const QString &title)
{
	m_title = title;
	emit statusChanged(0);
}
