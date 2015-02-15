/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

bool CompositingAwareObject::isCompositingEnabled() const
{
	return CompositingEnabled;
}
