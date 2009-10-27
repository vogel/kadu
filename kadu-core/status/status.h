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
	static QString name(const Status &status, bool fullName = true);

private:
	QString Type;
	QString Group;
	QString Description;

public:
	static Status null;

	explicit Status(const QString &type = QString::null, const QString &description = QString::null);
	Status(const Status &copyme);
	~Status();

	QString type() const { return Type; }
	void setType(const QString &type);

	QString group() const { return Group; }

	QString description() const { return Description; }
	void setDescription(const QString &description) { Description = description; }

	bool isDisconnected() const;

	bool operator < (const Status &compare) const;
	bool operator == (const Status &compare) const;
	bool operator != (const Status &compare) const;

};

Q_DECLARE_METATYPE(Status)

#endif // STATUS_H
