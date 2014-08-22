/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "otr-policy.h"

OtrPolicy OtrPolicy::PolicyUndefined(OTRL_POLICY_MANUAL, "undefined");
OtrPolicy OtrPolicy::PolicyNever(OTRL_POLICY_NEVER, "never");
OtrPolicy OtrPolicy::PolicyManual(OTRL_POLICY_MANUAL, "manual");
OtrPolicy OtrPolicy::PolicyOpportunistic(OTRL_POLICY_OPPORTUNISTIC, "opportunistic");
OtrPolicy OtrPolicy::PolicyAlways(OTRL_POLICY_ALWAYS, "always");

QList<OtrPolicy> OtrPolicy::Values = QList<OtrPolicy>()
		<< OtrPolicy::PolicyUndefined
		<< OtrPolicy::PolicyManual
		<< OtrPolicy::PolicyOpportunistic
		<< OtrPolicy::PolicyAlways
		<< OtrPolicy::PolicyNever;

OtrPolicy OtrPolicy::fromPolicy(OtrlPolicy otrPolicy)
{
	foreach (const OtrPolicy &policy, Values)
		if (otrPolicy == policy.toOtrPolicy())
			return policy;

	return Values.at(0);
}

OtrPolicy OtrPolicy::fromString(const QString &policyString)
{
	foreach (const OtrPolicy &policy, Values)
		if (policyString == policy.toString())
			return policy;

	return Values.at(0);
}

OtrPolicy::OtrPolicy(OtrlPolicy otrPolicy, const QString &policyString)
		: Policy(otrPolicy), PolicyString(policyString)
{
}

OtrPolicy::OtrPolicy(const OtrPolicy &copyFrom)
{
	*this = copyFrom;
}

OtrPolicy & OtrPolicy::operator = (const OtrPolicy &copyFrom)
{
	Policy = copyFrom.Policy;
	PolicyString = copyFrom.PolicyString;

	return *this;
}

bool OtrPolicy::operator == (const OtrPolicy &compareTo) const
{
	return PolicyString == compareTo.PolicyString;
}

OtrlPolicy OtrPolicy::toOtrPolicy() const
{
	return Policy;
}

const QString& OtrPolicy::toString() const
{
	return PolicyString;
}
