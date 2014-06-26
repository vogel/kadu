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
#include "misc/kadu-paths.h"

#include "otr-context-converter.h"
#include "otr-op-data.h"
#include "otr-user-state-service.h"

#include "otr-fingerprint-service.h"

void OtrFingerprintService::wrapperOtrWriteFingerprints(void *data)
{
	OtrOpData *opData = static_cast<OtrOpData *>(data);
	if (opData->fingerprintService())
		opData->fingerprintService()->writeFingerprints();
}

OtrFingerprintService::OtrFingerprintService(QObject *parent) :
		QObject(parent)
{
}

OtrFingerprintService::~OtrFingerprintService()
{
}

void OtrFingerprintService::setContextConverter(OtrContextConverter *contextConverter)
{
	ContextConverter = contextConverter;
}

void OtrFingerprintService::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

QString OtrFingerprintService::fingerprintsStoreFileName() const
{
	return KaduPaths::instance()->profilePath() + QString("/keys/otr_fingerprints");
}

void OtrFingerprintService::readFingerprints() const
{
	if (!UserStateService)
		return;

	OtrlUserState userState = UserStateService->userState();
	otrl_privkey_read_fingerprints(userState, fingerprintsStoreFileName().toUtf8().data(), 0, 0);

	emit fingerprintsUpdated();
}

void OtrFingerprintService::writeFingerprints() const
{
	if (!UserStateService)
		return;

	OtrlUserState userState = UserStateService->userState();
	otrl_privkey_write_fingerprints(userState, fingerprintsStoreFileName().toUtf8().data());

	emit fingerprintsUpdated();
}

void OtrFingerprintService::setContactFingerprintTrust(const Contact &contact, OtrFingerprintService::Trust trust) const
{
	if (!ContextConverter)
		return;

	ConnContext *context = ContextConverter->contactToContextConverter(contact);
	if (!context->active_fingerprint)
		return;

	otrl_context_set_trust(context->active_fingerprint, TrustVerified == trust ? "verified" : "");
	writeFingerprints();
}

OtrFingerprintService::Trust OtrFingerprintService::contactFingerprintTrust(const Contact &contact) const
{
	if (!ContextConverter)
		return TrustNotVerified;

	ConnContext *context = ContextConverter->contactToContextConverter(contact);
	if (!context->active_fingerprint)
		return TrustNotVerified;

	return (context->active_fingerprint->trust && context->active_fingerprint->trust[0] != 0) ? TrustVerified : TrustNotVerified;
}

QString OtrFingerprintService::extractAccountFingerprint(const Account &account) const
{
	if (!UserStateService)
		return QString();

	char fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN];
	char *result = otrl_privkey_fingerprint(UserStateService->userState(), fingerprint,
											qPrintable(account.id()), qPrintable(account.protocolName()));

	if (!result)
		return QString();

	fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN - 1] = 0;
	return QString(fingerprint);
}

QString OtrFingerprintService::extractContactFingerprint(const Contact &contact) const
{
	if (!ContextConverter)
		return QString();

	ConnContext *context = ContextConverter->contactToContextConverter(contact);
	if (!context->active_fingerprint)
		return QString();

	char fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN];
	otrl_privkey_hash_to_human(fingerprint, context->active_fingerprint->fingerprint);

	fingerprint[OTRL_PRIVKEY_FPRINT_HUMAN_LEN - 1] = 0;
	return QString(fingerprint);
}
