/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CONTAINER_MANAGER_H
#define STATUS_CONTAINER_MANAGER_H

#include <QtCore/QObject>

#include "status/status-container.h"

class KADUAPI StatusContainerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusContainerManager)

	static StatusContainerManager *Instance;

	StatusContainerManager();
	virtual ~StatusContainerManager();

	void init();

	QList<StatusContainer *> StatusContainers;

public:
	static StatusContainerManager * instance();

	unsigned int count() const { return StatusContainers.count(); }
	const QList<StatusContainer *> statusContainers() const { return StatusContainers; }

	void registerStatusContainer(StatusContainer *statusContainer);
	void unregisterStatusContainer(StatusContainer *statusContainer);

signals:
	void statusContainerAboutToBeRegistered(StatusContainer *);
	void statusContainerRegistered(StatusContainer *);
	void statusContainerAboutToBeUnregistered(StatusContainer *);
	void statusContainerUnregistered(StatusContainer *);

};

#endif // STATUS_CONTAINER_MANAGER_H
