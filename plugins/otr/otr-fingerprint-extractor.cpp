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

extern "C" {
#	include <libotr/privkey.h>
}

#include "accounts/account.h"
#include "contacts/contact.h"

#include "otr-context-converter.h"
#include "otr-user-state.h"

#include "otr-fingerprint-extractor.h"

OtrFingerprintExtractor::OtrFingerprintExtractor(QObject *parent) :
		QObject(parent), UserState(0)
{
}

OtrFingerprintExtractor::~OtrFingerprintExtractor()
{
}

void OtrFingerprintExtractor::setUserState(OtrUserState *userState)
{
	UserState = userState;
}

void OtrFingerprintExtractor::setContextConverter(OtrContextConverter *contextConverter)
{
	ContextConverter = contextConverter;
}

QString OtrFingerprintExtractor::extractAccountFingerprint(const Account &account) const
{
	if (!UserState)
		return QString();

	char fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN];
	char *result = otrl_privkey_fingerprint(UserState->userState(), fingerprint, qPrintable(account.id()), qPrintable(account.protocolName()));

	if (!result)
		return QString();

	fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN - 1] = 0;
	return QString(fingerprint);
}

QString OtrFingerprintExtractor::extractContactFingerprint(const Contact &contact) const
{
	if (!UserState || !ContextConverter)
		return QString();

	ConnContext *context = ContextConverter.data()->contactToContextConverter(contact);
	if (!context->active_fingerprint)
		return QString();

	char fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN];
	otrl_privkey_hash_to_human(fingerprint, context->active_fingerprint->fingerprint);

	fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN - 1] = 0;
	return QString(fingerprint);
}
