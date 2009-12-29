/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPLIT_STATUS_CHANGER_H
#define SPLIT_STATUS_CHANGER_H

#include "status/status-changer.h"

class QTimer;

class KADUAPI SplitStatusChanger : public StatusChanger
{
	Q_OBJECT

	int splitSize;

	int descriptionSplitBegin;
	int descriptionSplitLength;

	QTimer *splitTimer;
	QString lastDescription;

	bool enabled;

private slots:
	void timerInvoked();

public:
	explicit SplitStatusChanger(unsigned int splitSize);
	virtual ~SplitStatusChanger();

	virtual void changeStatus(Status &status);

	void enable();
	void disable();

};

#endif // SPLIT_STATUS_CHANGER_H
