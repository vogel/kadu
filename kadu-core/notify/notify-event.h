/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFY_EVENT_H
#define NOTIFY_EVENT_H

#include <QtCore/QString>

class NotifyEvent
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
	CallbackRequirement IsCallbackRequired;
	const char *Description;

public:
	NotifyEvent(const QString &name, CallbackRequirement isCallbackRequired, const char *description);
	bool operator == (const NotifyEvent &compare);

	QString name() { return Name; }
	CallbackRequirement isCallbackRequired() { return IsCallbackRequired; }
	const char * description() { return Description; }

};

#endif // NOTIFY_EVENT_H
