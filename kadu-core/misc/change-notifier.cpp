/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "change-notifier.h"

ChangeNotifier::ChangeNotifier(QObject *parent) :
		QObject(parent), Blocked(0), Changed(false)
{
}

ChangeNotifier::~ChangeNotifier()
{
}

void ChangeNotifier::block()
{
	Blocked++;
}

void ChangeNotifier::unblock()
{
	Q_ASSERT(Blocked > 0);

	Blocked--;

	if (0 == Blocked && Changed)
	{
		emit changed();
		Changed = false;
	}
}

void ChangeNotifier::notify()
{
	if (0 == Blocked)
	{
		emit changed();
		return;
	}

	Changed = true;
}

void ChangeNotifier::forget()
{
	Changed = false;
}

#include "moc_change-notifier.cpp"
