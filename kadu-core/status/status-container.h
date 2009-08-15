/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CONTAINER_H
#define STATUS_CONTAINER_H

#include "status/status.h"

class StatusType;

class StatusContainer : public QObject
{
	Q_OBJECT

public:
	virtual QString statusContainerName() = 0;

	virtual void setStatus(Status newStatus) = 0;
	virtual Status status() = 0;

	virtual QString statusName() = 0;
	virtual QPixmap statusPixmap() = 0;
	virtual QPixmap statusPixmap(const QString &statusType) = 0;

	virtual QList<StatusType *> supportedStatusTypes() = 0;

	virtual int maxDescriptionLength() = 0;

	virtual QString statusNamePrefix() { return QString(""); }

signals:
	void statusChanged();

};

#endif // STATUS_CONTAINER_H
