/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef COMPOSITING_AWARE_OBJECT
#define COMPOSITING_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class KADUAPI CompositingAwareObject : public AwareObject<CompositingAwareObject>
{
	static KADUAPI bool CompositingEnabled;
protected:
	virtual void compositingEnabled() = 0;
	virtual void compositingDisabled() = 0;

public:
	static KADUAPI void compositingStateChanged();

	void triggerCompositingStateChanged();

};

#endif // COMPOSITING_AWARE_OBJECT
