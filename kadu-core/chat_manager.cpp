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
#include <qtooltip.h>

#include "action.h"
#include "debug.h"
#include "chat_manager.h"
#include "chat_widget.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_splitter.h"
#include "message_box.h"
#include "misc.h"
#include "pending_msgs.h"
#include "search.h"
#include "protocols_manager.h"
#include "userbox.h"
#include "ignore.h"

// TODO: make new icons for iconset instead of tricks like that
static QPixmap string_to_pixmap(const QString& str, const QFont& font)
{
	QSize size = QFontMetrics(font).size(0, str);
	QPixmap pixmap(size);
	pixmap.fill(kadu->palette().color(QPalette::Active, QColorGroup::Button));
	QPainter* painter = new QPainter();
	painter->begin(&pixmap);
	painter->setPen(kadu->palette().color(QPalette::Active, QColorGroup::ButtonText));
	painter->setFont(font);
	painter->drawText(QRect(QPoint(0, 0), size), 0, str);
	painter->end();
	pixmap.setMask(pixmap.createHeuristicMask());
	delete painter;
	return pixmap;
}

ChatManager::ChatManager(QObject* parent, const char* name)
	: QObject(parent, name), ChatWidgets(), addons(), refreshTitlesTimer()
{
	kdebugf();

	Action* auto_send_action = new Action(icons_manager->loadIcon("AutoSendMessage"),
		tr("%1 sends message").arg(config_file.readEntry("ShortCuts", "chat_newline")),
		"autoSendAction", Action::TypeChat);
	auto_send_action->setToggleAction(true);
	auto_send_action->setAllOn(config_file.readBoolEntry("Chat", "AutoSend"));
	connect(auto_send_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(autoSendActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("autoSendAction", auto_send_action);

	Action* scroll_lock_action = new Action(icons_manager->loadIcon("ScrollLock"),
		tr("Blocks scrolling"), "scrollLockAction", Action::TypeChat);
	scroll_lock_action->setToggleAction(true);
	connect(scroll_lock_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(scrollLockActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("scrollLockAction", scroll_lock_action);

	Action* clear_action = new Action(icons_manager->loadIcon("ClearChat"),
		tr("Clear messages in chat window"), "clearChatAction", Action::TypeChat);
	connect(clear_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(clearActionActivated(const UserGroup*)));
	KaduActions.insert("clearChatAction", clear_action);

	Action* insert_emot_action = new Action(icons_manager->loadIcon("ChooseEmoticon"),
		tr("Insert emoticon"), "insertEmoticonAction", Action::TypeChat);
	connect(insert_emot_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(insertEmoticonActionActivated(const UserGroup*, const QWidget*)));
	connect(insert_emot_action, SIGNAL(addedToToolbar(const UserGroup*, ToolButton*, ToolBar*)),
		this, SLOT(insertEmoticonActionAddedToToolbar(const UserGroup*, ToolButton*, ToolBar*)));
	KaduActions.insert("insertEmoticonAction", insert_emot_action);

	Action* whois_action = new Action(icons_manager->loadIcon("LookupUserInfo"),
		tr("Search this user in directory"), "whoisAction", Action::TypeChat);
	connect(whois_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(whoisActionActivated(const UserGroup*)));
	KaduActions.insert("whoisAction", whois_action);

	Action* insert_img_action = new Action(icons_manager->loadIcon("ChooseImage"),
		tr("Insert image"), "insertImageAction", Action::TypeChat);
	connect(insert_img_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(insertImageActionActivated(const UserGroup*)));
	KaduActions.insert("insertImageAction", insert_img_action);

	Action* ignore_user_action = new Action(icons_manager->loadIcon("ManageIgnored"),
		tr("Ignore user"), "ignoreUserAction", Action::TypeUser);
	connect(ignore_user_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(ignoreUserActionActivated(const UserGroup*)));
	KaduActions.insert("ignoreUserAction", ignore_user_action);

	Action* block_user_action = new Action(icons_manager->loadIcon("Blocking"),
		tr("Block user"), "blockUserAction", Action::TypeUser);
	connect(block_user_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(blockUserActionActivated(const UserGroup*)));
	KaduActions.insert("blockUserAction", block_user_action);

	QFont font;

	font.setBold(true);
	Action* bold_action = new Action(string_to_pixmap("B", font),
		tr("Bold"), "boldAction", Action::TypeChat);
	bold_action->setToggleAction(true);
	connect(bold_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(boldActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("boldAction", bold_action);

	font.setBold(false);
	font.setItalic(true);
	Action* italic_action = new Action(string_to_pixmap("I", font),
		tr("Italic"), "italicAction", Action::TypeChat);
	italic_action->setToggleAction(true);
	connect(italic_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(italicActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("italicAction", italic_action);

	font.setItalic(false);
	font.setUnderline(true);
	Action* underline_action = new Action(string_to_pixmap("U", font),
		tr("Underline"), "underlineAction", Action::TypeChat);
	underline_action->setToggleAction(true);
	connect(underline_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(underlineActionActivated(const UserGroup*, const QWidget*, bool)));
	KaduActions.insert("underlineAction", underline_action);

	QPixmap p(12, 12);
	p.fill(Qt::black);
	Action* color_action = new Action(p,
		tr("Change color"), "colorAction", Action::TypeChat);
	connect(color_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(colorActionActivated(const UserGroup*, const QWidget*)));
	KaduActions.insert("colorAction", color_action);

	Action* send_action = new Action(icons_manager->loadIcon("SendMessage"),
		tr("&Send"), "sendAction", Action::TypeChat);
	connect(send_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(sendActionActivated(const UserGroup*)));
	KaduActions.insert("sendAction", send_action);

	Action* chat_action = new Action(icons_manager->loadIcon("OpenChat"),
		tr("&Chat"), "chatAction", Action::TypeUserList);
	connect(chat_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(chatActionActivated(const UserGroup*)));
	KaduActions.insert("chatAction", chat_action);

	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "autoSendAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "scrollLockAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "clearChatAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "insertEmoticonAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "whoisAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "insertImageAction");

	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "boldAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "italicAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "underlineAction");
	KaduActions.addDefaultToolbarAction("Chat toolbar 2", "colorAction");

	KaduActions.addDefaultToolbarAction("Chat toolbar 3", "sendAction", -1, true);

	KaduActions.addDefaultToolbarAction("Search toolbar", "chatAction", -1, true);

	if (config_file.readBoolEntry("Chat", "RememberPosition"))
	{
		userlist->addPerContactNonProtocolConfigEntry("chat_geometry", "ChatGeometry");
		userlist->addPerContactNonProtocolConfigEntry("chat_vertical_sizes", "VerticalSizes");
	}
	connect(&refreshTitlesTimer, SIGNAL(timeout()), this, SLOT(refreshTitles()));
	connect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshTitlesLater()));

	kdebugf2();
}

void ChatManager::closeAllWindows()
{
	kdebugf();
	while (!ChatWidgets.empty())
	{
		ChatWidget *chat=ChatWidgets.first();
		delete chat;
	}
	kdebugf2();
}

void ChatManager::loadOpenedWindows()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->findElement(root_elem, "ChatWindows");
	if (!chats_elem.isNull())
	{
		for (QDomNode win = chats_elem.firstChild(); !win.isNull(); win = win.nextSibling())
		{
			const QDomElement &window_elem = win.toElement();
			if (window_elem.isNull())
				continue;
			if (window_elem.tagName() != "Window")
				continue;
			QString protocolId = window_elem.attribute("protocol");
			QString accountId = window_elem.attribute("id");
			UserListElements users;
			for (QDomNode contact = window_elem.firstChild(); !contact.isNull(); contact = contact.nextSibling())
			{
				const QDomElement &contact_elem = contact.toElement();
				if (contact_elem.isNull())
					continue;
				if (contact_elem.tagName() != "Contact")
					continue;
				QString id = contact_elem.attribute("id");
				users.append(userlist->byID(protocolId, id));
			}
			Protocol *protocol = protocols_manager->byID(protocolId, accountId);
			if (protocol)
				openChatWidget(protocol, users);
			else
				kdebugm(KDEBUG_WARNING, "protocol %s/%s not found!\n", protocolId.local8Bit().data(), accountId.local8Bit().data());
		}
	}
	kdebugf2();
}

void ChatManager::saveOpenedWindows()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->accessElement(root_elem, "ChatWindows");
	xml_config_file->removeChildren(chats_elem);
	CONST_FOREACH(chat, ChatWidgets)
	{
		QDomElement window_elem = xml_config_file->createElement(chats_elem, "Window");
		Protocol *protocol = (*chat)->currentProtocol();
		QString protoId = protocol->protocolID();
		window_elem.setAttribute("protocol", protoId);
		window_elem.setAttribute("id", protocol->ID());
		const UserGroup *users = (*chat)->users();
		CONST_FOREACH(user, *users)
		{
			QDomElement user_elem = xml_config_file->createElement(window_elem, "Contact");
 			user_elem.setAttribute("id", (*user).ID(protoId));
		}
	}
	kdebugf2();
}

ChatManager::~ChatManager()
{
	kdebugf();
	disconnect(&refreshTitlesTimer, SIGNAL(timeout()), this, SLOT(refreshTitles()));
	disconnect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshTitlesLater()));

	closeAllWindows();

#if DEBUG_ENABLED
	// for valgrind
	QStringList chatActions;
	chatActions << "autoSendAction" << "scrollLockAction" << "clearChatAction"
				<< "insertEmoticonAction" << "whoisAction"
				<< "insertImageAction" << "ignoreUserAction" << "blockUserAction"
				<< "boldAction" << "italicAction" << "underlineAction"
				<< "colorAction" << "sendAction" << "chatAction";
	CONST_FOREACH(act, chatActions)
	{
		Action *a = KaduActions[*act];
		KaduActions.remove(*act);
		delete a;
	}
#endif

	kdebugf2();
}

void ChatManager::autoSendActionActivated(const UserGroup* users, const QWidget* /*source*/, bool is_on)
{
	kdebugf();
	findChatWidget(users)->setAutoSend(is_on);
	KaduActions["autoSendAction"]->setAllOn(is_on);
	config_file.writeEntry("Chat", "AutoSend", is_on);
	kdebugf2();
}

void ChatManager::scrollLockActionActivated(const UserGroup* users, const QWidget* /*source*/, bool is_on)
{
	kdebugf();
	findChatWidget(users)->setScrollLocked(is_on);
	kdebugf2();
}

void ChatManager::clearActionActivated(const UserGroup* users)
{
	kdebugf();
	findChatWidget(users)->clearChatWindow();
	kdebugf2();
}

void ChatManager::boldActionActivated(const UserGroup* users, const QWidget* /*source*/, bool is_on)
{
	kdebugf();
	findChatWidget(users)->edit()->setBold(is_on);
	kdebugf2();
}

void ChatManager::italicActionActivated(const UserGroup* users, const QWidget* /*source*/, bool is_on)
{
	kdebugf();
	findChatWidget(users)->edit()->setItalic(is_on);
	kdebugf2();
}

void ChatManager::underlineActionActivated(const UserGroup* users, const QWidget* /*source*/, bool is_on)
{
	kdebugf();
	findChatWidget(users)->edit()->setUnderline(is_on);
	kdebugf2();
}

void ChatManager::colorActionActivated(const UserGroup* users, const QWidget* source)
{
	kdebugf();
	findChatWidget(users)->changeColor(source);
	kdebugf2();
}

void ChatManager::insertEmoticonActionActivated(const UserGroup* users, const QWidget* source)
{
	kdebugf();
	findChatWidget(users)->openEmoticonSelector(source);
	kdebugf2();
}

void ChatManager::insertEmoticonActionAddedToToolbar(const UserGroup* /*users*/,
	ToolButton* button, ToolBar* /*toolbar*/)
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
		(new SearchDialog())->show();
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

void ChatManager::ignoreUserActionActivated(const UserGroup* users)
{
	kdebugf();
	if (users->count() > 0)
	{
		bool ContainsBad = false;
		QString MyGGUIN = QString::number(config_file.readUnsignedNumEntry("General", "UIN"));

		CONST_FOREACH(user, (*users))
		{
			if (!(*user).usesProtocol("Gadu") || (*user).ID("Gadu") == MyGGUIN)
			{
				ContainsBad = true;
				break;
			}
		}

		if (!ContainsBad)
		{
			UserListElements u = users->toUserListElements();
			if (isIgnored(u))
				delIgnored(u);
			else
			{
				addIgnored(u);
				ChatWidget *c = findChatWidget(u);
				if (c)
					c->close();
			}
			kadu->userbox()->refresh();
			writeIgnored();
		}
	}
	kdebugf2();
}

void ChatManager::blockUserActionActivated(const UserGroup* users)
{
	kdebugf();
	if (users->count() > 0)
	{
		bool on = true;
		bool blocked_anonymous = false; // true, if we blocked at least one anonymous user
		QString MyGGUIN = QString::number(config_file.readUnsignedNumEntry("General", "UIN"));

		CONST_FOREACH(user, (*users))
			if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "Blocking").toBool())
			{
				on = false;
				break;
			}

		FOREACH(user, (*users))
			if ((*user).usesProtocol("Gadu") && (*user).ID("Gadu") != MyGGUIN && (*user).protocolData("Gadu", "Blocking").toBool() != !on)
			{
				(*user).setProtocolData("Gadu", "Blocking", !on);
				if ((!on) && (!blocked_anonymous) && ((*user).isAnonymous()))
					blocked_anonymous = true;
			}

		if (!on) // if we were blocking, we also close the chat (and show info if blocked anonymous)
		{
			if (blocked_anonymous)
				MessageBox::msg(tr("Anonymous users will be unblocked after restarting Kadu"), false, "Information", kadu);

			UserListElements u = users->toUserListElements();
			ChatWidget *c = findChatWidget(u);
			if (c)
				c->close();
		}

		userlist->writeToConfig();
	}
	kdebugf2();
}

void ChatManager::insertImageActionActivated(const UserGroup* users)
{
	kdebugf();
	findChatWidget(users)->insertImage();
	kdebugf2();
}

void ChatManager::sendActionActivated(const UserGroup* users)
{
	kdebugf();
	ChatWidget* chat = findChatWidget(users);
	if (chat->waitingForACK())
		chat->cancelMessage();
	else
		chat->sendMessage();
	kdebugf2();
}

void ChatManager::chatActionActivated(const UserGroup* users)
{
	kdebugf();
	if (users->count() > 0)
	{
		bool ContainsBad = false;
		QString MyGGUIN = QString::number(config_file.readUnsignedNumEntry("General", "UIN"));

		CONST_FOREACH(user, (*users))
		{
			if (!(*user).usesProtocol("Gadu") || (*user).ID("Gadu") == MyGGUIN)
			{
				ContainsBad = true;
				break;
			}
		}

		if (!ContainsBad)
			openChatWidget(gadu, users->toUserListElements(), 0);
	}
	kdebugf2();
}

const ChatList& ChatManager::chats() const
{
	return ChatWidgets;
}

int ChatManager::registerChatWidget(ChatWidget* chat)
{
	ChatWidgets.append(chat);
	return ChatWidgets.count() - 1;
}

void ChatManager::unregisterChatWidget(ChatWidget* chat)
{
	kdebugf();
	FOREACH(curChat, ChatWidgets)
		if (*curChat == chat)
		{
			emit chatWidgetDestroying(chat);
			ChatWidgets.remove(curChat);
			kdebugf2();
			return;
		}
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "NOT found\n");
}

void ChatManager::refreshTitlesLater()
{
	refreshTitlesTimer.start(0, true);
}

void ChatManager::refreshTitles()
{
	kdebugf();
 	CONST_FOREACH(chat, ChatWidgets)
		(*chat)->refreshTitle();
	emit chatWidgetTitlesUpdated();
	kdebugf2();
}

void ChatManager::refreshTitlesForUser(UserListElement user)
{
	kdebugf();
 	CONST_FOREACH(chat, ChatWidgets)
 		if ((*chat)->users()->contains(user))
 			(*chat)->refreshTitle();
	kdebugf2();
}

void ChatManager::changeAppearance()
{
	kdebugf();
 	CONST_FOREACH(chat, ChatWidgets)
 		(*chat)->changeAppearance();
	kdebugf2();
}

ChatWidget* ChatManager::findChatWidget(const UserGroup *group) const
{
	CONST_FOREACH(chat, ChatWidgets)
		if ((*chat)->users() == group)
			return *chat;
	kdebugmf(KDEBUG_WARNING, "no such chat\n");
	return NULL;
}

ChatWidget* ChatManager::findChatWidget(UserListElements users) const
{
	CONST_FOREACH(chat, ChatWidgets)
		if (users.equals((*chat)->users()))
			return *chat;
	kdebugmf(KDEBUG_WARNING, "no such chat\n");
	return NULL;
}

int ChatManager::openChatWidget(Protocol *initialProtocol, const UserListElements &users, time_t time)
{
	kdebugf();

	int myUin = config_file.readNumEntry("General", "UIN");

	CONST_FOREACH(user, users)
		if ((*user).ID("Gadu") == myUin)
		{
			kdebugf2();
			return -1;
		}

	unsigned int i = 0;
	CONST_FOREACH(chat, ChatWidgets)
	{
		if ((*chat)->users()->equals(users))
		{
			QWidget *win = *chat;
			kdebugm(KDEBUG_INFO, "parent: %p\n", win->parent());
			while (win->parent()) // for tabs module
			{
				kdebugm(KDEBUG_INFO, "parent type: %s\n", win->parent()->className());
				win = static_cast<QWidget *>(win->parent());
			}
#if QT_VERSION >= 0x030300
			if (qstrcmp(qVersion(), "3.3") >= 0) // dodatkowe zabezpieczenie przed idiotami u¿ywaj±cymi opcji --force przy instalacji pakietów
				win->setWindowState(win->windowState() & ~WindowMinimized | WindowActive);
#endif
			win->raise();
			(*chat)->makeActive();
			emit chatWidgetOpen(*chat);
			return i;
		}
		++i;
	}
	QStringList userNames;
	CONST_FOREACH(user, users)
		userNames.append((*user).altNick());
	userNames.sort();
	ChatWidget* chat = new ChatWidget(initialProtocol, users, 0, QString("chat:%1").arg(userNames.join(",")).local8Bit().data());
	ChatWindow* window = new ChatWindow(chat,0,QString("chat:%1").arg(userNames.join(",")).local8Bit().data());
	window->setCentralWidget(chat);
  	chat->reparent(window,0, QPoint(),true);
 	chat->refreshTitle();
	connect(chat, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)));
	connect(chat, SIGNAL(chatWidgetActivated(ChatWidget *)), this, SIGNAL(chatWidgetActivated(ChatWidget *)));

	window->show();
	//chat->show();
//	chat->makeActive();
	emit chatWidgetCreated(chat);
	emit chatWidgetCreated(chat, time);
	emit chatWidgetOpen(chat);
	kdebugf2();
	return ChatWidgets.count() - 1;
}

ChatMessage * ChatManager::openPendingMsg(int index, int &k)
{
	kdebugf();
	// TODO: check if index does not exceed boundaries
	PendingMsgs::Element p = pending[index];
	// opening chat (if it does not exist)
	k = openChatWidget(gadu, p.users, p.time);
	// appending new message

	if (k < 0)
		return 0;

	QDateTime date;
	date.setTime_t(p.time);

	ChatMessage *msg = new ChatMessage(p.users[0], p.msg, false, QDateTime::currentDateTime(), date);

	// remove message from pending
	pending.deleteMsg(index);
	kdebugf2();
	// return index of opened chat window
	return msg;
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

	UserBox::refreshAllLater();
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
				ChatMessage *msg = openPendingMsg(i, k);

				if (k < 0)
					return;

				messages.append(msg);

				--i;
				users = elem.users;
				stop = true;
			}
	}
	if (stop)
	{
		ChatWidgets[k]->scrollMessages(messages);
		UserBox::refreshAllLater();
	}
	else
		k = openChatWidget(gadu, users, 0);
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

				ChatMessage *msg = openPendingMsg(i, k);

				if (k < 0)
					return;

				messages.append(msg);

				--i;
				stop = true;
			}
	}
	if (stop)
	{
		kdebugmf(KDEBUG_INFO, "stopped\n");
		ChatWidgets[k]->scrollMessages(messages);
		UserBox::refreshAllLater();
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

				ChatMessage *msg = openPendingMsg(i, k);

				if (k < 0)
					return;

				messages.append(msg);

				--i;
				stop = true;
			}
	}
	if (stop)
	{
		ChatWidgets[k]->scrollMessages(messages);
		UserBox::refreshAllLater();
	}
	else
		k = openChatWidget(gadu, selected_users, 0);
	kdebugf2();
}

void ChatManager::chatMsgReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool& grab)
{
	ChatWidget *chat = findChatWidget(senders);
	if (chat != NULL)
	{
		chat->newMessage(protocol->protocolID(), senders, msg, time);
		grab = true;
	}
}

QVariant& ChatManager::getChatWidgetProperty(const UserGroup *group, const QString &name)
{
	kdebugf();
	FOREACH(addon, addons)
		if (group->equals((*addon).users))
		{
			kdebugf2();
			return (*addon).map[name];
		}
	ChatInfo info;
	info.users = group->toUserListElements();
	info.map[name] = QVariant();
	addons.push_front(info);
	kdebugmf(KDEBUG_FUNCTION_END, "end: %s NOT found\n", name.local8Bit().data());
	return addons[0].map[name];
}

void ChatManager::setChatWidgetProperty(const UserGroup *group, const QString &name, const QVariant &value)
{
	kdebugf();
	FOREACH(addon, addons)
		if (group->equals((*addon).users))
		{
			(*addon).map[name] = value;
			kdebugf2();
			return;
		}
	ChatInfo info;
	info.users = group->toUserListElements();
	info.map[name] = value;
	addons.push_front(info);
	kdebugf2();
}

void ChatManager::closeModule()
{
	kdebugf();

	ChatMessage::unregisterParserTags();

	disconnect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		chat_manager, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));
	chat_manager->saveOpenedWindows();

	delete chat_manager;
	chat_manager = 0;
	kdebugf2();
}

void ChatManager::initModule()
{
	kdebugf();

	ChatMessage::registerParserTags();

// pierwsze uruchomienie kadu
	config_file.addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrNickColor", QColor("#000000"));

	//naglowki wiadomosci
	config_file.addVariable("Look", "NoHeaderRepeat", false);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "NoHeaderInterval", "10");

	//czas serwera
	config_file.addVariable("Look", "NoServerTime", false);
	config_file.addVariable("Look", "NoServerTimeDiff", 5);

	config_file.addVariable("Look", "ChatFont", defaultFont);

	config_file.addVariable("Chat", "LastImagePath", QString(getenv("HOME")) + '/');

	config_file.addVariable("Look", "Style", "kadu");

// 	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default","Style"));
// 		ConfigDialog::addComboBox("Look", "Style", QT_TRANSLATE_NOOP("@default", "Select chat style"), "Style", toStringList("Kadu", "Hapi", "IRC", tr("Custom")), toStringList("kadu", "hapi", "irc", "custom"));
// 		ConfigDialog::addTextEdit("Look", "Style", QT_TRANSLATE_NOOP("@default", "Full chat style:"), "FullStyle", 0,
// 			QT_TRANSLATE_NOOP("@default", "Syntax:\n%1 - background color\n%2 - text font color\n%3 - nick color\n%4 - nick\n%5 - timestamp\n%6 - timestamp with server time\n%7 - message"), 0, Expert);

	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	chat_manager = new ChatManager(kadu, "chat_manager");
	connect(gadu, SIGNAL(chatMsgReceived1(Protocol *, UserListElements, const QString&, time_t, bool&)),
		chat_manager, SLOT(chatMsgReceived(Protocol *, UserListElements, const QString&, time_t, bool&)));

	kdebugf2();
}

void ChatManager::configurationUpdated()
{
	kdebugf();

	changeAppearance();

	if (config_file.readBoolEntry("Chat", "RememberPosition"))
	{
		userlist->addPerContactNonProtocolConfigEntry("chat_geometry", "ChatGeometry");
		userlist->addPerContactNonProtocolConfigEntry("chat_vertical_sizes", "VerticalSizes");
	}
	else
	{
		userlist->removePerContactNonProtocolConfigEntry("chat_geometry");
		userlist->removePerContactNonProtocolConfigEntry("chat_vertical_sizes");
	}

// 	bool msgTitle = config_file.readBoolEntry("Chat","NewMessagesInChatTitle");
// 	bool blnTitle = config_file.readBoolEntry("Chat","BlinkChatTitle");

	KaduActions["autoSendAction"]->setAllOn(config_file.readBoolEntry("Chat", "AutoSend"));

	kdebugf2();
}

ChatManager* chat_manager=NULL;
