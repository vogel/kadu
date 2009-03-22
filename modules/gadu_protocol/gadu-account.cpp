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

void GaduAccount::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Account::loadConfiguration(configurationStorage, parent);

	AllowDcc = QVariant(configurationStorage->getTextNode(parent, "AllowDcc")).toBool();
	DccIpDetect = QVariant(configurationStorage->getTextNode(parent, "DccIpDetect")).toBool();
	DccPort = QVariant(configurationStorage->getTextNode(parent, "DccPort")).toInt();
	DccExternalPort = QVariant(configurationStorage->getTextNode(parent, "DccExternalPort")).toInt();
	DccLocalPort = QVariant(configurationStorage->getTextNode(parent, "DccLocalPort")).toInt();
	RemoveCompletedTransfers = QVariant(configurationStorage->getTextNode(parent, "RemoveCompletedTransfers")).toBool();
	DccForwarding = QVariant(configurationStorage->getTextNode(parent, "DccForwarding")).toBool();

	QHostAddress host;
	if (!host.setAddress(configurationStorage->getTextNode(parent, "DccExternalIp")))
		host.setAddress("0.0.0.0");
	DccExternalIP = host;
	if (!host.setAddress(configurationStorage->getTextNode(parent, "DccIP")))
		host.setAddress("0.0.0.0");
	DccIP = host;
}

void GaduAccount::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Account::storeConfiguration(configurationStorage, parent);

	configurationStorage->createTextNode(parent, "AllowDcc", QVariant(AllowDcc).toString());
	configurationStorage->createTextNode(parent, "DccIP", DccIP.toString());
	configurationStorage->createTextNode(parent, "DccIpDetect", QVariant(DccIpDetect).toString());
	configurationStorage->createTextNode(parent, "DccPort", QVariant(DccPort).toString());
	configurationStorage->createTextNode(parent, "DccExternalIp", DccExternalIP.toString());
	configurationStorage->createTextNode(parent, "DccExternalPort", QVariant(DccExternalPort).toString());
	configurationStorage->createTextNode(parent, "DccLocalPort", QVariant(DccLocalPort).toString());
	configurationStorage->createTextNode(parent, "RemoveCompletedTransfers", QVariant(RemoveCompletedTransfers).toString());
	configurationStorage->createTextNode(parent, "DccForwarding", QVariant(DccForwarding).toString());
}

/*
bool GaduAccountData::validateId(const QString &id)
{
	bool ok;
	UinType tmpUin = id.toLong(&ok);
	return ok;
}*/
