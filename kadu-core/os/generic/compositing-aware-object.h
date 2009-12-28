/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COMPOSITING_AWARE_OBJECT
#define COMPOSITING_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class KADUAPI CompositingAwareObject : public AwareObject<CompositingAwareObject>
{
	static bool CompositingEnabled;
protected:
	virtual void compositingEnabled() = 0;
	virtual void compositingDisabled() = 0;

public:
	static void compositingStateChanged();

	void triggerCompositingStateChanged();

};

#endif // COMPOSITING_AWARE_OBJECT
