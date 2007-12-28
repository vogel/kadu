/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qaccel.h>
#include <qdragobject.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qvbox.h>

#include "action.h"
#include "chat_widget.h"
#include "chat_manager.h"
#include "chat_message.h"
#include "color_selector.h"
#include "custom_input.h"
#include "debug.h"
#include "dockarea.h"
#include "gadu_images_manager.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "kadu_splitter.h"
#include "kadu_text_browser.h"
#include "message_box.h"
#include "misc.h"
#include "protocol.h"
#include "search.h"
#include "syntax_editor.h"
#include "userbox.h"

ChatWidget::ChatWidget(Protocol *initialProtocol, const UserListElements &usrs, QWidget* parent, const char* name)
	: QHBox(parent, name), CurrentProtocol(initialProtocol),
	Users(new UserGroup(usrs)),
	index(0), actcolor(), Edit(0),
	bodyformat(new QMimeSourceFactory()), emoticon_selector(0), color_selector(0),
	WaitingForACK(false), userbox(0), vertSplit(0), horizSplit(0),
	activationCount(0), NewMessagesCount(0)
{
	kdebugf();
	const int minimumDockAreaSize = 3;
	QValueList<int> sizes;

	setAcceptDrops(true);
	/* register us in the chats registry... */
	index=chat_manager->registerChatWidget(this);
	DockArea *leftDockArea = new DockArea(Qt::Vertical, DockArea::Normal, this,
		"chatLeftDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(leftDockArea, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	// TODO: fix this workaround
	connect(leftDockArea, SIGNAL(toolbarAttached()),
		this, SLOT(editTextChanged()));
	leftDockArea->setMinimumWidth(minimumDockAreaSize);

	QVBox *central = new QVBox(this, "central");
	this->setStretchFactor(central, 50);

	DockArea *rightDockArea = new DockArea(Qt::Vertical, DockArea::Normal, this,
		"chatRightDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(rightDockArea, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	// TODO: fix this workaround
	connect(rightDockArea, SIGNAL(toolbarAttached()),
		this, SLOT(editTextChanged()));
	rightDockArea->setMinimumWidth(minimumDockAreaSize);

	vertSplit = new KaduSplitter(Qt::Vertical, central, "vertSplit");

	QVBox *topArea = new QVBox(vertSplit, "topArea");
	DockArea *topDockArea = new DockArea(Qt::Horizontal, DockArea::Normal, topArea,
		"chatTopDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(topDockArea, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	// TODO: fix this workaround
	connect(topDockArea, SIGNAL(toolbarAttached()),
		this, SLOT(editTextChanged()));
	topDockArea->setMinimumHeight(minimumDockAreaSize);

	if (Users->count() > 1)
	{
		horizSplit = new KaduSplitter(Qt::Horizontal, topArea, "horizSplit");
		horizSplit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

		body = new ChatMessagesView(horizSplit, "body");

		QVBox *userlistContainer = new QVBox(horizSplit);

		userbox = new UserBox(false, Users, userlistContainer, "userbox");
		userbox->setMinimumSize(QSize(30,30));
		connect(userbox, SIGNAL(rightButtonPressed(QListBoxItem *, const QPoint &)),
			UserBox::userboxmenu, SLOT(show(QListBoxItem *)));

		QPushButton *leaveConference = new QPushButton(tr("Leave conference"), userlistContainer);
		connect(leaveConference, SIGNAL(clicked()), this, SLOT(leaveConference()));

		sizes.append(3);
		sizes.append(1);
		horizSplit->setSizes(sizes);
	}
	else
		body = new ChatMessagesView(topArea, "body");

	QVBox *downpart = new QVBox(vertSplit, "downpartBox");
	QHBox *edtbuttontray = new QHBox(downpart, "edtbuttontrayBox");

	vertSplit->setResizeMode(downpart, QSplitter::KeepSize);

	if (config_file.readBoolEntry("Chat", "ShowEditWindowLabel", true))
	{
		QLabel *edt = new QLabel(tr("Edit window:"), edtbuttontray, "editLabel");
		QToolTip::add(edt, tr("This is where you type in the text to be sent"));
		edtbuttontray->setStretchFactor(edt, 1);
	}

	DockArea* buttontray = new DockArea(Qt::Horizontal, DockArea::Normal, edtbuttontray,
		"chatMiddleDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(buttontray, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	// TODO: fix this workaround
	connect(buttontray, SIGNAL(toolbarAttached()),
		this, SLOT(editTextChanged()));
	buttontray->setMinimumHeight(minimumDockAreaSize);
	edtbuttontray->setStretchFactor(buttontray, 50);

	Edit = new CustomInput(downpart, "edit");
 	Edit->setMinimumHeight(1);
	Edit->setWordWrap(QMultiLineEdit::WidgetWidth);

	connect(Edit, SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)), this, SLOT(keyPressedSlot(QKeyEvent *, CustomInput *, bool &)));

	setFocusProxy(Edit);

	DockArea* btnpart = new DockArea(Qt::Horizontal, DockArea::Normal, downpart,
		"chatBottomDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(btnpart, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	// TODO: fix this workaround
	connect(btnpart, SIGNAL(toolbarAttached()),
		this, SLOT(editTextChanged()));
	btnpart->setMinimumHeight(minimumDockAreaSize);

	QAccel *acc = new QAccel(this, "returnAccel");
	acc->connectItem(acc->insertItem(Key_Return + CTRL), this, SLOT(sendMessage()));

	acc = new QAccel(this, "pageUpAccel");
	acc->connectItem(acc->insertItem(Key_PageUp + SHIFT), body, SLOT(pageUp()));
	acc = new QAccel(this, "pageDownAccel");
	acc->connectItem(acc->insertItem(Key_PageDown + SHIFT), body, SLOT(pageDown()));

	topDockArea->loadFromConfig(this);
	leftDockArea->loadFromConfig(this);
	rightDockArea->loadFromConfig(this);

	if (userbox)
	{
		connect(userbox, SIGNAL(selectionChanged()), topDockArea, SLOT(usersChangedSlot()));
		connect(userbox, SIGNAL(selectionChanged()), rightDockArea, SLOT(usersChangedSlot()));
		connect(userbox, SIGNAL(selectionChanged()), leftDockArea, SLOT(usersChangedSlot()));
		connect(userbox, SIGNAL(selectionChanged()), buttontray, SLOT(usersChangedSlot()));
		connect(userbox, SIGNAL(selectionChanged()), btnpart, SLOT(usersChangedSlot()));
	}

	if (!buttontray->loadFromConfig(this))
	{
		// TOOLBAR 1
		ToolBar* tb1 = new ToolBar(this, "Chat toolbar 1");
		tb1->setOffset(10000);
		tb1->show();
		buttontray->moveDockWindow(tb1);
		buttontray->setAcceptDockWindow(tb1, true);
		tb1->loadDefault();
	}

	if (!btnpart->loadFromConfig(this))
	{
		// TOOLBAR2
		ToolBar* tb2 = new ToolBar(this, "Chat toolbar 2");
		tb2->show();
		btnpart->moveDockWindow(tb2);
		btnpart->setAcceptDockWindow(tb2, true);
		tb2->loadDefault();
		// TOOLBAR 3
		ToolBar* tb3 = new ToolBar(this, "Chat toolbar 3");
		tb3->setOffset(10000);
		tb3->show();
		btnpart->moveDockWindow(tb3);
		btnpart->setAcceptDockWindow(tb3, true);
		tb3->loadDefault();
	}

	Edit->setMimeSourceFactory(bodyformat);
	Edit->setTextFormat(Qt::RichText);

	connect(Edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(curPosChanged(int, int)));
	connect(Edit, SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(Edit, SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));
	connect(Edit, SIGNAL(textChanged()), this, SLOT(editTextChanged()));

	editTextChanged(); // slot ustawia poprawny stan przycisku Send (tutaj - blokuje)
	setActColor(false); // ustawia poprawny kolor na przycisku wyboru koloru

	connect(KaduActions["sendAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(sendActionAddedToToolbar(ToolButton*, ToolBar*)));
	connect(KaduActions["colorAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(colorActionForceRefresh()));
	connect(KaduActions["colorAction"], SIGNAL(iconsRefreshed()),
		this, SLOT(colorActionForceRefresh()));

	Edit->installEventFilter(this);

	configurationUpdated();

	kdebugf2();
}

ChatWidget::~ChatWidget()
{
	kdebugf();

	chat_manager->unregisterChatWidget(this);

	disconnectAcknowledgeSlots();
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		body, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));


	if (userbox)
		delete userbox;
	delete bodyformat;
	delete Users;

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed: index %d\n", index);
}

void ChatWidget::configurationUpdated()
{
	if (config_file.readBoolEntry("Chat", "ChatPrune"))
		body->setPrune(config_file.readUnsignedNumEntry("Chat", "ChatPruneLen"));
	else
		body->setPrune(0);

	if (userbox)
	{
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
	}

	Edit->setFont(config_file.readFontEntry("Look","ChatFont"));
	Edit->setPaper(QBrush(config_file.readColorEntry("Look","ChatTextBgColor")));
	AutoSend = config_file.readBoolEntry("Chat", "AutoSend");
	Edit->setAutosend(AutoSend);

	refreshTitle();
}

void ChatWidget::specialKeyPressed(int key)
{
	kdebugf();
	switch (key)
	{
		case CustomInput::KEY_BOLD:
			KaduActions["boldAction"]->setOn(Users->toUserListElements(), !KaduActions["boldAction"]->isOn(Users->toUserListElements()));
			Edit->setBold(KaduActions["boldAction"]->isOn(Users->toUserListElements()));
			break;
		case CustomInput::KEY_ITALIC:
			KaduActions["italicAction"]->setOn(Users->toUserListElements(), !KaduActions["italicAction"]->isOn(Users->toUserListElements()));
			Edit->setItalic(KaduActions["italicAction"]->isOn(Users->toUserListElements()));
			break;
		case CustomInput::KEY_UNDERLINE:
			KaduActions["underlineAction"]->setOn(Users->toUserListElements(), !KaduActions["underlineAction"]->isOn(Users->toUserListElements()));
			Edit->setUnderline(KaduActions["underlineAction"]->isOn(Users->toUserListElements()));
			break;
	}
	kdebugf2();
}

void ChatWidget::curPosChanged(int, int)
{
	kdebugf();

	UserListElements elems = Users->toUserListElements();
	if (Edit->bold() != KaduActions["boldAction"]->isOn(elems))
		KaduActions["boldAction"]->setOn(elems, Edit->bold());
	if (Edit->italic() != KaduActions["italicAction"]->isOn(elems))
		KaduActions["italicAction"]->setOn(elems, Edit->italic());
	if (Edit->underline() != KaduActions["underlineAction"]->isOn(elems))
		KaduActions["underlineAction"]->setOn(elems, Edit->underline());

	setActColor(false);

	kdebugf2();
}

void ChatWidget::setActColor(bool force)
{
	kdebugf();

	if (force || (Edit->color() != actcolor))
	{
		int i;
		for (i = 0; i < 16; ++i)
			if (Edit->color() == QColor(colors[i]))
				break;
		QPixmap p(12, 12);
		if (i >= 16)
			actcolor = Edit->paletteForegroundColor();
		else
			actcolor = colors[i];
		p.fill(actcolor);

		QIconSet icon;
		icon.setPixmap(p, QIconSet::Automatic, QIconSet::Normal);
		icon.setPixmap(icons_manager->loadIcon("Black_dis"), QIconSet::Automatic, QIconSet::Disabled);

		KaduActions["colorAction"]->setIconSets(Users->toUserListElements(), icon);
	}

	kdebugf2();
}

void ChatWidget::insertImage()
{
	kdebugf();
	ImageDialog* id = new ImageDialog(this);
	id->setDir(config_file.readEntry("Chatt", "LastImagePath"));
	id->setCaption(tr("Insert image"));
	if (id->exec() == QDialog::Accepted)
	{
		config_file.writeEntry("Chatt", "LastImagePath", id->dirPath());
		QString selectedFile = id->selectedFile();
		QFileInfo f(selectedFile);
		delete id;id = NULL;
		if (!f.isReadable())
		{
			MessageBox::msg(tr("This file is not readable"), true, "Warning", this);
			QTimer::singleShot(0, this, SLOT(insertImage()));
			kdebugf2();
			return;
		}

		if (f.size() >= (1 << 18)) // 256kB
		{
			MessageBox::msg(tr("This file is too big (%1 >= %2)").arg(f.size()).arg(1<<18), true, "Warning", this);
			QTimer::singleShot(0, this, SLOT(insertImage()));
			kdebugf2();
			return;
		}

		int counter = 0;
		CONST_FOREACH(user, *Users)
		{
			if ((*user).usesProtocol("Gadu"))//TODO: user.hasFeature("ImageSending")
			{
				unsigned int maximagesize = (*user).protocolData("Gadu", "MaxImageSize").toUInt();
				if (f.size() >= maximagesize * 1024)
					++counter;
			}
			else
				++counter;
		}
		if (counter == 1 && Users->count() == 1)
		{
			if (!MessageBox::ask(tr("This file is too big for %1.\nDo you really want to send this image?\n").arg((*Users->constBegin()).altNick())))
			{
				QTimer::singleShot(0, this, SLOT(insertImage()));
				kdebugf2();
				return;
			}
		}
		else if	(counter > 0 &&
			!MessageBox::ask(tr("This file is too big for %1 of %2 contacts.\nDo you really want to send this image?\nSome of them probably will not get it.").arg(counter).arg(Users->count())))
		{
			QTimer::singleShot(0, this, SLOT(insertImage()));
			kdebugf2();
			return;
		}
		Edit->insert(QString("[IMAGE ") + selectedFile + ']');
	}
	else
		delete id;

	kdebugf2();
}

void ChatWidget::sendActionAddedToToolbar(ToolButton* button, ToolBar* /*toolbar*/)
{
	kdebugf();
	button->setEnabled(!Edit->text().isEmpty());
	kdebugf2();
}

void ChatWidget::colorActionForceRefresh()
{
	kdebugf();
	setActColor(true);
	kdebugf2();
}

void ChatWidget::refreshTitle()
{
	kdebugf();
	QString title;

	int uinsSize = Users->count();
	kdebugmf(KDEBUG_FUNCTION_START, "Uins.size() = %d\n", uinsSize);
	if (uinsSize > 1)
	{
		if (config_file.readEntry("Look","ConferencePrefix").isEmpty())
			title = tr("Conference with ");
		else
			title = config_file.readEntry("Look","ConferencePrefix");
		int i = 0;

		if (config_file.readEntry("Look", "ConferenceContents").isEmpty())
			CONST_FOREACH(user, *Users)
			{
				title.append(KaduParser::parse("%a", *user, false));

				if (++i < uinsSize)
					title.append(", ");
			}
		else
			CONST_FOREACH(user, *Users)
			{
				title.append(KaduParser::parse(config_file.readEntry("Look","ConferenceContents"), *user, false));

				if (++i < uinsSize)
					title.append(", ");
			}

 		pix = icons_manager->loadIcon("Online");
	}
	else
	{
		UserListElement user = *Users->constBegin();
		if (config_file.readEntry("Look", "ChatContents").isEmpty())
		{
			if (user.isAnonymous())
				title = KaduParser::parse(tr("Chat with ")+"%a", user, false);
			else
				title = KaduParser::parse(tr("Chat with ")+"%a (%s[: %d])", user, false);
		}
		else
			title = KaduParser::parse(config_file.readEntry("Look","ChatContents"), user, false);
		pix = user.status(currentProtocol()->protocolID()).pixmap();
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	Caption = title;
	emit captionUpdated();
	kdebugf2();
}


bool ChatWidget::keyPressEventHandled(QKeyEvent *e)
{
	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
		clearChatWindow();
	else if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
		emit closed();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
		KaduActions["whoisAction"]->activate(Users);
	else
		return false;
// 	return true;
}

QPixmap ChatWidget::icon()
{
	return pix;
}

void ChatWidget::keyPressEvent(QKeyEvent* e)
{
	kdebugf();
 	if (keyPressEventHandled(e))
 		e->accept();
 	else
 		QWidget::keyPressEvent(e);
	kdebugf2();
}

void ChatWidget::editTextChanged()
{
	kdebugf();
	QValueList<ToolButton*> buttons =
		KaduActions["sendAction"]->toolButtonsForUserListElements(Users->toUserListElements());
	bool buttonsEnabled = !Edit->text().isEmpty();
	CONST_FOREACH(i, buttons)
		(*i)->setEnabled(buttonsEnabled);
	kdebugf2();
}

// TODO: remove
bool ChatWidget::eventFilter(QObject *watched, QEvent *ev)
{
//	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_START, "watched: %p, Edit: %p, ev->type():%d, KeyPress:%d\n", watched, Edit, ev->type(), QEvent::KeyPress);

 	if (watched != Edit || ev->type() != QEvent::KeyPress)
 		return QWidget::eventFilter(watched, ev);
 	kdebugf();
 	QKeyEvent *e = static_cast<QKeyEvent *>(ev);
 	if (keyPressEventHandled(e))
 		return true;
 	return QWidget::eventFilter(watched, ev);
}

QDateTime ChatWidget::getLastMsgTime()
{
	return lastMsgTime;
}

void ChatWidget::appendMessages(const QValueList<ChatMessage *> &messages, bool pending)
{
	body->appendMessages(messages);

	if (pending)
		lastMsgTime = QDateTime::currentDateTime();
}

void ChatWidget::appendMessage(ChatMessage *message, bool pending)
{
	body->appendMessage(message);

	if (pending)
		lastMsgTime = QDateTime::currentDateTime();
}

void ChatWidget::appendSystemMessage(const QString &rawContent, const QString &backgroundColor, const QString &fontColor)
{
	ChatMessage *message = new ChatMessage(rawContent, TypeSystem, QDateTime::currentDateTime(),
		backgroundColor, fontColor, fontColor);
	body->appendMessage(message);
}

/* invoked from outside when new message arrives, this is the window to the world */
void ChatWidget::newMessage(const QString &/*protocolName*/, UserListElements senders, const QString &msg, time_t time)
{
	QDateTime date;
	date.setTime_t(time);

	ChatMessage *message = new ChatMessage(senders[0], msg, TypeReceived, QDateTime::currentDateTime(), date);
	body->appendMessage(message);

	lastMsgTime = QDateTime::currentDateTime();
	NewMessagesCount++;

 	emit messageReceived(this);
}

void ChatWidget::writeMyMessage()
{
	kdebugf();
	ChatMessage *message = new ChatMessage(kadu->myself(), myLastMessage, TypeSent, QDateTime::currentDateTime());
	body->appendMessage(message);

	if (!Edit->isEnabled())
		cancelMessage();
	Edit->clear();

	if (KaduActions["boldAction"]->isOn(Users->toUserListElements()))
		Edit->setBold(true);
	if (KaduActions["italicAction"]->isOn(Users->toUserListElements()))
		Edit->setItalic(true);
	if (KaduActions["underlineAction"]->isOn(Users->toUserListElements()))
		Edit->setUnderline(true);
	kdebugf2();
}

void ChatWidget::clearChatWindow()
{
	kdebugf();
	if (!config_file.readBoolEntry("Chat", "ConfirmChatClear") || MessageBox::ask(tr("Chat window will be cleared. Continue?")))
	{
		body->clearMessages();
		setActiveWindow();
	}
	kdebugf2();
}

void ChatWidget::setAutoSend(bool auto_send)
{
	kdebugf();
	AutoSend = auto_send;
	Edit->setAutosend(auto_send);
	kdebugf2();
}

void ChatWidget::cancelMessage()
{
	kdebugf();
//	seq = 0;
	disconnectAcknowledgeSlots();

	Edit->setReadOnly(false);
	Edit->setEnabled(true);
	Edit->setFocus();

	WaitingForACK = false;
	KaduActions["sendAction"]->setIconSets(Users->toUserListElements(),
		icons_manager->loadIconSet("SendMessage"));
	KaduActions["sendAction"]->setTexts(Users->toUserListElements(), tr("&Send"));
	kdebugf2();
}

void ChatWidget::messageNotDeliveredSlot(const QString &message)
{
	kdebugf();
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	MessageBox::msg(message/*tr("Message has not been delivered.")*/, true, "Warning", this);
	cancelMessage();
	kdebugf2();
}

void ChatWidget::messageAcceptedSlot()
{
	kdebugf();
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	writeMyMessage();
	emit messageSentAndConfirmed(Users->toUserListElements(), myLastMessage);
	disconnectAcknowledgeSlots();
	kdebugf2();
}

void ChatWidget::connectAcknowledgeSlots()
{
	kdebugf();
	connect(CurrentProtocol, SIGNAL(messageNotDelivered(const QString&)), this, SLOT(messageNotDeliveredSlot(const QString&)));
	connect(CurrentProtocol, SIGNAL(messageAccepted()), this, SLOT(messageAcceptedSlot()));
	kdebugf2();
}

void ChatWidget::disconnectAcknowledgeSlots()
{
	kdebugf();
	disconnect(CurrentProtocol, SIGNAL(messageNotDelivered(const QString&)), this, SLOT(messageNotDeliveredSlot(const QString&)));
	disconnect(CurrentProtocol, SIGNAL(messageAccepted()), this, SLOT(messageAcceptedSlot()));
	kdebugf2();
}

void ChatWidget::selectedUsersNeeded(const UserGroup*& user_group)
{
	kdebugf();
	user_group = users();
	//TODO: do przemyslenia kwestia akcji, ktore dzialaja tylko dla wybranych
	// kontaktow z userboxa, cos w stylu:
	/*	if (!userbox)
		user = *Users->constBegin();
	else
		if (userbox->currentItem() == -1)
			user = *Users->constBegin();
		else
			user = Users->byAltNick(userbox->currentText());*/
	// tak poprzednio dzialalo "whois"
	kdebugf2();
}

/* sends the message typed */
void ChatWidget::sendMessage()
{
	kdebugf();
	if (Edit->text().isEmpty())
	{
		kdebugf2();
		return;
	}

	emit messageSendRequested(this);

	if (currentProtocol()->currentStatus().isOffline())
	{
		MessageBox::msg(tr("Cannot send message while being offline."), false, "Critical", this);
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	if (config_file.readBoolEntry("Chat","MessageAcks"))
	{
		Edit->setReadOnly(true);
		Edit->setEnabled(false);
		WaitingForACK = true;
		KaduActions["sendAction"]->setIconSets(Users->toUserListElements(),
			icons_manager->loadIconSet("CancelMessage"));
		KaduActions["sendAction"]->setTexts(Users->toUserListElements(), tr("&Cancel"));
	}
	QString message = Edit->text();

	// TODO: to na pewno jest potrzebne ??
	int messageBegin = message.find("<p>");
	int messageEnd = message.findRev("</p>");
	myLastMessage = message.mid(messageBegin + 3, messageEnd - messageBegin - 3);
	myLastMessage.replace("</p>", "");
	myLastMessage.replace("<p>", "");
	myLastMessage.replace("<br />", "\n");

	myLastMessage = currentProtocol()->sendMessage(Users->toUserListElements(), myLastMessage);

	if (config_file.readBoolEntry("Chat", "MessageAcks"))
		connectAcknowledgeSlots();
	else
	{
		writeMyMessage();
		emit messageSentAndConfirmed(Users->toUserListElements(), myLastMessage);
	}

	emit messageSent(this);
	kdebugf2();
}

void ChatWidget::openEmoticonSelector(const QWidget* activating_widget)
{
	//emoticons_selector zawsze bêdzie NULLem gdy wchodzimy do tej funkcji
	//bo EmoticonSelector ma ustawione flagi Qt::WDestructiveClose i Qt::WType_Popup
	//akcj± na opuszczenie okna jest ustawienie zmiennej emoticons_selector w Chacie na NULL
	emoticon_selector = new EmoticonSelector(NULL, "emoticon_selector", this);
	emoticon_selector->alignTo(const_cast<QWidget*>(activating_widget)); //TODO: do something about const_cast
	emoticon_selector->show();
}

void ChatWidget::changeColor(const QWidget* activating_widget)
{
	//sytuacja podobna jak w przypadku emoticon_selectora
	color_selector = new ColorSelector(Edit->paletteForegroundColor(), this, "color_selector");
	color_selector->alignTo(const_cast<QWidget*>(activating_widget)); //TODO: do something about const_cast
	color_selector->show();
	connect(color_selector, SIGNAL(colorSelect(const QColor&)), this, SLOT(colorChanged(const QColor&)));
	connect(color_selector, SIGNAL(aboutToClose()), this, SLOT(colorSelectorAboutToClose()));
}

void ChatWidget::colorSelectorAboutToClose()
{
	kdebugf();
	color_selector = NULL;
	kdebugf2();
}

void ChatWidget::colorChanged(const QColor& color)
{
	color_selector = NULL;

	QPixmap p(12, 12);
	p.fill(color);

	QIconSet icon;
	icon.setPixmap(p, QIconSet::Automatic, QIconSet::Normal);
	icon.setPixmap(icons_manager->loadIcon("Black_dis"), QIconSet::Automatic, QIconSet::Disabled);

	KaduActions["colorAction"]->setIconSets(Users->toUserListElements(), icon);
	Edit->setColor(color);
	actcolor = color;
}

/* adds an emoticon code to the edit window */
void ChatWidget::addEmoticon(QString emot)
{
	if (emot.length())
	{
		emot.replace("&lt;", "<");
		emot.replace("&gt;", ">");
		Edit->insert(emot);
	}
	emoticon_selector = NULL;
}


const UserGroup *ChatWidget::users() const
{
	return Users;
}

const QString& ChatWidget::caption() const
{
 	return Caption;
}

CustomInput* ChatWidget::edit()
{
	return Edit;
}

bool ChatWidget::autoSend() const
{
	return AutoSend;
}

bool ChatWidget::waitingForACK() const
{
	return WaitingForACK;
}

void ChatWidget::dragEnterEvent(QDragEnterEvent *e)
{
	QStringList files;

	if (QUriDrag::decodeLocalFiles(e, files))
		e->accept(files.count() > 0);
	else
		e->accept(false);
}

void ChatWidget::dragMoveEvent(QDragMoveEvent *e)
{
	QStringList files;

	if (QUriDrag::decodeLocalFiles(e, files))
		e->accept(files.count() > 0);
	else
		e->accept(false);
}

void ChatWidget::dropEvent(QDropEvent *e)
{
	QStringList files;

	if (QUriDrag::decodeLocalFiles(e, files))
	{
		e->accept(files.count() > 0);

		QStringList::iterator i = files.begin();
		QStringList::iterator end = files.end();

		for (; i != end; i++)
			emit fileDropped(Users, *i);
	}
	else
		e->accept(false);
}

Protocol *ChatWidget::currentProtocol()
{
	return CurrentProtocol;
}

// TODO: do dupy, zmieniæ przed 0.6
void ChatWidget::makeActive()
{
	kdebugf();
	QWidget *win = this;
	while (win->parent()) //for tabs module
		win = static_cast<QWidget *>(win->parent());
	win->setActiveWindow();
	// workaround for kwin which sometimes don't make window active when it's requested right after "unminimization"
	if (!win->isActiveWindow() && activationCount++ < 20)
		QTimer::singleShot(100, this, SLOT(makeActive()));
	else
		activationCount = 0;
	kdebugf2();
}

void ChatWidget::markAllMessagesRead()
{
	NewMessagesCount = 0;
}

unsigned int ChatWidget::newMessagesCount() const
{
	return NewMessagesCount;
}

void ChatWidget::restoreGeometry()
{
	QValueList<int> vertSizes = toIntList(chat_manager->getChatWidgetProperty(Users, "VerticalSizes").toList());
	if (vertSizes.empty() && Users->count() == 1)
	{
		QString vert_sz_str = (*(Users->constBegin())).data("VerticalSizes").toString();
		if (!vert_sz_str.isEmpty())
		{
			bool ok[2];
			QStringList s = QStringList::split(",", vert_sz_str);
			vertSizes.append(s[0].toInt(ok));
			vertSizes.append(s[1].toInt(ok + 1));
			if (!(ok[0] && ok[1]))
				vertSizes.clear();
		}
	}

	if (vertSizes.empty())
	{
		int h = height() / 3;
		vertSizes.append(h * 2);
		vertSizes.append(h);
	}
	vertSplit->setSizes(vertSizes);

	if (horizSplit)
	{
		QValueList<int> horizSizes = toIntList(chat_manager->getChatWidgetProperty(Users, "HorizontalSizes").toList());
		if (!horizSizes.empty())
			horizSplit->setSizes(horizSizes);
	}
}

void ChatWidget::storeGeometry()
{
	QValueList<int> sizes = vertSplit->sizes();
	chat_manager->setChatWidgetProperty(Users, "VerticalSizes", toVariantList(sizes));

	if (Users->count() == 1)
		(*Users->begin()).setData("VerticalSizes", QString("%1,%2").arg(sizes[0]).arg(sizes[1]));

	if (horizSplit)
		chat_manager->setChatWidgetProperty(Users, "HorizontalSizes", toVariantList(horizSplit->sizes()));
}

void ChatWidget::leaveConference()
{
	if (!MessageBox::ask("All messages received in this conference will be ignored\nfrom now on. Are you sure you want to leave this conference?", "Warning", this))
		return;

	UserListElements users = Users->toUserListElements();
	if (!IgnoredManager::isIgnored(users))
		IgnoredManager::insert(users);

	emit closed();
}

void ChatWidget::keyPressedSlot(QKeyEvent *e, CustomInput *sender, bool &handled)
{
	emit keyPressed(e, this, handled);
}
