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

#include "accounts/account.h"

#include "otr-policy.h"

#include "otr-policy-account-store.h"

void OtrPolicyAccountStore::storePolicyToAccount(const Account &account, const OtrPolicy &policy)
{
	account.addProperty("encryption_ng_otr:policy", policy.toString(), CustomProperties::Storable);
}

OtrPolicy OtrPolicyAccountStore::loadPolicyFromAccount(const Account &account)
{
	return OtrPolicy::fromString(account.property("encryption_ng_otr:policy", QVariant()).toString());
}
