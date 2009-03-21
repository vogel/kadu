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

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "exports.h"

class KADUAPI Status
{
public:
	enum StatusType
	{
		Online,
		Busy,
		Invisible,
		Offline
	};
	static QString name(const Status &status, bool fullName = true);

private:
	StatusType Type;
	QString Description;

	static StatusType comparableType(StatusType type);

public:
	Status(StatusType type = Offline, QString description = QString::null);
	Status(const Status &copyme);

	StatusType type() const { return Type; }
	void setType(StatusType type) { Type = type; }

	QString description() const { return Description; }
	void setDescription(const QString &description) { Description = description; }

	bool isOnline() const { return Online == Type; }
	bool isBusy() const { return Busy == Type; }
	bool isInvisible() const { return Invisible == Type; }
	bool isOffline() const { return Offline == Type; }

	int compareTo(const Status& compare) const;
	bool operator < (const Status &compare) const;
	bool operator == (const Status &compare) const;

};

Q_DECLARE_METATYPE(Status)

#endif // STATUS_H
