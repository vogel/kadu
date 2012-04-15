/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "misc/misc.h"
#include "status/status-changer-manager.h"
#include "debug.h"

#include "filedesc.h"

#define MODULE_FILEDESC_VERSION 1.14

// Implementation of FileDescStatusChanger class

FileDescStatusChanger::FileDescStatusChanger(FileDescription *parent, QObject *parentObj) :
		StatusChanger(900, parentObj), Parent(parent)
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

	if (status.description().isEmpty() && !Parent->forceDesc())
		return;

	if (!status.description().isEmpty() && Parent->allowOther())
		return;

	status.setDescription(Title);
}

void FileDescStatusChanger::setTitle(const QString &title)
{
	Title = title;
	emit statusChanged(0);
}

// Implementation of FileDescription class

FileDescription::FileDescription(QObject *parent) :
		QObject(parent)
{
	kdebugf();

	createDefaultConfiguration();

	Timer = new QTimer(this);
	Timer->setSingleShot(false);
	Timer->setInterval(500);
	connect(Timer, SIGNAL(timeout()), this, SLOT(checkTitle()));
	Timer->start();

	StatusChanger = new FileDescStatusChanger(this, this);
	configurationUpdated();

	StatusChangerManager::instance()->registerStatusChanger(StatusChanger);
}

FileDescription::~FileDescription()
{
	kdebugf();
	Timer->stop();

	StatusChangerManager::instance()->unregisterStatusChanger(StatusChanger);
}

void FileDescription::configurationUpdated()
{
	File = config_file.readEntry("FileDesc", "file", KaduPaths::instance()->profilePath() + QLatin1String("description.txt"));
	ForceDesc = config_file.readBoolEntry("FileDesc", "forceDescr", true);
	AllowOther = config_file.readBoolEntry("FileDesc", "allowOther", true);

	checkTitle();
}

void FileDescription::checkTitle()
{
	QFile file(File);

	if (!file.exists())
		return;

	if (!file.open(QIODevice::ReadOnly))
		return;

	QString description;
	QTextStream stream(&file);
	if (!stream.atEnd())
		description = stream.readLine();
	file.close();

	StatusChanger->setTitle(description);
}

void FileDescription::createDefaultConfiguration()
{
	config_file.addVariable("FileDesc", "file", KaduPaths::instance()->profilePath() + QLatin1String("description.txt"));
	config_file.addVariable("FileDesc", "forceDescr", true);
	config_file.addVariable("FileDesc", "allowOther", true);
}
