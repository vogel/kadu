/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "protocols/protocol.h"

class AccountShared;

class GaduAccountData final
{

public:
	explicit GaduAccountData(AccountShared *data);
	~GaduAccountData();

	UinType uin();

	bool receiveImagesDuringInvisibility() const;
	void setReceiveImagesDuringInvisibility(bool receiveImagesDuringInvisibility) const;

	bool chatImageSizeWarning() const;
	void setChatImageSizeWarning(bool chatImageSizeWarning) const;

	bool initialRosterImport() const;
	void setInitialRosterImport(bool initialRosterImport) const;

	bool sendTypingNotification() const;
	void setSendTypingNotification(bool sendTypingNotification) const;

	int userlistVersion() const;
	void setUserlistVersion(int userlistVersion) const;

	bool receiveSpam() const;
	void setReceiveSpam(bool receiveSpam) const;

private:
	AccountShared *m_data;

};
