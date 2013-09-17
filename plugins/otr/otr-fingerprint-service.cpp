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

#include "contacts/contact.h"

#include "otr-context-converter.h"

#include "otr-fingerprint-service.h"

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

void OtrFingerprintService::setContactFingerprintTrust(const Contact &contact, OtrFingerprintService::Trust trust) const
{
	if (!ContextConverter)
		return;

	ConnContext *context = ContextConverter.data()->contactToContextConverter(contact);
	if (!context->active_fingerprint)
		return;

	otrl_context_set_trust(context->active_fingerprint, TrustVerified == trust ? "verified" : "");
}

OtrFingerprintService::Trust OtrFingerprintService::contactFingerprintTrust(const Contact &contact) const
{
	if (!ContextConverter)
		return TrustNotVerified;

	ConnContext *context = ContextConverter.data()->contactToContextConverter(contact);
	if (!context->active_fingerprint)
		return TrustNotVerified;

	return QLatin1String("verified") == context->active_fingerprint->trust ? TrustVerified : TrustNotVerified;
}
