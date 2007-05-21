/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qtimer.h>

#include "debug.h"
#include "misc.h"

#include "status_changer.h"

StatusChanger::StatusChanger(int priority)
	: Priority(priority)
{
}

StatusChanger::~StatusChanger()
{
}

int StatusChanger::priority()
{
	return Priority;
}

UserStatusChanger::UserStatusChanger()
	: StatusChanger(0)
{
}

UserStatusChanger::~UserStatusChanger()

{
}

void UserStatusChanger::changeStatus(UserStatus &status)
{
	kdebugf();

	status = userStatus;

	kdebugf2();
}

void UserStatusChanger::userStatusSet(UserStatus &status)
{
	kdebugf();

	userStatus = status;
	emit statusChanged();

	kdebugf2();
}

SplitStatusChanger::SplitStatusChanger(unsigned int splitSize)
	: StatusChanger(1000), splitSize(splitSize), descriptionSplitBegin(0), descriptionSplitLength(0), splitTimer(0), enabled(false)
{
}

SplitStatusChanger::~SplitStatusChanger()
{
	if (splitTimer)
	{
		delete splitTimer;
		splitTimer = 0;
	}
}

void SplitStatusChanger::enable()
{
	if (!splitTimer)
	{
		splitTimer = new QTimer();
		connect(splitTimer, SIGNAL(timeout()), this, SLOT(timerInvoked()));
	}

	splitTimer->start(30000);

	enabled = true;
}

void SplitStatusChanger::disable()
{
	if (splitTimer)
	{
		delete splitTimer;
		splitTimer = 0;
	}

	enabled = false;
}

void SplitStatusChanger::changeStatus(UserStatus &status)
{
	kdebugf();

	if (lastDescription != status.description())
	{
		lastDescription = status.description();
		descriptionSplitBegin = 0;
		descriptionSplitLength = lastDescription.length();

		if (lastDescription.length() <= splitSize)
		{
			disable();
			return;
		}
		else
		{
			descriptionSplitLength = splitSize - 5;
			enable();
		}
	}

	if (!enabled)
		return;

	QString description = lastDescription.mid(descriptionSplitBegin, descriptionSplitLength);
	if (descriptionSplitBegin == 0)
		description.append(" --->");
	else if (descriptionSplitBegin + descriptionSplitLength == lastDescription.length())
		description = QString("<--- ").append(description);
	else
		description = QString("<--- ").append(description).append(" --->");

	status.setDescription(description);

	kdebugf2();
}

void SplitStatusChanger::timerInvoked()
{
	kdebugf();

	if (descriptionSplitBegin + descriptionSplitLength == lastDescription.length())
	{
		descriptionSplitBegin = 0;
		descriptionSplitLength = splitSize - 5;
	}
	else
	{
		descriptionSplitBegin = descriptionSplitBegin + descriptionSplitLength;

		if (descriptionSplitBegin + splitSize - 5 >= lastDescription.length())
			descriptionSplitLength = lastDescription.length() - descriptionSplitBegin;
		else
			descriptionSplitLength = splitSize - 10;
	}

	emit statusChanged();

	kdebugf2();
}

void StatusChangerManager::initModule()
{
	status_changer_manager = new StatusChangerManager();
}

void StatusChangerManager::closeModule()
{
	delete status_changer_manager;
	status_changer_manager = 0;
}

StatusChangerManager::StatusChangerManager()
	: enabled(false)
{
}

StatusChangerManager::~StatusChangerManager()
{
}

void StatusChangerManager::registerStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	connect(statusChanger, SIGNAL(statusChanged()), this, SLOT(statusChanged()));

	for (unsigned int i = 0; i < statusChangers.count(); i++)
		if ((*statusChangers.at(i))->priority() > statusChanger->priority())
		{
			statusChangers.insert(statusChangers.at(i), statusChanger);
			return;
		}

	statusChangers.insert(statusChangers.end(), statusChanger);
	statusChanged();

	kdebugf2();
}

void StatusChangerManager::unregisterStatusChanger(StatusChanger *statusChanger)
{
	kdebugf();

	if (statusChangers.remove(statusChanger))
	{
		disconnect(statusChanger, SIGNAL(statusChanged()), this, SLOT(statusChanged()));
		statusChanged();
	}

	kdebugf2();
}

void StatusChangerManager::statusChanged()
{
	kdebugf();

	if (!enabled)
		return;

	UserStatus status;
	for (unsigned int i = 0; i < statusChangers.count(); i++)
		(*statusChangers.at(i))->changeStatus(status);

	emit statusChanged(status);

	kdebugf2();
}

void StatusChangerManager::enable()
{
	if (enabled)
		return;

	enabled = true;
	statusChanged();
}

StatusChangerManager *status_changer_manager;
