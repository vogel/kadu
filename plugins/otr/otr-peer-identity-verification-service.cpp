/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "contacts/contact.h"

#include "otr-app-ops-service.h"
#include "otr-context-converter.h"
#include "otr-op-data-factory.h"
#include "otr-op-data.h"
#include "otr-peer-identity-verification-state.h"
#include "otr-user-state-service.h"

#include "otr-peer-identity-verification-service.h"

void OtrPeerIdentityVerificationService::wrapperHandleSmpEvent(void *data, OtrlSMPEvent smpEvent, ConnContext *context,
															   unsigned short progressPercent, char *question)
{
	Q_UNUSED(context);

	OtrOpData *opData = static_cast<OtrOpData *>(data);
	if (opData->peerIdentityVerificationService())
		opData->peerIdentityVerificationService()->handleSmpEvent(opData->contact(), smpEvent, progressPercent, QString::fromUtf8(question));
}

OtrPeerIdentityVerificationService::OtrPeerIdentityVerificationService(QObject *parent) :
		QObject(parent)
{
}

OtrPeerIdentityVerificationService::~OtrPeerIdentityVerificationService()
{
}

void OtrPeerIdentityVerificationService::setAppOpsService(OtrAppOpsService *appOpsService)
{
	AppOpsService = appOpsService;
}

void OtrPeerIdentityVerificationService::setContextConverter(OtrContextConverter *contextConverter)
{
	ContextConverter = contextConverter;
}

void OtrPeerIdentityVerificationService::setOpDataFactory(OtrOpDataFactory *opDataFactory)
{
	OpDataFactory = opDataFactory;
}

void OtrPeerIdentityVerificationService::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

void OtrPeerIdentityVerificationService::updateContactState(const Contact &contact, const OtrPeerIdentityVerificationState &state)
{
	emit contactStateUpdated(contact, state);
}

void OtrPeerIdentityVerificationService::startQuestionAndAnswerVerification(const Contact &contact, const QString &question, const QString &answer)
{
	if (!ContextConverter || !OpDataFactory || !UserStateService || !contact || question.isEmpty() || answer.isEmpty())
		return;

	OtrlUserState userState = UserStateService.data()->userState();
	const OtrlMessageAppOps *appOps = AppOpsService.data()->appOps();
	OtrOpData opData = OpDataFactory.data()->opDataForContact(contact);
	ConnContext *context = ContextConverter.data()->contactToContextConverter(contact);

	otrl_message_initiate_smp_q(userState, appOps, &opData, context, qPrintable(question), (const unsigned char *) qPrintable(answer),
								static_cast<size_t>(answer.length()));
}

void OtrPeerIdentityVerificationService::startSharedSecretVerficiation(const Contact &contact, const QString &sharedSecret)
{
	if (!ContextConverter || !OpDataFactory || !UserStateService || !contact || sharedSecret.isEmpty())
		return;

	OtrlUserState userState = UserStateService.data()->userState();
	const OtrlMessageAppOps *appOps = AppOpsService.data()->appOps();
	OtrOpData opData = OpDataFactory.data()->opDataForContact(contact);
	ConnContext *context = ContextConverter.data()->contactToContextConverter(contact);

	otrl_message_initiate_smp(userState, appOps, &opData, context, (const unsigned char *) qPrintable(sharedSecret),
							  static_cast<size_t>(sharedSecret.length()));
}

void OtrPeerIdentityVerificationService::respondVerification(const Contact &contact, const QString &answer)
{
	if (!ContextConverter || !OpDataFactory || !UserStateService || !contact || answer.isEmpty())
		return;

	OtrlUserState userState = UserStateService.data()->userState();
	const OtrlMessageAppOps *appOps = AppOpsService.data()->appOps();
	OtrOpData opData = OpDataFactory.data()->opDataForContact(contact);
	ConnContext *context = ContextConverter.data()->contactToContextConverter(contact);

	otrl_message_respond_smp(userState, appOps, &opData, context, (const unsigned char *) qPrintable(answer),
							 static_cast<size_t>(answer.length()));
}

void OtrPeerIdentityVerificationService::cancelVerification(const Contact &contact)
{
	if (!ContextConverter || !OpDataFactory || !UserStateService || !contact)
		return;

	OtrlUserState userState = UserStateService.data()->userState();
	const OtrlMessageAppOps *appOps = AppOpsService.data()->appOps();
	OtrOpData opData = OpDataFactory.data()->opDataForContact(contact);
	ConnContext *context = ContextConverter.data()->contactToContextConverter(contact);

	otrl_message_abort_smp(userState, appOps, &opData, context);
}

void OtrPeerIdentityVerificationService::handleSmpEvent(const Contact &contact, OtrlSMPEvent smpEvent, int progressPercent, const QString &question)
{
	Q_UNUSED(question);

	if (!contact)
		return;

	OtrPeerIdentityVerificationState::State state = OtrPeerIdentityVerificationState::StateNotStarted;
	switch (smpEvent)
	{
		case OTRL_SMPEVENT_IN_PROGRESS:
			state = OtrPeerIdentityVerificationState::StateInProgress;
			break;
		case OTRL_SMPEVENT_ASK_FOR_ANSWER:
			state = OtrPeerIdentityVerificationState::StateInProgress;
			emit questionAnswerRequested(contact, question);
			break;
		case OTRL_SMPEVENT_ASK_FOR_SECRET:
			state = OtrPeerIdentityVerificationState::StateInProgress;
			emit sharedSecretRequested(contact);
			break;
		case OTRL_SMPEVENT_CHEATED:
		case OTRL_SMPEVENT_ERROR:
		case OTRL_SMPEVENT_FAILURE:
			state = OtrPeerIdentityVerificationState::StateFailed;
			break;
		case OTRL_SMPEVENT_SUCCESS:
			state = OtrPeerIdentityVerificationState::StateSucceeded;
			break;
		default:
			state = OtrPeerIdentityVerificationState::StateNotStarted;
			break;
	}

	updateContactState(contact, OtrPeerIdentityVerificationState(state, progressPercent));
}
