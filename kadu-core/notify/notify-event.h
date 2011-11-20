/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef NOTIFY_EVENT_H
#define NOTIFY_EVENT_H

#include <QtCore/QString>
#include "exports.h"

class KADUAPI NotifyEvent
{
public:
	/**
		@enum CallbackRequirement
		Okre�la, czy dane zdarzenie wymaga podj�cia od u�ytkownika akcji innej ni� domy�lne zaakceptowanie/odrzucenie.
	 **/
	enum CallbackRequirement {
		CallbackRequired,
		CallbackNotRequired
	};

private:
	QString Name;
	QString Category;

	CallbackRequirement IsCallbackRequired;
	const char *Description;

public:
	NotifyEvent(const QString &name, CallbackRequirement isCallbackRequired, const char *description);
	bool operator == (const NotifyEvent &compare);

	const QString & name() const { return Name; }
	const QString & category() const { return Category; }
	CallbackRequirement isCallbackRequired() { return IsCallbackRequired; }
	const char * description() { return Description; }

};

#endif // NOTIFY_EVENT_H
