/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "history_module.h"

#include <sys/stat.h>
#include <sys/types.h>

#include "action.h"
#include "chat_manager.h"
#include "debug.h"
#include "history.h"
#include "history_dialog.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "message_box.h"
#include "userbox.h"

extern "C" int history_init()
{
	kdebugf();

	history_module = new HistoryModule();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/history.ui"), history_module);

	kdebugf2();
	return 0;
}

extern "C" void history_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/history.ui"), history_module);
	delete history_module;
	history_module = 0;

	kdebugf2();
}

HistoryModule::HistoryModule() : QObject(NULL, "history")
{
	kdebugf();

	QString path_ = ggPath(QString::null);
	path_.append("/history/");
	mkdir(path_.local8Bit().data(), 0700);

	history = new HistoryManager(0, "history_manager");

	connect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
		history, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *, time_t)),
		this, SLOT(chatCreated(ChatWidget*, time_t)));
	connect(kadu, SIGNAL(removingUsers(UserListElements)),
		this, SLOT(removingUsers(UserListElements)));

	Action* history_action = new Action("History", tr("Show history"), "showHistoryAction", Action::TypeUser);
	connect(history_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(historyActionActivated(const UserGroup*)));
	KaduActions.insert("showHistoryAction", history_action);

	KaduActions.addDefaultToolbarAction("Kadu toolbar", "showHistoryAction", 4);
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "showHistoryAction", 3);

	UserBox::userboxmenu->addItemAtPos(5, "History", tr("History"), this, SLOT(viewHistory()), HotKey::shortCutFromFile("ShortCuts", "kadu_viewhistory"));
	UserBox::management->addItemAtPos(7, "ClearHistory", tr("Clear history"),  this, SLOT(deleteHistory()));
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userboxMenuPopup()));

	kdebugf2();
}

HistoryModule::~HistoryModule()
{
	kdebugf();

	int history_item = UserBox::userboxmenu->getItem(tr("History"));
	int delete_history_item = UserBox::management->getItem(tr("Clear history"));
	UserBox::userboxmenu->removeItem(history_item);
	UserBox::management->removeItem(delete_history_item);
	disconnect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userboxMenuPopup()));

	KaduActions.remove("showHistoryAction");

	disconnect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
		history, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *, time_t)),
		this, SLOT(chatCreated(ChatWidget *, time_t)));
	disconnect(kadu, SIGNAL(removingUsers(UserListElements)),
		this, SLOT(removingUsers(UserListElements)));

	delete history;
	history = 0;

	kdebugf2();
}

void HistoryModule::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	dontCiteOldMessagesLabel = dynamic_cast<QLabel *>(mainConfigurationWindow->widgetById("history/dontCiteOldMessagesLabel"));
	connect(mainConfigurationWindow->widgetById("history/dontCiteOldMessages"), SIGNAL(valueChanged(int)),
		this, SLOT(updateQuoteTimeLabel(int)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/statusChanges"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/citation"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/shortCuts"), SLOT(setEnabled(bool)));
}

void HistoryModule::updateQuoteTimeLabel(int value)
{
	dontCiteOldMessagesLabel->setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
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

void HistoryModule::chatCreated(ChatWidget *chat, time_t time)
{
	kdebugf();
	connect(chat, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SLOT(messageSentAndConfirmed(UserListElements, const QString&)));
	UserListElements senders = chat->users()->toUserListElements();

	QValueList<HistoryEntry> entries;
	QValueList<HistoryEntry> entriestmp;
	QDateTime date;
	unsigned int from, end, count;

	date.setTime_t(time);

	UinsList uins;//TODO: throw out UinsList as soon as possible!
	CONST_FOREACH(user, senders)
		uins.append((*user).ID("Gadu").toUInt());

	count = history->getHistoryEntriesCount(uins);
	end = count - 1;

	from = count;
	unsigned int chatHistoryQuotation=config_file.readUnsignedNumEntry("History", "ChatHistoryCitation");
	while (from >= 1 && entries.count() < chatHistoryQuotation)
	{
		if (end < chatHistoryQuotation)
			from = 0;
		else
			from = end - chatHistoryQuotation + 1;

		entriestmp = history->getHistoryEntries(uins, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebugmf(KDEBUG_INFO, "temp entries = %u\n", entriestmp.count());
		if (time)
		{
			QValueList<HistoryEntry>::iterator it = entriestmp.begin();
			while (it != entriestmp.end())
			{
				if ((*it).type == HISTORYMANAGER_ENTRY_CHATRCV
					|| (*it).type == HISTORYMANAGER_ENTRY_MSGRCV)
				{
					kdebugmf(KDEBUG_INFO, "%s %s\n",
						date.toString("dd.MM.yyyy hh:mm:ss").local8Bit().data(),
						(*it).sdate.toString("dd.MM.yyyy hh:mm:ss").local8Bit().data());
					if (date <= (*it).sdate)
						it = entriestmp.remove(it);
					else
						++it;
				}
				else
					++it;
			}
		}
		if (!entriestmp.isEmpty())
			entries = entriestmp + entries;
		kdebugmf(KDEBUG_INFO, "entries = %u\n", entries.count());
		end = from - 1;
	}

	unsigned int entryCount = entries.count();
	if (entryCount < chatHistoryQuotation)
		from = 0;
	else
		from = entryCount - chatHistoryQuotation;

	QValueList<ChatMessage *> messages;

	int quotTime = config_file.readNumEntry("History","ChatHistoryQuotationTime");

	QValueListConstIterator<HistoryEntry> entry = entries.at(from);
	QValueListConstIterator<HistoryEntry> entriesEnd = entries.end();
	for (; entry!=entriesEnd; ++entry)
		if ((*entry).date.secsTo(QDateTime::currentDateTime()) <= -quotTime * 3600)
		{
			ChatMessage *message;
			if ((*entry).type == HISTORYMANAGER_ENTRY_MSGSEND || (*entry).type == HISTORYMANAGER_ENTRY_CHATSEND)
				message = new ChatMessage(kadu->myself(), (*entry).message, TypeSent, (*entry).date);
			else
			message = new ChatMessage(userlist->byID("Gadu", QString::number((*entry).uin)), (*entry).message, TypeReceived, (*entry).date, (*entry).sdate);
			messages.append(message);
		}
	if (!messages.empty())
		chat->appendMessages(messages);
	kdebugf2();
}

void HistoryModule::messageSentAndConfirmed(UserListElements receivers, const QString& message)
{
	UinsList uins;
	CONST_FOREACH(user, receivers)
		uins.append((*user).ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	history->addMyMessage(uins, message);
}

void HistoryModule::viewHistory()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();

	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}
	UserListElements users = activeUserBox->selectedUsers();
	UserGroup user_group(users);
	KaduActions["showHistoryAction"]->activate(&user_group);
	kdebugf2();
}

void HistoryModule::deleteHistory()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}
	//TODO: throw out UinsList as soon as possible!
	UinsList uins;
	UserListElements users = activeUserBox->selectedUsers();
	CONST_FOREACH(user, users)
		if ((*user).usesProtocol("Gadu"))
			uins.append((*user).ID("Gadu").toUInt());

	history->removeHistory(uins);
	kdebugf2();
}

void HistoryModule::userboxMenuPopup()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	if (!users.count())
	{
		kdebugf2();
		return;
	}

	int history_item = UserBox::userboxmenu->getItem(tr("History"));
	int delete_history_item = UserBox::management->getItem(tr("Clear history"));

	bool any_ok = false;
	CONST_FOREACH(user, users)
		if ((*user).usesProtocol("Gadu") && ((*user).ID("Gadu") != kadu->myself().ID("Gadu")))
		{
			any_ok = true;
			break;
		}
	UserBox::userboxmenu->setItemVisible(history_item, any_ok);
	UserBox::userboxmenu->setItemVisible(delete_history_item, any_ok);

	kdebugf2();
}

void HistoryModule::removingUsers(UserListElements users)
{
	kdebugf();
	if (!MessageBox::ask(tr("The following users were deleted:\n%0\nDo you want to remove history as well?").arg(users.altNicks().join(", ")), "Warning", kadu))
		return;

	QString fname;
	CONST_FOREACH(user, users)
	{
		if ((*user).usesProtocol("Gadu"))
		{
			fname = ggPath("history/") + (*user).ID("Gadu");
			kdebugmf(KDEBUG_INFO, "deleting %s\n", (const char *)fname.local8Bit());
			QFile::remove(fname);
			QFile::remove(fname + ".idx");
		}
	}
	kdebugf2();
}

HistoryModule* history_module = NULL;
