/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "qfacebook/qfacebook-msgid.h"
#include "qfacebook/qfacebook-uid.h"

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <vector>

class QFacebookJsonReader
{
public:
	explicit QFacebookJsonReader(const QByteArray &content);

	QStringList keys() const { return m_object.keys(); }

	bool hasObject(const QString &name) const;
	QFacebookJsonReader readObject(const QString &name) const;

	std::vector<QFacebookJsonReader> readArray(const QString &name) const;
	bool readBool(const QString &name) const;

	bool hasInt(const QString &name) const;
	int readInt(const QString &name) const;

	long readLong(const QString &name) const;
	long long readLongLong(const QString &name) const;
	QString readString(const QString &name) const;
	QFacebookMsgId readMsgId(const QString &name) const;
	QFacebookUid readUid(const QString &name) const;

private:
	QJsonObject m_object;

	explicit QFacebookJsonReader(const QJsonObject &object);

};
