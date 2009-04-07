/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/windows/open-chat-with/open-chat-with-runner.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"

#include "misc/misc.h"
#include "xml_config_file.h"

#include "gadu-account.h"

GaduAccount::GaduAccount(const QUuid &uuid) :
	Account(uuid), AllowDcc(true), DccIP(QHostAddress()), DccExternalIP(QHostAddress()), DccIpDetect(true),
        DccPort(0), DccExternalPort(0), DccLocalPort(0), RemoveCompletedTransfers(0), DccForwarding(0)
{
	OpenChatRunner = new GaduOpenChatWithRunner(this);
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
}

GaduAccount::~GaduAccount()
{
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	delete OpenChatRunner;
	OpenChatRunner = 0;
}

bool GaduAccount::setId(const QString &id)
{
	if (!Account::setId(id))
		return false;

	Uin = id.toLong();
	return true;
}

void GaduAccount::loadConfiguration()
{
	if (!isValidStorage())
		return;

	Account::loadConfiguration();

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
}

void GaduAccount::store()
{
	if (!isValidStorage())
		return;

	Account::store();

	storeValue("AllowDcc", AllowDcc);
	storeValue("DccIP", DccIP.toString());
	storeValue("DccIpDetect", DccIpDetect);
	storeValue("DccPort", DccPort);
	storeValue("DccExternalIp", DccExternalIP.toString());
	storeValue("DccExternalPort", DccExternalPort);
	storeValue("DccLocalPort", DccLocalPort);
	storeValue("RemoveCompletedTransfers", RemoveCompletedTransfers);
	storeValue("DccForwarding", DccForwarding);
}

/*
bool GaduAccountData::validateId(const QString &id)
{
	bool ok;
	UinType tmpUin = id.toLong(&ok);
	return ok;
}*/
