/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"

#include "misc.h"
#include "xml_config_file.h"

#include "gadu-account.h"

GaduAccount::GaduAccount(const QUuid &uuid) :
	Account(uuid)
{
// 	OpenChatRunner = new GaduOpenChatWithRunner();
// 	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
}

bool GaduAccount::setId(const QString &id)
{
	if (!Account::setId(id))
		return false;

	Uin = id.toLong();
	return true;
}
/*
bool GaduAccountData::validateId(const QString &id)
{
	bool ok;
	UinType tmpUin = id.toLong(&ok);
	return ok;
}*/
