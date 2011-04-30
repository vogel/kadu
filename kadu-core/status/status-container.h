/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "exports.h"

class KaduIcon;
class StatusType;

class KADUAPI StatusContainer : public QObject
{
	Q_OBJECT

	// all status changes must be performed by Core
	friend class Core;

public:
	explicit StatusContainer(QObject *parent = 0) : QObject(parent) {}
	virtual ~StatusContainer() {}

	virtual QString statusContainerName() = 0;

	virtual void setStatus(Status newStatus, bool flush = true) = 0;
	virtual Status status() = 0;
	virtual bool isStatusSettingInProgress() = 0;

	virtual void setDescription(const QString &description, bool flush = true) = 0;

	virtual QString statusDisplayName() = 0;

	virtual KaduIcon statusIcon() = 0;
	virtual KaduIcon statusIcon(const Status &status) = 0;
	virtual KaduIcon statusIcon(const QString &statusType) = 0;

	virtual QList<StatusType *> supportedStatusTypes() = 0;

	virtual int maxDescriptionLength() = 0;

	virtual QString statusNamePrefix() { return QString(); }

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool StartupLastDescription) = 0;
	virtual void storeStatus(Status status) = 0;

signals:
	/**
	 * This signal is emitted when the status was explicitly changed or something in this
	 * StatusContainer was changed that might have caused implicit status change (but did not
	 * need to, so do not assume real status change after this signal).
	 */
	void statusUpdated();

};

#endif // STATUS_CONTAINER_H
