/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "autoresponder.h"

#include "events.h"
#include "message_box.h"
#include "gadu.h"
#include "status.h"

extern "C" void init_module()
{
	autoresponder=new AutoResponder();
	QObject::connect(&event_manager,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		autoresponder,SLOT(chatReceived(UinsList,const QString&,time_t)));
}

AutoResponder::AutoResponder() : QObject()
{
	MessageBox::msg("Autoresponder");
}

void AutoResponder::chatReceived(UinsList senders,const QString& msg,time_t time)
{
	int status=getActualStatus();
	if(status==GG_STATUS_BUSY||status==GG_STATUS_BUSY_DESCR)
		gadu->sendMessage(senders,"KADU AUTORESPONDER: Thanks for your message. User is not currently available.");
}

AutoResponder* autoresponder;
