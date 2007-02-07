/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "history_module.h"

#include "action.h"
#include "chat_manager.h"
#include "debug.h"
#include "history.h"
#include "history_dialog.h"
#include "icons_manager.h"
#include "misc.h"

extern "C" int history_init()
{
	kdebugf();
	history_module = new HistoryModule();
	kdebugf2();
	return 0;
}

extern "C" void history_close()
{
	kdebugf();
	delete history_module;
	history_module = NULL;
	kdebugf2();
}

HistoryModule::HistoryModule() : QObject(NULL, "history")
{
	kdebugf();

	Action* history_action = new Action(icons_manager->loadIcon("History"),
		tr("Show history"), "showHistoryAction", Action::TypeUser);
	connect(history_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(historyActionActivated(const UserGroup*)));
	KaduActions.insert("showHistoryAction", history_action);

	KaduActions.addDefaultToolbarAction("Kadu toolbar", "showHistoryAction", 4);
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "showHistoryAction", 3);

	kdebugf2();
}

HistoryModule::~HistoryModule()
{
	kdebugf();

	KaduActions.remove("showHistoryAction");

	kdebugf2();
}

void HistoryModule::historyActionActivated(const UserGroup* users)
{
	kdebugf();
	UinsList uins;
	CONST_FOREACH(user, *users)
		uins.append((*user).ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	(new HistoryDialog(uins))->show();
	kdebugf2();
}

HistoryModule* history_module = NULL;
