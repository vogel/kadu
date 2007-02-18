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
#include "config_dialog.h"
#include "debug.h"
#include "history.h"
#include "history_dialog.h"
#include "icons_manager.h"
#include "misc.h"
#include "userbox.h"

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

HistorySlots::HistorySlots(QObject *parent, const char *name) : QObject(parent, name)
{
}

void HistorySlots::onCreateTabHistory()
{
	kdebugf();
	QLabel *l_qtimeinfo=(QLabel*)(ConfigDialog::widget("History", 0, "dayhour"));
	l_qtimeinfo->setAlignment(Qt::AlignHCenter);
	updateQuoteTimeLabel(config_file.readNumEntry("History", "ChatHistoryQuotationTime"));
	kdebugf2();
}

void HistorySlots::onApplyTabHistory()
{
//	kdebugf();
//	kdebugf2();
}

void HistorySlots::updateQuoteTimeLabel(int value)
{
	kdebugf();
	ConfigDialog::getLabel("History", 0, "dayhour") ->
			setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
	kdebugf2();
}

HistorySlots* HistoryModule::historyslots;

HistoryModule::HistoryModule() : QObject(NULL, "history")
{
	kdebugf();

	history = new HistoryManager(0, "history_manager");
	historyslots = new HistorySlots(history, "history_slots");

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "History"), "HistoryTab");
	ConfigDialog::addVGroupBox("History", "History", QT_TRANSLATE_NOOP("@default","Message citation in chat window"));
	ConfigDialog::addSpinBox("History", "Message citation in chat window", QT_TRANSLATE_NOOP("@default", "Count:"), "ChatHistoryCitation", 0, 200, 1, 10);
	ConfigDialog::addLabel("History", "Message citation in chat window", QT_TRANSLATE_NOOP("@default", "Don't cite messages older than:"));
	ConfigDialog::addSlider("History", "Message citation in chat window", "historyslider", "ChatHistoryQuotationTime", -744, -1, 24, -336);
	ConfigDialog::addLabel("History", "Message citation in chat window", 0, "dayhour");
	ConfigDialog::addCheckBox("History", "History", QT_TRANSLATE_NOOP("@default", "Log messages"), "Logging", true);
	ConfigDialog::addCheckBox("History", "History", QT_TRANSLATE_NOOP("@default", "Don't show status changes"), "DontShowStatusChanges", false, 0, 0, Advanced);
	ConfigDialog::addCheckBox("History", "History", QT_TRANSLATE_NOOP("@default", "Don't save status changes"), "DontSaveStatusChanges", true, 0, 0, Advanced);

	ConfigDialog::registerSlotOnCreateTab("History", historyslots, SLOT(onCreateTabHistory()));
	ConfigDialog::registerSlotOnApplyTab("History", historyslots, SLOT(onApplyTabHistory()));
	ConfigDialog::connectSlot("History", "historyslider", SIGNAL(valueChanged(int)), historyslots, SLOT(updateQuoteTimeLabel(int)));

	ConfigDialog::addCheckBox("General", "grid-expert", QT_TRANSLATE_NOOP("@default", "Show emoticons in history"), "ShowEmotHist", false, 0, 0, Expert);
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "View history"), "kadu_viewhistory", "Ctrl+H");

	connect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		history, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	connect(chat_manager, SIGNAL(chatCreated(const UserGroup*, time_t)),
		this, SLOT(chatCreated(const UserGroup*, time_t)));

	Action* history_action = new Action(icons_manager->loadIcon("History"),
		tr("Show history"), "showHistoryAction", Action::TypeUser);
	connect(history_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(historyActionActivated(const UserGroup*)));
	KaduActions.insert("showHistoryAction", history_action);

	KaduActions.addDefaultToolbarAction("Kadu toolbar", "showHistoryAction", 4);
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "showHistoryAction", 3);

	UserBox::userboxmenu->addItemAtPos(5, "History", tr("View history"), this, SLOT(viewHistory()), HotKey::shortCutFromFile("ShortCuts", "kadu_viewhistory"));
	UserBox::management->addItemAtPos(7, tr("Clear history"),  this, SLOT(deleteHistory()));
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userboxMenuPopup()));

	kdebugf2();
}

HistoryModule::~HistoryModule()
{
	kdebugf();

	int history_item = UserBox::userboxmenu->getItem(tr("View history"));
	int delete_history_item = UserBox::management->getItem(tr("Clear history"));
	UserBox::userboxmenu->removeItem(history_item);
	UserBox::userboxmenu->removeItem(delete_history_item);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()), this, SLOT(userboxMenuPopup()));

	KaduActions.remove("showHistoryAction");

	disconnect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		history, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	disconnect(chat_manager, SIGNAL(chatCreated(const UserGroup*, time_t)),
		this, SLOT(chatCreated(const UserGroup*, time_t)));

	ConfigDialog::removeControl("General", "Show emoticons in history");
	ConfigDialog::removeControl("ShortCuts", "View history");

	ConfigDialog::disconnectSlot("History", "historyslider", SIGNAL(valueChanged(int)), historyslots, SLOT(updateQuoteTimeLabel(int)));
	ConfigDialog::unregisterSlotOnCreateTab("History", historyslots, SLOT(onCreateTabHistory()));
	ConfigDialog::unregisterSlotOnApplyTab("History", historyslots, SLOT(onApplyTabHistory()));

	ConfigDialog::removeControl("History", "Don't save status changes");
	ConfigDialog::removeControl("History", "Don't show status changes");
	ConfigDialog::removeControl("History", "Log messages");
	ConfigDialog::removeControl("History", 0, "dayhour");
	ConfigDialog::removeControl("History", "historyslider");
	ConfigDialog::removeControl("History", "Don't cite messages older than:");
	ConfigDialog::removeControl("History", "Count:");
	ConfigDialog::removeControl("History", "Message citation in chat window");
	ConfigDialog::removeTab("History");

	delete historyslots;
	historyslots = 0;
	delete history;
	history = 0;

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

void HistoryModule::chatCreated(const UserGroup *group, time_t time)
{
	kdebugf();
	Chat* chat = chat_manager->findChat(group);
	connect(chat, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SLOT(messageSentAndConfirmed(UserListElements, const QString&)));
	UserListElements senders = group->toUserListElements();

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
		kdebugmf(KDEBUG_INFO, "temp entries = %lu\n", entriestmp.count());
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
		kdebugmf(KDEBUG_INFO, "entries = %lu\n", entries.count());
		end = from - 1;
	}

	unsigned int entryCount = entries.count();
	if (entryCount < chatHistoryQuotation)
		from = 0;
	else
		from = entryCount - chatHistoryQuotation;

	QValueList<ChatMessage *> messages;

	int quotTime = config_file.readNumEntry("History","ChatHistoryQuotationTime");
	QString myNick = config_file.readEntry("General","Nick");

	QValueListConstIterator<HistoryEntry> entry = entries.at(from);
	QValueListConstIterator<HistoryEntry> entriesEnd = entries.end();
	for (; entry!=entriesEnd; ++entry)
		if ((*entry).date.secsTo(QDateTime::currentDateTime()) <= -quotTime * 3600)
		{
			ChatMessage *msg;
			if ((*entry).type == HISTORYMANAGER_ENTRY_MSGSEND || (*entry).type == HISTORYMANAGER_ENTRY_CHATSEND)
				msg = new ChatMessage(myNick, (*entry).message, true, (*entry).date);
			else
				msg = new ChatMessage((*entry).nick, (*entry).message, false, (*entry).date, (*entry).sdate);
			messages.append(msg);
		}
	chat->formatMessages(messages);
	if (!messages.empty())
		chat->scrollMessages(messages);
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
	UserListElements users = UserBox::activeUserBox()->selectedUsers();
	int history_item = UserBox::userboxmenu->getItem(tr("View history"));
	int delete_history_item = UserBox::management->getItem(tr("Clear history"));

	bool any_ok = false;
	CONST_FOREACH(user, users)
		if ((*user).usesProtocol("Gadu") && (*user).ID("Gadu").toUInt() != config_file.readUnsignedNumEntry("General","UIN"))
		{
			any_ok = true;
			break;
		}
	UserBox::userboxmenu->setItemEnabled(history_item, any_ok);
	UserBox::userboxmenu->setItemEnabled(delete_history_item, any_ok);
}

HistoryModule* history_module = NULL;
