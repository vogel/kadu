/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
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

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"
#include "gui/windows/open-chat-with/open-chat-with-runner.h"
#include "misc/misc.h"

#include "gadu-account-details.h"

GaduAccountDetails::GaduAccountDetails(AccountShared *data) :
		AccountDetails(data), AllowDcc(true), ReceiveImagesDuringInvisibility(true),
		ChatImageSizeWarning(true), InitialRosterImport(true), TlsEncryption(false),
		SendTypingNotification(true), ExternalPort(0), UserlistVersion(-1), ReceiveSpam(true)
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
	ReceiveImagesDuringInvisibility = loadValue<bool>("ReceiveImagesDuringInvisibility", true);
	ChatImageSizeWarning = loadValue<bool>("ChatImageSizeWarning", true);
	InitialRosterImport = loadValue<bool>("InitialRosterImport", true);
	TlsEncryption = loadValue<bool>("TlsEncryption", false);
	SendTypingNotification = loadValue<bool>("SendTypingNotification", true);
	ExternalIp = loadValue<QString>("ExternalIp");
	ExternalPort = loadValue<unsigned int>("ExternalPort", 0);
	UserlistVersion = loadValue<int>("UserlistVersion", -1);
	ReceiveSpam = loadValue<bool>("ReceiveSpam", true);
}

void GaduAccountDetails::store()
{
	if (!isValidStorage())
		return;

	storeValue("AllowDcc", AllowDcc);
	storeValue("ReceiveImagesDuringInvisibility", ReceiveImagesDuringInvisibility);
	storeValue("ChatImageSizeWarning", ChatImageSizeWarning);
	storeValue("InitialRosterImport", InitialRosterImport);
	storeValue("TlsEncryption", TlsEncryption);
	storeValue("SendTypingNotification", SendTypingNotification);
	storeValue("ExternalIp", ExternalIp);
	storeValue("ExternalPort", ExternalPort);
	storeValue("UserlistVersion", UserlistVersion);
	storeValue("ReceiveSpam", ReceiveSpam);
}

UinType GaduAccountDetails::uin()
{
	return mainData()->id().toULong();
}
