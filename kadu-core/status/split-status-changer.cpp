/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>

#include "status/status.h"
#include "debug.h"

#include "split-status-changer.h"

SplitStatusChanger::SplitStatusChanger(unsigned int splitSize) :
		StatusChanger(1000), splitSize(splitSize), descriptionSplitBegin(0), descriptionSplitLength(0), splitTimer(0), enabled(false)
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

void SplitStatusChanger::changeStatus(Status &status)
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
