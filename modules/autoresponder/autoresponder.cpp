/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "autoresponder.h"
//#include "events.h"
#include "status.h"
#include "config_dialog.h"
#include "chat.h"
#include "debug.h"


extern "C" int autoresponder_init()
{
	autoresponder=new AutoResponder();
	return 0;
}

extern "C" void autoresponder_close()
{
	delete autoresponder;
	autoresponder=NULL;
}

AutoResponder::AutoResponder() : QObject(NULL, "autoresponder")
{	kdebugf();
	config=new ConfigFile(ggPath(QString("autoresponder.conf")));
	QObject::connect(chat_manager,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		this,SLOT(chatReceived(UinsList,const QString&,time_t)));
	QObject::connect(chat_manager, SIGNAL(chatCreated(const UinsList&)), this, SLOT(chatOpened(const UinsList&)));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Autoresponder"));
	ConfigDialog::addVGroupBox( "Autoresponder", "Autoresponder", QT_TRANSLATE_NOOP( "@default", "Autoresponder options" ));
	ConfigDialog::addLineEdit(config, "Autoresponder","Autoresponder options",
		QT_TRANSLATE_NOOP("@default", "Autoanswer text:"),
		"Autotext",
		QT_TRANSLATE_NOOP("@default", "Thanks for your message. User is not currently available."));
	ConfigDialog::addLabel("Autoresponder", "Autoresponder options", QT_TRANSLATE_NOOP( "@default","Choose status:" ));
	ConfigDialog::addCheckBox(config, "Autoresponder", "Autoresponder options", QT_TRANSLATE_NOOP( "@default","Status invisible" ),"StatusInvisible",false );
	ConfigDialog::addCheckBox(config, "Autoresponder", "Autoresponder options", QT_TRANSLATE_NOOP( "@default","Status busy" ),"StatusBusy",true );
	ConfigDialog::addCheckBox(config, "Autoresponder", "Autoresponder options", QT_TRANSLATE_NOOP( "@default","Status available" ),"StatusAvailable",false );
	ConfigDialog::addCheckBox(config, "Autoresponder", "Autoresponder options", QT_TRANSLATE_NOOP( "@default","Only for the first time" ),"OnlyFirstTime",true );
	kdebugf2();
}

AutoResponder::~AutoResponder()
{
	kdebugf();
	QObject::disconnect(chat_manager,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		this,SLOT(chatReceived(UinsList,const QString&,time_t)));
	QObject::disconnect(chat_manager, SIGNAL(chatCreated(const UinsList&)), this, SLOT(chatOpened(const UinsList&)));
	ConfigDialog::removeControl("Autoresponder", "Choose status:");
	ConfigDialog::removeControl("Autoresponder", "Status invisible");
	ConfigDialog::removeControl("Autoresponder", "Status busy");
	ConfigDialog::removeControl("Autoresponder", "Status available");
	ConfigDialog::removeControl("Autoresponder", "Autoanswer text:");
	ConfigDialog::removeControl("Autoresponder", "Only for the first time");
	ConfigDialog::removeControl("Autoresponder", "Autoresponder options");
	ConfigDialog::removeTab("Autoresponder");
	config->sync();
	delete config;
	kdebugf2();
}

void AutoResponder::chatReceived(UinsList senders, const QString& msg, time_t time)
{
	kdebugf();
	if (msg.left(5)!="KADU ")
	{
		int status=gadu->getCurrentStatus() & ~GG_STATUS_FRIENDS_MASK; //pozbywamy siê flagi "Tylko dla przyjació³"
		
		bool was=false;					//to pamieta czy okienko juz otwarte czy nie
		if (!UserList.isEmpty())
		for (unsigned int l=0; l<senders.count(); ++l)
			if (UserList.findIndex(senders[l])!=-1)
				was=true;						//jak bylo to bylo=true		
	
		bool respond=config->readBoolEntry("Autoresponder", "StatusInvisible") &&
					(status==GG_STATUS_INVISIBLE || status==GG_STATUS_INVISIBLE_DESCR);
	
		if (!respond)
			respond=config->readBoolEntry("Autoresponder", "StatusBusy") &&
					(status==GG_STATUS_BUSY || status==GG_STATUS_BUSY_DESCR);

		if (!respond)
			respond=config->readBoolEntry("Autoresponder", "StatusAvailable") &&
					(status==GG_STATUS_AVAIL || status==GG_STATUS_AVAIL_DESCR);

		if ((config->readBoolEntry("Autoresponder", "OnlyFirstTime")) && (was))
			respond=false;			//to zablokuje odpisanie na wiadomosc
		
		if (respond)
			{	gadu->sendMessage(senders, unicode2cp(tr("KADU AUTORESPONDER:")+"\n"+
							config->readEntry("Autoresponder", "Autotext")));
				for (unsigned int l=0; l<senders.count(); ++l)
					UserList+=senders[l];	//doda kolesi do listy (jednego jak jeden albo wszystkich z konferencji
			}
	}
	kdebugf2();
}

void AutoResponder::chatOpened(const UinsList& senders)
{	int indexx;
	for (unsigned int l=0; l<UserList.count(); ++l)
	{	indexx=UserList.findIndex(senders[l]);
		if (indexx!=-1)
			UserList.remove(indexx);
	}
}


AutoResponder* autoresponder;
