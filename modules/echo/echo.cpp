/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "echo.h"

//#include "message_box.h"
#include "gadu.h"
#include "debug.h"

extern "C" int echo_init()
{
	echo=new Echo();
	QObject::connect(gadu,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		echo,SLOT(chatReceived(UinsList,const QString&,time_t)));
	return 0;
}

extern "C" void echo_close()
{
	QObject::disconnect(gadu,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		echo,SLOT(chatReceived(UinsList,const QString&,time_t)));
	delete echo;
}

Echo::Echo() : QObject(NULL, "echo")
{
//	MessageBox::msg(tr("Echo started"));
}

Echo::~Echo()
{
//	MessageBox::msg(tr("Echo stopped"));
}

void Echo::chatReceived(UinsList senders,const QString& msg,time_t time)
{
	kdebugf();
	if (msg.left(5)!="KADU ")
	{
		QString resp=QString("KADU ECHO: ")+msg;
		gadu->sendMessage(senders,unicode2cp(resp));
	}
	kdebugf2();
}

Echo* echo;
