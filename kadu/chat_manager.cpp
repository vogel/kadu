/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qbitmap.h>

#include "action.h"
#include "chat_manager_slots.h"
#include "config_dialog.h"
#include "debug.h"
#include "chat_manager.h"
#include "history.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_splitter.h"
#include "misc.h"
#include "pending_msgs.h"
#include "search.h"
#include "userbox.h"

// TODO: make new icons for iconset instead of tricks like that
static QPixmap string_to_pixmap(const QString& str, const QFont& font)
{
	QSize size = QFontMetrics(font).size(0, str);
	QPixmap pixmap(size);
	pixmap.fill();
	QPainter* painter = new QPainter();
	painter->begin(&pixmap);
	painter->setFont(font);
	painter->drawText(QRect(QPoint(0, 0), size), 0, str);
	painter->end();
	pixmap.setMask(pixmap.createHeuristicMask());
	delete painter;
	return pixmap;
}

ChatManager::ChatManager(QObject* parent, const char* name)
	: QObject(parent, name)
{
	kdebugf();

	Action* auto_send_action = new Action(icons_manager->loadIcon("AutoSendMessage"),
		tr("%1 sends message").arg(config_file.readEntry("ShortCuts", "chat_newline")),
		"autoSendAction");
	auto_send_action->setToggleAction(true);
	auto_send_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(auto_send_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(autoSendActionActivated(const UserGroup*, const QWidget*, bool)));
	connect(auto_send_action, SIGNAL(addedToToolbar(ToolButton*, ToolBar*,
			const UserListElements&)),
		this, SLOT(autoSendActionAddedToToolbar(ToolButton*, ToolBar*,
			const UserListElements&)));
	KaduActions.insert("autoSendAction", auto_send_action);

	Action* scroll_lock_action = new Action(icons_manager->loadIcon("ScrollLock"),
		tr("Blocks scrolling"), "scrollLockAction");
	scroll_lock_action->setToggleAction(true);
	scroll_lock_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(scroll_lock_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(scrollLockActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("scrollLockAction", scroll_lock_action);

	Action* clear_action = new Action(icons_manager->loadIcon("ClearChat"),
		tr("Clear messages in chat window"), "clearChatAction");
	clear_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(clear_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(clearActionActivated(const UserGroup*)));
	KaduActions.insert("clearChatAction", clear_action);

	Action* history_action = new Action(icons_manager->loadIcon("History"),
		tr("Show history"), "showHistoryAction");
	connect(history_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(historyActionActivated(const UserGroup*)));
	KaduActions.insert("showHistoryAction", history_action);

	Action* insert_emot_action = new Action(icons_manager->loadIcon("ChooseEmoticon"),
		tr("Insert emoticon"), "insertEmoticonAction");
	insert_emot_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(insert_emot_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(insertEmoticonActionActivated(const UserGroup*, const QWidget*)));
	connect(insert_emot_action, SIGNAL(addedToToolbar(ToolButton*, ToolBar*,
			const UserListElements&)),
		this, SLOT(insertEmoticonActionAddedToToolbar(ToolButton*, ToolBar*,
			const UserListElements&)));
	KaduActions.insert("insertEmoticonAction", insert_emot_action);

	Action* whois_action = new Action(icons_manager->loadIcon("LookupUserInfo"),
		tr("Lookup user info"), "whoisAction");
	connect(whois_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(whoisActionActivated(const UserGroup*)));
	KaduActions.insert("whoisAction", whois_action);

	Action* insert_img_action = new Action(icons_manager->loadIcon("ChooseImage"),
		tr("Insert image"), "insertImageAction");
	insert_img_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(insert_img_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(insertImageActionActivated(const UserGroup*)));
	KaduActions.insert("insertImageAction", insert_img_action);

	QFont font;

	font.setBold(true);
	Action* bold_action = new Action(string_to_pixmap("B", font),
		tr("Bold"), "boldAction");
	bold_action->setToggleAction(true);
	bold_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(bold_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(boldActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("boldAction", bold_action);

	font.setBold(false);
	font.setItalic(true);
	Action* italic_action = new Action(string_to_pixmap("I", font),
		tr("Italic"), "italicAction");
	italic_action->setToggleAction(true);
	italic_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(italic_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(italicActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("italicAction", italic_action);

	font.setItalic(false);
	font.setUnderline(true);
	Action* underline_action = new Action(string_to_pixmap("U", font),
		tr("Underline"), "underlineAction");
	underline_action->setToggleAction(true);
	underline_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(underline_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(underlineActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("underlineAction", underline_action);

	QPixmap p(12, 12);
	p.fill(Qt::black);
	Action* color_action = new Action(p,
		tr("Change color"), "colorAction");
	color_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(color_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(colorActionActivated(const UserGroup*, const QWidget*)));
	KaduActions.insert("colorAction", color_action);

	Action* send_action = new Action(icons_manager->loadIcon("SendMessage"),
		tr("&Send"), "sendAction");
	send_action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(send_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(sendActionActivated(const UserGroup*)));
	KaduActions.insert("sendAction", send_action);

	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "autoSendAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "scrollLockAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "clearChatAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "showHistoryAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "insertEmoticonAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "whoisAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "insertImageAction");

	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "boldAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "italicAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "underlineAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "colorAction");

	KaduActions.addDefaultToolbarAction("Chat toolbar 3", "sendAction", -1, true);

	kdebugf2();
}

void ChatManager::closeAllWindows()
{
	kdebugf();
	while (!Chats.empty())
	{
		Chat *chat=Chats.first();
		delete chat;
	}
	kdebugf2();
}

ChatManager::~ChatManager()
{
	kdebugf();
	closeAllWindows();
}

void ChatManager::autoSendActionAddedToToolbar(ToolButton* button, ToolBar* toolbar,
	const UserListElements& users)
{
	kdebugf();
	button->setOn(findChat(users)->autoSend());
	kdebugf2();
}

void ChatManager::autoSendActionActivated(const UserGroup* users, const QWidget* source, bool is_on)
{
	kdebugf();
	findChat(users)->setAutoSend(is_on);
	kdebugf2();
}

void ChatManager::scrollLockActionActivated(const UserGroup* users, const QWidget* source, bool is_on)
{
	kdebugf();
	findChat(users)->setScrollLocked(is_on);
	kdebugf2();
}

void ChatManager::clearActionActivated(const UserGroup* users)
{
	kdebugf();
	findChat(users)->clearChatWindow();
	kdebugf2();
}

void ChatManager::historyActionActivated(const UserGroup* users)
{
	kdebugf();
	UinsList uins;
	CONST_FOREACH(user, *users)
		uins.append((*user).ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	(new History(uins))->show();
	kdebugf2();
}

void ChatManager::boldActionActivated(const UserGroup* users, const QWidget* source, bool is_on)
{
	kdebugf();
	findChat(users)->edit()->setBold(is_on);
	kdebugf2();
}

void ChatManager::italicActionActivated(const UserGroup* users, const QWidget* source, bool is_on)
{
	kdebugf();
	findChat(users)->edit()->setItalic(is_on);
	kdebugf2();
}

void ChatManager::underlineActionActivated(const UserGroup* users, const QWidget* source, bool is_on)
{
	kdebugf();
	findChat(users)->edit()->setUnderline(is_on);
	kdebugf2();
}

void ChatManager::colorActionActivated(const UserGroup* users, const QWidget* source)
{
	kdebugf();
	findChat(users)->changeColor(source);
	kdebugf2();
}

void ChatManager::insertEmoticonActionActivated(const UserGroup* users, const QWidget* source)
{
	kdebugf();
	findChat(users)->openEmoticonSelector(source);
	kdebugf2();
}

void ChatManager::insertEmoticonActionAddedToToolbar(ToolButton* button, ToolBar* toolbar,
	const UserListElements& users)
{
	kdebugf();
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_NONE)
	{
		QToolTip::remove(button);
		QToolTip::add(button, tr("Insert emoticon - enable in configuration"));
		button->setEnabled(false);
	}
	kdebugf2();
}

void ChatManager::whoisActionActivated(const UserGroup* users)
{
	kdebugf();
	if (users->count() == 0)
	{
		SearchDialog* sd = new SearchDialog();
		sd->show();
	}
	else
	{
		UserListElement user = *users->constBegin();
		if (user.usesProtocol("Gadu"))
		{
			UinType uin = user.ID("Gadu").toUInt();
			SearchDialog *sd = new SearchDialog(0, QString("SearchDialog:%1").arg(uin).local8Bit().data(), uin);
			sd->show();
			sd->firstSearch();
		}
	}
	kdebugf2();
}

void ChatManager::insertImageActionActivated(const UserGroup* users)
{
	kdebugf();
	findChat(users)->insertImage();
	kdebugf2();
}

void ChatManager::sendActionActivated(const UserGroup* users)
{
	kdebugf();
	Chat* chat = findChat(users);
	if (chat->waitingForACK())
		chat->cancelMessage();
	else
		chat->sendMessage();
	kdebugf2();
}

const ChatList& ChatManager::chats() const
{
	return Chats;
}

int ChatManager::registerChat(Chat* chat)
{
	Chats.append(chat);
	return Chats.count() - 1;
}

void ChatManager::unregisterChat(Chat* chat)
{
	kdebugf();
	FOREACH(curChat, Chats)
		if (*curChat == chat)
		{
			const UserGroup *users = chat->users();
			setChatProperty(users, "Geometry", QRect(chat->pos().x(), chat->pos().y(), chat->size().width(), chat->size().height()));
			setChatProperty(users, "VerticalSizes", toVariantList(chat->vertSplit->sizes()));
			if (chat->horizSplit)
				setChatProperty(users, "HorizontalSizes", toVariantList(chat->horizSplit->sizes()));

			emit chatDestroying(users);
			Chats.remove(curChat);
			emit chatDestroyed(users);
			kdebugf2();
			return;
		}
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "NOT found\n");
}

void ChatManager::refreshTitles()
{
	kdebugf();
	CONST_FOREACH(chat, Chats)
		(*chat)->refreshTitle();
	kdebugf2();
}

void ChatManager::refreshTitlesForUser(UserListElement user)
{
	kdebugf();
	CONST_FOREACH(chat, Chats)
		if ((*chat)->users()->contains(user))
			(*chat)->refreshTitle();
	kdebugf2();
}

void ChatManager::changeAppearance()
{
	kdebugf();
	CONST_FOREACH(chat, Chats)
		(*chat)->changeAppearance();
	kdebugf2();
}

Chat* ChatManager::findChat(const UserGroup *group) const
{
	CONST_FOREACH(chat, Chats)
		if ((*chat)->users() == group)
			return *chat;
	kdebugmf(KDEBUG_WARNING, "no such chat\n");
	return NULL;
}

Chat* ChatManager::findChat(UserListElements users) const
{
	CONST_FOREACH(chat, Chats)
		if (users.equals((*chat)->users()))
			return *chat;
	kdebugmf(KDEBUG_WARNING, "no such chat\n");
	return NULL;
}

int ChatManager::openChat(QString initialProtocol, UserListElements users, time_t time)
{
	kdebugf();
	emit chatOpen(users);
	unsigned int i = 0;
	CONST_FOREACH(chat, Chats)
	{
		if ((*chat)->users()->equals(users))
		{
#if QT_VERSION >= 0x030300
			(*chat)->setWindowState((*chat)->windowState() & ~WindowMinimized);
#endif
			(*chat)->raise();
			(*chat)->setActiveWindow();
			return i;
		}
		++i;
	}
	QStringList userNames;
	CONST_FOREACH(user, users)
		userNames.append((*user).altNick());
	userNames.sort();

	Chat* chat = new Chat(users, 0, QString("chat:%1").arg(userNames.join(",")).local8Bit().data());
	chat->refreshTitle();

	const UserGroup *group = chat->users();
	QRect geometry = getChatProperty(group, "Geometry").toRect();
	if (geometry.isEmpty())
	{
		QPoint pos = QCursor::pos();
		int x,y,width,height;
		QDesktopWidget *desk=qApp->desktop();
		x=pos.x()+50;
		y=pos.y()+50;
		height=400;

		if (group->count() > 1)
			width=550;
		else
			width=400;
		if (x+width>desk->width())
			x=desk->width()-width-50;
		if (y+height>desk->height())
			y=desk->height()-height-50;
		if (x<50) x=50;
		if (y<50) y=50;
		geometry.setX(x);
		geometry.setY(y);
		geometry.setWidth(width);
		geometry.setHeight(height);
	}
	chat->setGeometry(geometry);

	QValueList<int> vertSizes = toIntList(getChatProperty(group, "VerticalSizes").toList());
	if (!vertSizes.empty())
		chat->vertSplit->setSizes(vertSizes);

	if (chat->horizSplit)
	{
		QValueList<int> horizSizes = toIntList(getChatProperty(group, "HorizontalSizes").toList());
		if (!horizSizes.empty())
			chat->horizSplit->setSizes(horizSizes);
	}

	chat->show();
	chat->writeMessagesFromHistory(users, time);
	emit chatCreated(group);
	kdebugf2();
	return Chats.count() - 1;
}

int ChatManager::openPendingMsg(int index, ChatMessage &msg)
{
	kdebugf();
	// TODO: check if index does not exceed boundaries
	PendingMsgs::Element p = pending[index];
	// opening chat (if it does not exist)
	int k = openChat("Gadu", p.users, p.time);
	// appending new message

	QDateTime date;
	date.setTime_t(p.time);

	msg = ChatMessage(p.users[0].altNick(), p.msg, false, QDateTime::currentDateTime(), date);
	Chats[k]->formatMessage(msg);

	// remove message from pending
	pending.deleteMsg(index);
	kdebugf2();
	// return index of opened chat window
	return k;
}

void ChatManager::deletePendingMsgs(UserListElements users)
{
	kdebugf();

	for (int i = 0; i < pending.count(); ++i)
		if (pending[i].users.equals(users))
		{
			pending.deleteMsg(i);
			--i;
		}

	UserBox::refreshAll();
	kdebugf2();
}

void ChatManager::openPendingMsgs(UserListElements users)
{
	kdebugf();
	PendingMsgs::Element elem;
	int k;
	bool stop = false;

	QValueList<ChatMessage *> messages;
	for (int i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if (elem.users.equals(users))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass)
			{
				ChatMessage *msg = new ChatMessage(QString::null);
				k = openPendingMsg(i, *msg);
				messages.append(msg);

				--i;
				users = elem.users;
				stop = true;
			}
	}
	if (stop)
	{
		Chats[k]->scrollMessages(messages);
		UserBox::refreshAll();
	}
	else
		k = openChat("Gadu", users, 0);
	kdebugf2();
}

void ChatManager::openPendingMsgs()
{
	kdebugf();
	UserListElements users;
	int i, k = -1;
	PendingMsgs::Element elem;
	bool stop = false;
	QValueList<ChatMessage *> messages;

	for(i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if (users.isEmpty() || elem.users.equals(users))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| (!elem.msgclass))
			{
				if (users.isEmpty())
					users = elem.users;

				ChatMessage *msg = new ChatMessage(QString::null);
				k = openPendingMsg(i, *msg);
				messages.append(msg);

				--i;
				stop = true;
			}
	}
	if (stop)
	{
		kdebugmf(KDEBUG_INFO, "stopped\n");
		Chats[k]->scrollMessages(messages);
		UserBox::refreshAll();
	}
	kdebugf2();
}

void ChatManager::sendMessage(UserListElement user, UserListElements selected_users)
{
	kdebugf();
	QString tmp;
	int i, k = -1;
	bool stop = false;
	PendingMsgs::Element elem;
	UserListElements users;
	QValueList<ChatMessage *> messages;

	for (i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if ((users.isEmpty() && elem.users.contains(user)) || (!users.isEmpty() && elem.users.equals(users)))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass)
			{
				if (users.isEmpty())
					users = elem.users;

				ChatMessage *msg = new ChatMessage(QString::null);
				k = openPendingMsg(i, *msg);
				messages.append(msg);

				--i;
				stop = true;
			}
	}
	if (stop)
		Chats[k]->scrollMessages(messages);
	else
		k = openChat("Gadu", selected_users, 0);
	kdebugf2();
}

void ChatManager::chatMsgReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool& grab)
{
	Chat *chat = findChat(senders);
	if (chat != NULL)
	{
		chat->newMessage(protocol->protocolID(), senders, msg, time);
		chat->alertNewMessage();
		grab = true;
	}
}

QVariant& ChatManager::getChatProperty(const UserGroup *group, const QString &name)
{
	kdebugf();
	FOREACH(addon, addons)
		if ((*addon).users == group)
		{
			kdebugf2();
			return (*addon).map[name];
		}
	ChatInfo info;
	info.users = group;
	info.map[name] = QVariant();
	addons.push_front(info);
	kdebugmf(KDEBUG_FUNCTION_END, "end: %s NOT found\n", name.local8Bit().data());
	return addons[0].map[name];
//	return addons[uins][name];
}

void ChatManager::setChatProperty(const UserGroup *group, const QString &name, const QVariant &value)
{
	kdebugf();
	FOREACH(addon, addons)
		if ((*addon).users == group)
		{
			(*addon).map[name] = value;
			kdebugf2();
			return;
		}
	ChatInfo info;
	info.users = group;
	info.map[name] = value;
	addons.push_front(info);
//	addons[uins][name]=value;
	kdebugf2();
}

void ChatManager::closeModule()
{
	delete chatslots;
	delete chat_manager;
}

void ChatManager::initModule()
{
	kdebugf();

	ConfigDialog::addTab("ShortCuts", "ShortCutsTab");
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", QT_TRANSLATE_NOOP("@default", "Define keys"));
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "New line / send message:"), "chat_newline", "Return");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Clear Chat:"), "chat_clear", "F9");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Close Chat:"), "chat_close", "Esc");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Bold text:"), "chat_bold", "Ctrl+B");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Italic text:"), "chat_italic", "Ctrl+I");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Underline text:"), "chat_underline", "Ctrl+U");

	ConfigDialog::addTab("Chat","ChatTab");
	ConfigDialog::addVGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Emoticons"), QString::null, Advanced);
	ConfigDialog::addComboBox("Chat", "Emoticons", QT_TRANSLATE_NOOP("@default", "Emoticons:"),
			"EmoticonsStyle", toStringList(tr("None"), tr("Static"), tr("Animated")), toStringList("0", "1", "2"), "2");

	ConfigDialog::addComboBox("Chat", "Emoticons", QT_TRANSLATE_NOOP("@default", "Emoticons theme"));

	ConfigDialog::addVGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "WWW options"), QString::null, Advanced);
	ConfigDialog::addComboBox("Chat", "WWW options", QT_TRANSLATE_NOOP("@default", "Choose your browser"));
	ConfigDialog::addComboBox("Chat", "WWW options", QT_TRANSLATE_NOOP("@default", "Browser options"));
	ConfigDialog::addLineEdit("Chat", "WWW options", QT_TRANSLATE_NOOP("@default", "Custom Web browser"), "WebBrowser", QString::null, QT_TRANSLATE_NOOP("@default", "%1 - Url clicked in chat window"));

	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Automatically prune chat messages"), "ChatPrune", true, QString::null, QString::null, Advanced);
	ConfigDialog::addHGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Message pruning"), QString::null, Advanced);
	ConfigDialog::addSpinBox("Chat", "Message pruning", QT_TRANSLATE_NOOP("@default", "Reduce the number of visible messages to"), "ChatPruneLen", 1,255,1,20);

	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Automatically fold links"), "FoldLink", false, QString::null, QString::null, Advanced);
	ConfigDialog::addHGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Link folding"), QString::null, Advanced);
	ConfigDialog::addSpinBox("Chat", "Link folding", QT_TRANSLATE_NOOP("@default", "Automatically fold links longer than"), "LinkFoldTreshold", 1,500,1,50);

	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Open chat window on new message"), "OpenChatOnMessage", false);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Scroll chat window downward, not upward"), "ScrollDown", true, QString::null, QString::null, Advanced);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "\"%1\" in chat sends message by default"), "AutoSend", true, QString::null, QString::null, Advanced);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Message acknowledgements (wait for delivery)"), "MessageAcks", true);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Flash chat title on new message"), "BlinkChatTitle", true, QString::null, QString::null, Advanced);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Ignore messages from anonymous users"), "IgnoreAnonymousUsers", false, QString::null, QString::null, Advanced);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Ignore richtext from anonymous users"), "IgnoreAnonymousRichtext", true, QString::null, QString::null, Advanced);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Receive images during invisibility"), "ReceiveImagesDuringInvisibility", true, QString::null, QString::null, Expert);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Block window close on new message"), "ChatCloseTimer", false, QString::null, QString::null, Advanced);
	ConfigDialog::addSpinBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Max time to block window close"),
			"ChatCloseTimerPeriod", 1, 5, 1, 2, QString::null, QString::null, Expert);

	ConfigDialog::addSpinBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Max image size"),
			"MaxImageSize", 0, 255, 5, 20);


// pierwsze uruchomienie kadu
	config_file.addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));

	//naglowki wiadomosci
	config_file.addVariable("Look", "NoHeaderRepeat", false);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "NoHeaderInterval", "10");

	config_file.addVariable("Look", "ChatFont", defaultFont);

	config_file.addVariable("Chat", "LastImagePath", QString(getenv("HOME"))+"/");

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"), "LookTab");

	ConfigDialog::addGrid("Look", "Look", "varOpts", 2);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show status button"), "ShowStatusButton", true, QString::null, QString::null, Advanced);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Multiline description in userbox"), "ShowMultilineDesc", true, QString::null, QString::null, Advanced);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Display group tabs"), "DisplayGroupTabs", true, QString::null, QString::null, Expert);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show available users in bold"), "ShowBold", true, QT_TRANSLATE_NOOP("@default","Displays users that are not offline using a bold font"), QString::null, Expert);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show description in userbox"), "ShowDesc", true);

	ConfigDialog::addVBox("Look", "Look", "varOpts2");//potrzebne userboksowi

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"), QString::null, Advanced);
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Chat window"));
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "Chat window background color"), "ChatBgColor", config_file.readColorEntry("Look","ChatBgColor"), QString::null, "bg_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "Text edit background color"), "ChatTextBgColor", config_file.readColorEntry("Look","ChatTextBgColor"), QString::null, "text_bg_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "Your background color"), "ChatMyBgColor", config_file.readColorEntry("Look","ChatMyBgColor"), QString::null, "own_bg_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "User background color"), "ChatUsrBgColor", config_file.readColorEntry("Look","ChatUsrBgColor"), QString::null, "his_bg_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "Your font color"), "ChatMyFontColor", config_file.readColorEntry("Look","ChatMyFontColor"), QString::null, "own_font_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "User font color"), "ChatUsrFontColor", config_file.readColorEntry("Look","ChatUsrFontColor"), QString::null, "his_font_color");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"), QString::null, Advanced);
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in chat window"), "ChatFont", defaultFont->toString(), QString::null, "chat_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Previews"), QString::null, Advanced);
		ConfigDialog::addVGroupBox("Look", "Previews", QT_TRANSLATE_NOOP("@default", "Chat preview"));
			ConfigDialog::addHBox("Look", "Chat preview", "chat_prvw");
				ConfigDialog::addLabel("Look", "chat_prvw", QT_TRANSLATE_NOOP("@default", "<b>Me</b> 00:00:00"), "chat_me");
				ConfigDialog::addLabel("Look", "chat_prvw", QT_TRANSLATE_NOOP("@default", "<b>Other party</b> 00:00:02"), "chat_other");

	//naglowki
	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Headers"), QString::null, Advanced);
		ConfigDialog::addCheckBox("Look", "Headers", QT_TRANSLATE_NOOP("@default", "Remove chat header repetitions"), "NoHeaderRepeat", true, QString::null, QString::null, Advanced);
		ConfigDialog::addSpinBox("Look", "Headers", QT_TRANSLATE_NOOP("@default", "Chat header separators height:"), "HeaderSeparatorHeight", 0, config_file.readNumEntry("General", "ParagraphSeparator"), 1, 1, QString::null, QString::null, Expert);
		ConfigDialog::addSpinBox("Look", "Headers", QT_TRANSLATE_NOOP("@default", "Interval between header removal:"), "NoHeaderInterval", 1, 1439, 1, 10, QString::null, QString::null, Expert);

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Other"), QString::null, Expert);
		ConfigDialog::addLineEdit("Look", "Other", QT_TRANSLATE_NOOP("@default", "Chat window title syntax:"), "ChatContents", QString::null, Kadu::SyntaxText, QString::null, Expert);
		ConfigDialog::addHBox("Look", "Other", "conference", QString::null, Expert);
			ConfigDialog::addLineEdit("Look", "conference", QT_TRANSLATE_NOOP("@default", "Conference window title prefix:"), "ConferencePrefix", QString::null, QT_TRANSLATE_NOOP("@default", "This text will be before syntax.\nIf you leave blank, default settings will be used."));
			ConfigDialog::addLineEdit("Look", "conference", QT_TRANSLATE_NOOP("@default", "syntax:"), "ConferenceContents", "%a (%s[: %d])", Kadu::SyntaxText);

	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	chatslots =new ChatManagerSlots(kadu, "chat_slots");
	ConfigDialog::registerSlotOnCreateTab("Chat", chatslots, SLOT(onCreateTabChat()));
	ConfigDialog::registerSlotOnCreateTab("Look", chatslots, SLOT(onCreateTabLook()));
	ConfigDialog::registerSlotOnApplyTab("Chat", chatslots,SLOT(onApplyTabChat()));
	ConfigDialog::registerSlotOnApplyTab("Look", chatslots,SLOT(onApplyTabLook()));
	ConfigDialog::connectSlot("Chat", "Emoticons:", SIGNAL(activated(int)), chatslots, SLOT(chooseEmoticonsStyle(int)));
	ConfigDialog::connectSlot("Chat", "Automatically prune chat messages", SIGNAL(toggled(bool)), chatslots, SLOT(onPruneChat(bool)));
	ConfigDialog::connectSlot("Chat", "Automatically fold links", SIGNAL(toggled(bool)), chatslots, SLOT(onFoldLink(bool)));
	ConfigDialog::connectSlot("Chat", "Block window close on new message", SIGNAL(toggled(bool)), chatslots, SLOT(onBlockClose(bool)));

	ConfigDialog::connectSlot("Look", "Remove chat header repetitions", SIGNAL(toggled(bool)), chatslots, SLOT(onRemoveHeaders(bool)));

	ConfigDialog::connectSlot("Look", "Your background color", SIGNAL(changed(const char *, const QColor&)),
		chatslots, SLOT(chooseColor(const char *, const QColor&)), "own_bg_color");
	ConfigDialog::connectSlot("Look", "User background color", SIGNAL(changed(const char *, const QColor&)),
		chatslots, SLOT(chooseColor(const char *, const QColor&)), "his_bg_color");
	ConfigDialog::connectSlot("Look", "Your font color", SIGNAL(changed(const char *, const QColor&)),
		chatslots, SLOT(chooseColor(const char *, const QColor&)), "own_font_color");
	ConfigDialog::connectSlot("Look", "User font color", SIGNAL(changed(const char *, const QColor&)),
		chatslots, SLOT(chooseColor(const char *, const QColor&)), "his_font_color");

	ConfigDialog::connectSlot("Look", "Font in chat window", SIGNAL(changed(const char *, const QFont&)), chatslots, SLOT(chooseFont(const char *, const QFont&)), "chat_font_box");

	chat_manager=new ChatManager(kadu, "chat_manager");
	connect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		chat_manager, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	kdebugf2();
}

ChatManager* chat_manager=NULL;
