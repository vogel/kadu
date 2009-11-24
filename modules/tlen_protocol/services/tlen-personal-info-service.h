/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_PERSONAL_INFO_SERVICE_H
#define TLEN_PERSONAL_INFO_SERVICE_H

#include <QDomNodeList>

#include "protocols/services/personal-info-service.h"

class TlenProtocol;
class tlen;

class TlenPersonalInfoService : public PersonalInfoService
{
	Q_OBJECT

	TlenProtocol *Protocol;
	tlen * client;

private slots:
	void handlePubdirReceived(QDomNodeList node);

public:
	TlenPersonalInfoService(TlenProtocol *protocol);

	virtual void fetchPersonalInfo();
	virtual void updatePersonalInfo(Buddy buddy);

};

#endif // TLEN_PERSONAL_INFO_SERVICE_H

