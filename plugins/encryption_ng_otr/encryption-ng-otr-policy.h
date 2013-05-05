/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_OTR_POLICY_H
#define ENCRYPTION_NG_OTR_POLICY_H

#include <QtCore/QList>
#include <QtCore/QString>

extern "C" {
#	include <libotr/proto.h>
}

class EncryptionNgOtrPolicy
{
	static QList<EncryptionNgOtrPolicy> Values;

	OtrlPolicy Policy;
	QString PolicyString;

	explicit EncryptionNgOtrPolicy(OtrlPolicy otrPolicy, const QString &policyString);

public:
	static EncryptionNgOtrPolicy fromPolicy(OtrlPolicy otrPolicy);
	static EncryptionNgOtrPolicy fromString(const QString &policyString);

	EncryptionNgOtrPolicy(const EncryptionNgOtrPolicy &copyFrom);

	EncryptionNgOtrPolicy & operator = (const EncryptionNgOtrPolicy &copyFrom);
	bool operator == (const EncryptionNgOtrPolicy &compareTo) const;

	OtrlPolicy toOtrPolicy() const;
	const QString & toString() const;

};

#endif // ENCRYPTION_NG_OTR_POLICY_H
