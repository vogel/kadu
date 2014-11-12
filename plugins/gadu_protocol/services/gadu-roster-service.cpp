/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gadu-roster-service.h"

#include "protocols/services/roster/roster-state.h"

#include <QtCore/QScopedArrayPointer>

GaduRosterService::GaduRosterService(Account account, const QVector<Contact> &contacts, QObject *parent) :
		RosterService{account, std::move(contacts), parent}
{
}

GaduRosterService::~GaduRosterService()
{
}

void GaduRosterService::prepareRoster()
{
	setState(RosterState::Initialized);
	emit rosterReady(true);
}

void GaduRosterService::executeTask(const RosterTask &)
{
}

#include "moc_gadu-roster-service.cpp"
