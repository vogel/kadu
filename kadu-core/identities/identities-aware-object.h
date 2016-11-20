/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "exports.h"

#include <QtCore/QList>

class IdentityManager;
class Identity;

class KADUAPI IdentitiesAwareObject
{

protected:
	virtual void identityAdded(Identity identity) = 0;
	virtual void identityRemoved(Identity identity) = 0;

public:
	static void notifyIdentityAdded(Identity identity);
	static void notifyIdentityRemoved(Identity identity);

	IdentitiesAwareObject();
	virtual ~IdentitiesAwareObject();

	void triggerAllIdentitiesAdded(IdentityManager *identityManager);
	void triggerAllIdentitiesRemoved(IdentityManager *identityManager);

private:
	static QList<IdentitiesAwareObject *> Objects;

};
