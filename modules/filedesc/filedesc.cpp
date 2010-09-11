/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "status/status-changer-manager.h"
#include "debug.h"
#include "misc/misc.h"

#include "filedesc.h"

#define MODULE_FILEDESC_VERSION 1.14

FileDescription *file_desc;

extern "C" KADU_EXPORT int filedesc_init()
{
	file_desc = new FileDescription();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/filedesc.ui"));
	MainConfigurationWindow::registerUiHandler(file_desc);

	return 0;
}

extern "C" KADU_EXPORT void filedesc_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/filedesc.ui"));
	MainConfigurationWindow::unregisterUiHandler(file_desc);

	delete file_desc;
}

// Implementation of FileDescStatusChanger class

FileDescStatusChanger::FileDescStatusChanger()
	: StatusChanger(900), disabled(true)
{
}

FileDescStatusChanger::~FileDescStatusChanger()
{
}

void FileDescStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	Q_UNUSED(container)

	if (status.isDisconnected())
		return;
	if (!disabled)
		status.setDescription(title);
}

void FileDescStatusChanger::setTitle(const QString &newTitle)
{
	disabled = false;

	if (newTitle != title)
	{
		title = newTitle;
		emit statusChanged(0);
	}
}

void FileDescStatusChanger::disable()
{
	if (!disabled)
	{
		disabled = true;
		emit statusChanged(0);
	}
}

// Implementation of FileDescription class

FileDescription::FileDescription()
{
	kdebugf();

	createDefaultConfiguration();

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(checkTitle()));
	timer->start(500);

	fileDescStatusChanger = new FileDescStatusChanger();
	StatusChangerManager::instance()->registerStatusChanger(fileDescStatusChanger);
}

FileDescription::~FileDescription()
{
	kdebugf();
	disconnect(timer, SIGNAL(timeout()), this, SLOT(checkTitle()));
	delete timer;

	StatusChangerManager::instance()->unregisterStatusChanger(fileDescStatusChanger);
	delete fileDescStatusChanger;
	fileDescStatusChanger = 0;
}

// FIXME: Po migracji do 0.6.6 nie działa opcja "allowOther"
void FileDescription::checkTitle()
{
	QFile file(config_file.readEntry("FileDesc", "file"));

	if (!file.exists())
		return;

	if (!file.open(QIODevice::ReadOnly))
		return;

	QString desc = "";
	QTextStream stream(&file);
	if (!stream.atEnd())
		desc = stream.readLine();
	file.close();

	foreach(Account account, AccountManager::instance()->items())
	{
		if (!account.statusContainer()->status().isDisconnected())
		{
			if ((desc != currDesc
							|| (account.statusContainer()->status().description() != desc
									&& !config_file.readBoolEntry("FileDesc", "allowOther")
								)
							|| (account.statusContainer()->status().description().isEmpty()
									&& config_file.readBoolEntry("FileDesc", "forceDescr")
								)
				) && !account.statusContainer()->status().isDisconnected())
			{
				currDesc = desc;
				fileDescStatusChanger->setTitle(desc);
			}
		}
	}
}

void FileDescription::createDefaultConfiguration()
{
	config_file.addVariable("FileDesc", "file", profilePath("description.txt"));
	config_file.addVariable("FileDesc", "forceDescr", true);
	config_file.addVariable("FileDesc", "allowOther", true);
}
