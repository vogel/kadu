/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ROSTER_SERVICE_H
#define ROSTER_SERVICE_H

#include <QtCore/QObject>

#include "contacts/contact.h"

#include "exports.h"

class KADUAPI RosterService : public QObject
{
	Q_OBJECT

public:
	RosterService(QObject *parent = 0) : QObject(parent) {}

	virtual void addContact(const Contact &contact) = 0;
	virtual void removeContact(const Contact &contact) = 0;
	virtual void askForAuthorization(const Contact &contact) = 0;
	virtual void sendAuthorization(const Contact &contact) = 0;
};

#endif // ROSTER_SERVICE_H
