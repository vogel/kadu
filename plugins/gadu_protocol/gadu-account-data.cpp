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

#include "gadu-account-data.h"

#include "accounts/account-shared.h"

GaduAccountData::GaduAccountData(AccountShared *data) :
		m_data{data}
{
}

GaduAccountData::~GaduAccountData()
{
}

bool GaduAccountData::receiveImagesDuringInvisibility() const
{
	return m_data->loadValue<bool>("ReceiveImagesDuringInvisibility", true);
}

void GaduAccountData::setReceiveImagesDuringInvisibility(bool receiveImagesDuringInvisibility) const
{
	if (m_data->isValidStorage())
		m_data->storeValue("ReceiveImagesDuringInvisibility", receiveImagesDuringInvisibility);
}

bool GaduAccountData::chatImageSizeWarning() const
{
	return m_data->loadValue<bool>("ChatImageSizeWarning", true);
}

void GaduAccountData::setChatImageSizeWarning(bool chatImageSizeWarning) const
{
	if (m_data->isValidStorage())
		m_data->storeValue("ChatImageSizeWarning", chatImageSizeWarning);
}

bool GaduAccountData::initialRosterImport() const
{
	return m_data->loadValue<bool>("InitialRosterImport", true);
}

void GaduAccountData::setInitialRosterImport(bool initialRosterImport) const
{
	if (m_data->isValidStorage())
		m_data->storeValue("InitialRosterImport", initialRosterImport);
}

bool GaduAccountData::sendTypingNotification() const
{
	return m_data->loadValue<bool>("SendTypingNotification", true);
}

void GaduAccountData::setSendTypingNotification(bool sendTypingNotification) const
{
	if (m_data->isValidStorage())
		m_data->storeValue("SendTypingNotification", sendTypingNotification);
}

int GaduAccountData::userlistVersion() const
{
	return m_data->loadValue<int>("UserlistVersion", -1);
}

void GaduAccountData::setUserlistVersion(int userlistVersion) const
{
	if (m_data->isValidStorage())
		m_data->storeValue("UserlistVersion", userlistVersion);
}

bool GaduAccountData::receiveSpam() const
{
	return m_data->loadValue<bool>("ReceiveSpam", true);
}

void GaduAccountData::setReceiveSpam(bool receiveSpam) const
{
	if (m_data->isValidStorage())
		m_data->storeValue("ReceiveSpam", receiveSpam);
}

UinType GaduAccountData::uin()
{
	return m_data->id().toULong();
}
