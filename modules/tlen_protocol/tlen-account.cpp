/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"
#include "configuration/xml-configuration-file.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"

#include "tlen-protocol.h"
#include "tlen-account.h"

TlenAccount::TlenAccount(const QUuid &uuid)
	: Account(uuid)
{
	OpenChatRunner = new TlenOpenChatWithRunner(this);
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
}

TlenAccount::~TlenAccount()
{
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	delete OpenChatRunner;
	OpenChatRunner = 0;
}

bool TlenAccount::setId(const QString &id)
{
    	return Account::setId(id);
}

void TlenAccount::load()
{
	if (!isValidStorage())
		return;

	Account::load();

	/*
	AllowDcc = loadValue<bool>("AllowDcC");
	DccIpDetect = loadValue<bool>("DccIpDetect");
	DccPort = loadValue<int>("DccPort");
	DccExternalPort = loadValue<int>("DccExternalPort");
	DccLocalPort = loadValue<int>("DccLocalPort");
	RemoveCompletedTransfers = loadValue<bool>("RemoveCompletedTransfers");
	DccForwarding = loadValue<bool>("DccForwarding");

	QHostAddress host;
	if (!host.setAddress(loadValue<QString>("DccExternalIp")))
		host.setAddress("0.0.0.0");
	DccExternalIP = host;
	if (!host.setAddress(loadValue<QString>("DccIP")))
		host.setAddress("0.0.0.0");
	DccIP = host;
	*/
}

void TlenAccount::store()
{
	if (!isValidStorage())
		return;

	Account::store();

	/*
	storeValue("AllowDcc", AllowDcc);
	storeValue("DccIP", DccIP.toString());
	storeValue("DccIpDetect", DccIpDetect);
	storeValue("DccPort", DccPort);
	storeValue("DccExternalIp", DccExternalIP.toString());
	storeValue("DccExternalPort", DccExternalPort);
	storeValue("DccLocalPort", DccLocalPort);
	storeValue("RemoveCompletedTransfers", RemoveCompletedTransfers);
	storeValue("DccForwarding", DccForwarding);
	*/
}

/*
bool TlenAccountData::validateId(const QString &id)
{
	// TODO ascii?
	return true;
}
*/
