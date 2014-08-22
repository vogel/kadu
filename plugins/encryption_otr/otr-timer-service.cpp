/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QTimer>

extern "C" {
#	include <gcrypt.h>
#	include <libotr/proto.h>
#	include <libotr/userstate.h>
#	include <libotr/message.h>
}

#include "otr-app-ops-service.h"
#include "otr-op-data-factory.h"
#include "otr-op-data.h"
#include "otr-user-state-service.h"

#include "otr-timer-service.h"

void OtrTimerService::wrapperOtrTimerControl(void *data, unsigned int interval)
{
	OtrOpData *opData = static_cast<OtrOpData *>(data);
	if (opData->timerService())
		opData->timerService()->timerControl(interval);
}

OtrTimerService::OtrTimerService(QObject *parent) :
		QObject(parent), Timer(0)
{
}

OtrTimerService::~OtrTimerService()
{
}

void OtrTimerService::setAppOpsService(OtrAppOpsService *appOpsService)
{
	AppOpsService = appOpsService;
}

void OtrTimerService::setOpDataFactory(OtrOpDataFactory *opDataFactory)
{
	OpDataFactory = opDataFactory;
}

void OtrTimerService::setUserStateService(OtrUserStateService *userStateService)
{
	if (UserStateService)
	{
		delete Timer;
		Timer = 0;
	}

	UserStateService = userStateService;

	if (UserStateService)
	{
		Timer = new QTimer(this);
		connect(Timer, SIGNAL(timeout()), this, SLOT(otrTimerTimeout()));
	}
}

void OtrTimerService::otrTimerTimeout()
{
	if (!AppOpsService || !OpDataFactory || !UserStateService)
		return;

	OtrlUserState userState = UserStateService->userState();
	const OtrlMessageAppOps *ops = AppOpsService->appOps();
	OtrOpData opData = OpDataFactory->opData();

	otrl_message_poll(userState, ops, &opData);
}

void OtrTimerService::timerControl(unsigned int intervalInSeconds)
{
	if (!Timer)
		return;

	if (intervalInSeconds)
		Timer->start(static_cast<int>(intervalInSeconds * 1000));
	else
		Timer->stop();
}
