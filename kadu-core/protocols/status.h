/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_H
#define STATUS_H

#include <QtCore/QString>

class Status
{
public:
	enum StatusType
	{
		Online,
		Busy,
		Invisible,
		Offline
	};

private:
	StatusType Type;
	QString Description;

public:
	Status(StatusType type = Offline, QString description = QString::null);
	Status(const Status &copyme);

	StatusType type() { return Type; }
	void setType(StatusType type) { Type = type; }

	QString description() { return Description; }
	void setDescirption(const QString &description) { Description = description; }

	bool isOnline() { return Online == Type; }
	bool isBusy() { return Busy == Type; }
	bool isInvisible() { return Invisible == Type; }
	bool isOffline() { return Offline == Type; }

};

#endif // STATUS_H
