/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CRASG_AWARE_OBJECT_H
#define CRASG_AWARE_OBJECT_H

#include <QtCore/QList>

#include "aware-object.h"

class KADUAPI CrashAwareObject : public AwareObject<CrashAwareObject>
{

protected:
	virtual void crash();

public:
	static void notifyCrash();

};

#endif // CRASG_AWARE_OBJECT_H
