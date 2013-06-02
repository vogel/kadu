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

#include "encryption-ng-otr-app-ops-wrapper.h"
#include "encryption-ng-otr-user-state.h"

#include "encryption-ng-otr-timer.h"

EncryptionNgOtrTimer::EncryptionNgOtrTimer(QObject *parent) :
		QObject(parent), OtrTimer(0)
{

}

EncryptionNgOtrTimer::~EncryptionNgOtrTimer()
{

}

void EncryptionNgOtrTimer::setEncryptionNgOtrAppOpsWrapper(EncryptionNgOtrAppOpsWrapper *otrAppOpsWrapper)
{
	OtrAppOpsWrapper = otrAppOpsWrapper;
}

void EncryptionNgOtrTimer::setUserState(EncryptionNgOtrUserState *userState)
{
	if (UserState)
	{
		delete OtrTimer;
		OtrTimer = 0;
	}

	UserState = userState;

	if (UserState)
	{
		OtrTimer = new QTimer(this);
		connect(OtrTimer, SIGNAL(timeout()), this, SLOT(otrTimerTimeout()));
	}
}

void EncryptionNgOtrTimer::otrTimerTimeout()
{
	const OtrlMessageAppOps *ops = OtrAppOpsWrapper
			? OtrAppOpsWrapper.data()->ops()
			: 0;
	otrl_message_poll(UserState->userState(), ops, 0);
}

void EncryptionNgOtrTimer::timerControl(int intervalInSeconds)
{
	if (!OtrTimer)
		return;

	if (intervalInSeconds)
		OtrTimer->start(intervalInSeconds * 1000);
	else
		OtrTimer->stop();
}
