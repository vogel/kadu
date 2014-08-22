/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_POLICY_H
#define OTR_POLICY_H

#include <QtCore/QList>
#include <QtCore/QString>

extern "C" {
#	include <libotr/proto.h>
}

class OtrPolicy
{
	static QList<OtrPolicy> Values;

	OtrlPolicy Policy;
	QString PolicyString;

	explicit OtrPolicy(OtrlPolicy otrPolicy, const QString &policyString);

public:
	static OtrPolicy PolicyUndefined;
	static OtrPolicy PolicyNever;
	static OtrPolicy PolicyManual;
	static OtrPolicy PolicyOpportunistic;
	static OtrPolicy PolicyAlways;

	static OtrPolicy fromPolicy(OtrlPolicy otrPolicy);
	static OtrPolicy fromString(const QString &policyString);

	OtrPolicy(const OtrPolicy &copyFrom);

	OtrPolicy & operator = (const OtrPolicy &copyFrom);
	bool operator == (const OtrPolicy &compareTo) const;

	OtrlPolicy toOtrPolicy() const;
	const QString & toString() const;

};

#endif // OTR_POLICY_H
