/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef SEARCH_SERVICE_H
#define SEARCH_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy-list.h"
#include "buddies/buddy-search-criteria.h"

#include "exports.h"

class KADUAPI SearchService : public QObject
{
	Q_OBJECT

public:
	explicit SearchService(QObject *parent) : QObject(parent) {}

	virtual void searchFirst(BuddySearchCriteria criteria) = 0;
	virtual void searchNext() = 0;
	virtual void stop() = 0;

signals:
	void newResults(BuddyList buddies);

};

#endif // SEARCH_SERVICE_H
