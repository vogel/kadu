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

	connect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		history, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));

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

	disconnect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		history, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));

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

HistoryModule* history_module = NULL;
