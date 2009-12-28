/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "compositing-aware-object.h"

KADU_AWARE_CLASS(CompositingAwareObject)

bool CompositingAwareObject::CompositingEnabled = false;

void CompositingAwareObject::compositingStateChanged()
{
	CompositingEnabled = !CompositingEnabled;

	if (CompositingEnabled)
	{
		foreach (CompositingAwareObject *object, Objects)
			object->compositingEnabled();
	}
	else
	{
		foreach (CompositingAwareObject *object, Objects)
			object->compositingDisabled();
	}
}

void CompositingAwareObject::triggerCompositingStateChanged()
{
	if (CompositingEnabled)
		compositingEnabled();
	else
		compositingDisabled();
}
