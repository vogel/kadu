/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "otr-peer-identity-verification-state.h"

OtrPeerIdentityVerificationState::OtrPeerIdentityVerificationState(State state, int percentCompleted) :
		MyState(state), PercentCompleted(percentCompleted)
{
}

OtrPeerIdentityVerificationState::OtrPeerIdentityVerificationState(const OtrPeerIdentityVerificationState &copyMe)
{
	*this = copyMe;
}

OtrPeerIdentityVerificationState & OtrPeerIdentityVerificationState::operator = (const OtrPeerIdentityVerificationState &copyMe)
{
	MyState = copyMe.MyState;
	PercentCompleted = copyMe.PercentCompleted;

	return *this;
}

bool OtrPeerIdentityVerificationState::isFinished() const
{
	return (StateSucceeded == MyState) || (StateFailed == MyState);
}

OtrPeerIdentityVerificationState::State OtrPeerIdentityVerificationState::state() const
{
	return MyState;
}

int OtrPeerIdentityVerificationState::percentCompleted() const
{
	return PercentCompleted;
}
