/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
		AccountDetails(data), AllowDcc(true), LimitImageSize(true), MaximumImageSize(255),
		ImageSizeAsk(false), ReceiveImagesDuringInvisibility(true), MaximumImageRequests(10),
		ChatImageSizeWarning(true), InitialRosterImport(true), TlsEncryption(false),
		SendTypingNotification(true), ExternalPort(0), UserlistVersion(-1)
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

	AllowDcc = loadValue<bool>("AllowDcc", true);
	LimitImageSize = loadValue<bool>("LimitImageSize", true);
	MaximumImageSize = loadValue<int>("MaximumImageSize", 255);
	ImageSizeAsk = loadValue<bool>("ImageSizeAsk", false);
	ReceiveImagesDuringInvisibility = loadValue<bool>("ReceiveImagesDuringInvisibility", true);
	MaximumImageRequests = loadValue<short int>("MaximumImageRequests", 10);
	ChatImageSizeWarning = loadValue<bool>("ChatImageSizeWarning", true);
	InitialRosterImport = loadValue<bool>("InitialRosterImport", true);
	TlsEncryption = loadValue<bool>("TlsEncryption", false);
	SendTypingNotification = loadValue<bool>("SendTypingNotification", true);
	ExternalIp = loadValue<QString>("ExternalIp");
	ExternalPort = loadValue<unsigned int>("ExternalPort", 0);
	UserlistVersion = loadValue<int>("UserlistVersion", -1);
}

void GaduAccountDetails::store()
{
	if (!isValidStorage())
		return;

	storeValue("AllowDcc", AllowDcc);
	storeValue("LimitImageSize", LimitImageSize);
	storeValue("MaximumImageSize", MaximumImageSize);
	storeValue("ImageSizeAsk", ImageSizeAsk);
	storeValue("ReceiveImagesDuringInvisibility", ReceiveImagesDuringInvisibility);
	storeValue("MaximumImageRequests", MaximumImageRequests);
	storeValue("ChatImageSizeWarning", ChatImageSizeWarning);
	storeValue("InitialRosterImport", InitialRosterImport);
	storeValue("TlsEncryption", TlsEncryption);
	storeValue("SendTypingNotification", SendTypingNotification);
	storeValue("ExternalIp", ExternalIp);
	storeValue("ExternalPort", ExternalPort);
	storeValue("UserlistVersion", UserlistVersion);
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
