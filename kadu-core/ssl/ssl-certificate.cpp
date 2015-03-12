/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "ssl-certificate.h"

#include <QtCore/QHash>

SslCertificate::SslCertificate(QString hostName, QString pemHexEncodedCertificate) :
		m_hostName{std::move(hostName)},
		m_pemHexEncodedCertificate{std::move(pemHexEncodedCertificate)}
{
}

QString SslCertificate::hostName() const
{
	return m_hostName;
}

QString SslCertificate::pemHexEncodedCertificate() const
{
	return m_pemHexEncodedCertificate;
}

bool operator == (const SslCertificate &x, const SslCertificate &y)
{
	if (x.hostName() != y.hostName())
		return false;
	if (x.pemHexEncodedCertificate() != y.pemHexEncodedCertificate())
		return false;
	return true;
}

uint qHash(const SslCertificate &key, uint seed)
{
	auto result = 0xcbf29ce484222325ULL;
	result = (result ^ qHash(key.hostName(), seed)) * 0x100000001b3ULL;
	result = (result ^ qHash(key.pemHexEncodedCertificate(), seed)) * 0x100000001b3ULL;
	return result;
}
