/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASE_STATUS_CONTAINER_H
#define BASE_STATUS_CONTAINER_H

#include "status/status-container.h"

#include "exports.h"

class StorableObject;

class KADUAPI BaseStatusContainer : public StatusContainer
{
	StorableObject *MyStorableObject;

public:
	BaseStatusContainer(StorableObject *storableObject);

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
			const QString &startupDescription, bool StartupLastDescription);
	virtual void disconnectAndStoreLastStatus(bool disconnectWithCurrentDescription,
			const QString &disconnectDescription);

};

#endif // BASE_STATUS_CONTAINER_H
