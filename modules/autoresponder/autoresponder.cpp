/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "autoresponder.h"
#include "chat.h"
#include "chat_manager.h"
#include "config_dialog.h"
#include "debug.h"
#include "gadu.h"
#include "kadu.h"
#include "kadu_parser.h"

extern "C" int autoresponder_init()
{
	autoresponder = new AutoResponder(NULL, "autoresponder");
	return 0;
}

extern "C" void autoresponder_close()
{
	delete autoresponder;
	autoresponder = NULL;
}

AutoResponder::AutoResponder(QObject *parent, const char *name) : QObject(parent, name),
	config(new ConfigFile(ggPath("autoresponder.conf"))),
	replied(userlist->count() / 2, "replied_(autoresponder)")
{
	kdebugf();
	connect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		this, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	connect(chat_manager, SIGNAL(chatCreated(const UserGroup *)), this, SLOT(chatOpened(const UserGroup *)));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Autoresponder"), "AutoresponderTab");
	ConfigDialog::addVGroupBox( "Autoresponder", "Autoresponder", QT_TRANSLATE_NOOP( "@default", "Autoresponder options" ));
	ConfigDialog::addLineEdit(config, "Autoresponder","Autoresponder options",
		QT_TRANSLATE_NOOP("@default", "Autoanswer text:"),
		"Autotext",
		tr("Thanks for your message %a. I'm not available right now."), Kadu::SyntaxText);
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
	disconnect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		this, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	disconnect(chat_manager, SIGNAL(chatCreated(const UserGroup *)), this, SLOT(chatOpened(const UserGroup *)));
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

void AutoResponder::chatMsgReceived(Protocol * /*protocol*/, UserListElements senders, const QString& msg, time_t /*time*/, bool &/*grab*/)
{
	kdebugf();
	if (msg.left(5) != "KADU ")
	{
		bool was = false;					//to pamieta czy okienko juz otwarte czy nie
		if (replied.count() != 0)
			CONST_FOREACH(sender, senders)
				if (replied.contains(*sender))
					was = true;						//jak bylo to bylo=true

		bool respond = config->readBoolEntry("Autoresponder", "StatusInvisible") &&
					gadu->status().isInvisible();

		if (!respond)
			respond = config->readBoolEntry("Autoresponder", "StatusBusy") &&
					gadu->status().isBusy();;

		if (!respond)
			respond = config->readBoolEntry("Autoresponder", "StatusAvailable") &&
					gadu->status().isOnline();

		if (config->readBoolEntry("Autoresponder", "OnlyFirstTime") && was)
			respond = false;			//to zablokuje odpisanie na wiadomosc

		if (respond)
		{
			gadu->sendMessage(senders, unicode2cp(tr("KADU AUTORESPONDER:")+"\n"+
						KaduParser::parse(config->readEntry("Autoresponder", "Autotext"), senders[0])));
			CONST_FOREACH(sender, senders)
				replied.addUser(*sender);	//doda kolesi do listy (jednego jak jeden albo wszystkich z konferencji
		}
	}
	kdebugf2();
}

void AutoResponder::chatOpened(const UserGroup *group)
{
	CONST_FOREACH(sender, *group)
		if (replied.contains(*sender))
			replied.removeUser(*sender);
}

AutoResponder* autoresponder;
