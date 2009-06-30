/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CONTAINER_AWARE_OBJECT
#define STATUS_CONTAINER_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class StatusContainer;

class KADUAPI StatusContainerAwareObject : public AwareObject<StatusContainerAwareObject>
{

protected:
	virtual void statusContainerRegistered(StatusContainer *statusContainer) = 0;
	virtual void statusContainerUnregistered(StatusContainer *statusContainer) = 0;

public:
	static void notifyStatusContainerRegistered(StatusContainer *statusContainer);
	static void notifyStatusContainerUnregistered(StatusContainer *statusContainer);

	void triggerAllStatusContainerRegistered();
	void triggerAllStatusContainerUnregistered();

};

#endif // STATUS_CONTAINER_AWARE_OBJECT
