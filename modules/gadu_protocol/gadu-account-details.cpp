/*
 * %kadu copyright begin%
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "gui/windows/open-chat-with/open-chat-with-runner.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"
#include "misc/misc.h"

#include "gadu-account-details.h"

GaduAccountDetails::GaduAccountDetails(AccountShared *data) :
		AccountDetails(data),
		AllowDcc(true), DccIpDetect(true),
		DccPort(0), DccExternalPort(0), DccLocalPort(0), RemoveCompletedTransfers(0), DccForwarding(0), InitialRosterImport(false)
{
	OpenChatRunner = new GaduOpenChatWithRunner(data);
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
}

GaduAccountDetails::~GaduAccountDetails()
{
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	delete OpenChatRunner;
	OpenChatRunner = 0;
}

void GaduAccountDetails::load()
{
	if (!isValidStorage())
		return;

	AccountDetails::load();

	AllowDcc = loadValue<bool>("AllowDcc");
	DccIpDetect = loadValue<bool>("DccIpDetect");
	DccPort = loadValue<int>("DccPort");
	DccExternalPort = loadValue<int>("DccExternalPort");
	DccLocalPort = loadValue<int>("DccLocalPort");
	RemoveCompletedTransfers = loadValue<bool>("RemoveCompletedTransfers");
	DccForwarding = loadValue<bool>("DccForwarding");
	MaximumImageSize = loadValue<short int>("MaximumImageSize", 255);
	ReceiveImagesDuringInvisibility = loadValue<bool>("ReceiveImagesDuringInvisibility", true);
	MaximumImageRequests = loadValue<short int>("MaximumImageRequests", 10);
	InitialRosterImport = loadValue<bool>("InitialRosterImport", false);
	

	QHostAddress host;
	if (!host.setAddress(loadValue<QString>("DccExternalIp")))
		host.setAddress("0.0.0.0");
	DccExternalIP = host;
	if (!host.setAddress(loadValue<QString>("DccIP")))
		host.setAddress("0.0.0.0");
	DccIP = host;
}

void GaduAccountDetails::store()
{
	if (!isValidStorage())
		return;

	storeValue("AllowDcc", AllowDcc);
	storeValue("DccIP", DccIP.toString());
	storeValue("DccIpDetect", DccIpDetect);
	storeValue("DccPort", DccPort);
	storeValue("DccExternalIp", DccExternalIP.toString());
	storeValue("DccExternalPort", DccExternalPort);
	storeValue("DccLocalPort", DccLocalPort);
	storeValue("RemoveCompletedTransfers", RemoveCompletedTransfers);
	storeValue("DccForwarding", DccForwarding);
	storeValue("MaximumImageSize", MaximumImageSize);
	storeValue("ReceiveImagesDuringInvisibility", ReceiveImagesDuringInvisibility);
	storeValue("MaximumImageRequests", MaximumImageRequests);
	storeValue("InitialRosterImport", InitialRosterImport);
	
}

void GaduAccountDetails::import_0_6_5_LastStatus()
{
	if (!isValidStorage())
		return;

	QString name;

	int typeIndex = config_file.readNumEntry("General", "LastStatusType", -1);
	switch (typeIndex)
	{
		case 0: name = "Online"; break;
		case 1: name = "Away"; break;
		case 2: name = "Invisible"; break;
		default: name = "Offline"; break;
	}

	storeValue("LastStatusName", name);
	storeValue("LastStatusDescription", config_file.readEntry("General", "LastStatusDescription"));
}

UinType GaduAccountDetails::uin()
{
	return mainData()->id().toULong();
}
