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

#include <QtCore/QString>

class QFacebookJsonReader;

class QFacebookContact
{
public:
	static QFacebookContact fromJson(const QFacebookJsonReader &json);

	explicit QFacebookContact(QByteArray id, QString name, QByteArray avatarUrl);

	QByteArray id() const { return m_id; }
	QString name() const { return m_name; }
	QByteArray avatarUrl() const { return m_avatarUrl; }

private:
	QByteArray m_id;
	QString m_name;
	QByteArray m_avatarUrl;

};
