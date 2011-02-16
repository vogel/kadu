/*
 * %kadu copyright begin%
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef STATUS_H
#define STATUS_H

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "exports.h"

class KADUAPI Status
{
	QString Type;
	QString Group;
	QString Description;
	QString DisplayName;

public:
	explicit Status(const QString &type = QString(), const QString &description = QString());
	Status(const Status &copyme);
	~Status();

	const QString & type() const { return Type; }
	void setType(const QString &type);

	const QString & group() const { return Group; }

	const QString & displayName() const { return DisplayName; };

	const QString & description() const { return Description; }
	void setDescription(const QString &description) { Description = description; }
	bool hasDescription();

	bool isDisconnected() const;

	bool operator < (const Status &compare) const;
	bool operator == (const Status &compare) const;
	bool operator != (const Status &compare) const;

};

Q_DECLARE_METATYPE(Status)

#endif // STATUS_H
