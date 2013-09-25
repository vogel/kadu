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

#include <QtCore/QTimer>

extern "C" {
#	include <gcrypt.h>
#	include <libotr/proto.h>
#	include <libotr/userstate.h>
#	include <libotr/message.h>
}

#include "otr-app-ops-wrapper.h"
#include "otr-user-state-service.h"

#include "otr-timer.h"

OtrTimer::OtrTimer(QObject *parent) :
		QObject(parent), Timer(0)
{

}

OtrTimer::~OtrTimer()
{

}

void OtrTimer::setOtrAppOpsWrapper(OtrAppOpsWrapper *otrAppOpsWrapper)
{
	AppOpsWrapper = otrAppOpsWrapper;
}

void OtrTimer::setUserStateService(OtrUserStateService *userStateService)
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

void OtrTimer::otrTimerTimeout()
{
	const OtrlMessageAppOps *ops = AppOpsWrapper
			? AppOpsWrapper.data()->ops()
			: 0;
	otrl_message_poll(UserStateService.data()->userState(), ops, 0);
}

void OtrTimer::timerControl(int intervalInSeconds)
{
	if (!Timer)
		return;

	if (intervalInSeconds)
		Timer->start(intervalInSeconds * 1000);
	else
		Timer->stop();
}
