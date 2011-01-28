/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef STATUS_CONTAINER_H
#define STATUS_CONTAINER_H

#include "status/status.h"

class StatusType;

class StatusContainer : public QObject
{
	Q_OBJECT

	// all status changes must be performed by Core
	friend class Core;

protected:
	void emitStatusContainerUpdated() { emit updated(); }

public:
	explicit StatusContainer(QObject *parent = 0) : QObject(parent) {}
	virtual ~StatusContainer() {}

	virtual QString statusContainerName() = 0;
	
	virtual void setStatus(Status newStatus) = 0;
	virtual Status status() = 0;

	virtual void setDescription(const QString &description) = 0;

	virtual QString statusDisplayName() = 0;
	virtual QIcon statusIcon() = 0;
	virtual QIcon statusIcon(Status status) = 0;
	virtual QString statusIconPath(const QString &statusType) = 0;
	virtual QIcon statusIcon(const QString &statusType) = 0;

	virtual QList<StatusType *> supportedStatusTypes() = 0;

	virtual int maxDescriptionLength() = 0;

	virtual QString statusNamePrefix() { return QString(); }

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool StartupLastDescription) = 0;
	virtual void storeStatus(Status status) = 0;
	virtual void disconnectStatus(bool disconnectWithCurrentDescription, const QString &disconnectDescription) = 0;

signals:
	void statusChanged();
	void updated();

};

#endif // STATUS_CONTAINER_H
