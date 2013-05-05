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

#include "encryption-ng-otr-policy.h"

EncryptionNgOtrPolicy EncryptionNgOtrPolicy::NEVER(OTRL_POLICY_NEVER, "never");
EncryptionNgOtrPolicy EncryptionNgOtrPolicy::MANUAL(OTRL_POLICY_MANUAL, "manual");
EncryptionNgOtrPolicy EncryptionNgOtrPolicy::OPPORTUNISTIC(OTRL_POLICY_OPPORTUNISTIC, "opportunistic");
EncryptionNgOtrPolicy EncryptionNgOtrPolicy::ALWAYS(OTRL_POLICY_ALWAYS, "always");

QList<EncryptionNgOtrPolicy> EncryptionNgOtrPolicy::Values = QList<EncryptionNgOtrPolicy>()
		<< EncryptionNgOtrPolicy::MANUAL
		<< EncryptionNgOtrPolicy::OPPORTUNISTIC
		<< EncryptionNgOtrPolicy::ALWAYS
		<< EncryptionNgOtrPolicy::NEVER;

EncryptionNgOtrPolicy EncryptionNgOtrPolicy::fromPolicy(OtrlPolicy otrPolicy)
{
	foreach (const EncryptionNgOtrPolicy &policy, Values)
		if (otrPolicy == policy.toOtrPolicy())
			return policy;

	return Values.at(0);
}

EncryptionNgOtrPolicy EncryptionNgOtrPolicy::fromString(const QString &policyString)
{
	foreach (const EncryptionNgOtrPolicy &policy, Values)
		if (policyString == policy.toString())
			return policy;

	return Values.at(0);
}

EncryptionNgOtrPolicy::EncryptionNgOtrPolicy(OtrlPolicy otrPolicy, const QString &policyString)
		: Policy(otrPolicy), PolicyString(policyString)
{
}

EncryptionNgOtrPolicy::EncryptionNgOtrPolicy(const EncryptionNgOtrPolicy &copyFrom)
{
	*this = copyFrom;
}

EncryptionNgOtrPolicy & EncryptionNgOtrPolicy::operator=(const EncryptionNgOtrPolicy &copyFrom)
{
	Policy = copyFrom.Policy;
	PolicyString = copyFrom.PolicyString;

	return *this;
}

bool EncryptionNgOtrPolicy::operator == (const EncryptionNgOtrPolicy &compareTo) const
{
	return Policy == compareTo.Policy;
}

OtrlPolicy EncryptionNgOtrPolicy::toOtrPolicy() const
{
	return Policy;
}

const QString& EncryptionNgOtrPolicy::toString() const
{
	return PolicyString;
}
