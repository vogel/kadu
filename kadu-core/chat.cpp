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
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qvbox.h>

#include "action.h"
#include "chat.h"
#include "chat_colors.h"
#include "chat_manager.h"
#include "chat_message.h"
#include "chat_styles.h"
#include "color_selector.h"
#include "config_dialog.h"
#include "custom_input.h"
#include "debug.h"
#include "dockarea.h"
#include "gadu_images_manager.h"
#include "gadu_rich_text.h"
#include "icons_manager.h"
#include "kadu_parser.h"
#include "kadu_splitter.h"
#include "kadu_text_browser.h"
#include "message_box.h"
#include "misc.h"
#include "search.h"
#include "userbox.h"

Chat::Chat(Protocol *initialProtocol, const UserListElements &usrs, QWidget* parent, const char* name)
	: QMainWindow(parent, name, Qt::WDestructiveClose), ChatMessages(), CurrentProtocol(initialProtocol),
	Users(new UserGroup(usrs)),
	index(0), title_buffer(), title_timer(new QTimer(this, "title_timer")), actcolor(), Edit(0),
	bodyformat(new QMimeSourceFactory()), emoticon_selector(0), color_selector(0),
	AutoSend(config_file.readBoolEntry("Chat", "AutoSend")), ScrollLocked(false),
	WaitingForACK(false), userbox(0), myLastMessage(), myLastFormatsLength(0),
	myLastFormats(0), seq(0), vertSplit(0), horizSplit(0),
	ParagraphSeparator(config_file.readNumEntry("Look", "ParagraphSeparator")),
	lastMsgTime(), PreviousMessage(), CfgNoHeaderRepeat(config_file.readBoolEntry("Look","NoHeaderRepeat")),
	CfgHeaderSeparatorHeight(0), CfgNoHeaderInterval(0), Style(0), LastTime(0), body(0), activationCount(0),
	newMessagesNum(0), showNewMessagesNum(config_file.readBoolEntry("Chat","NewMessagesInChatTitle")),
	blinkChatTitle(config_file.readBoolEntry("Chat","BlinkChatTitle"))
{
	kdebugf();
	const int minimumDockAreaSize = 3;
	QValueList<int> sizes;

	setAcceptDrops(true);

	connect(title_timer,SIGNAL(timeout()),this,SLOT(changeTitle()));

	/* register us in the chats registry... */
	index=chat_manager->registerChat(this);

	QHBox *horizontalBox = new QHBox(this, "horizontalBox");
	setCentralWidget(horizontalBox);

	DockArea *leftDockArea = new DockArea(Qt::Vertical, DockArea::Normal, horizontalBox,
		"chatLeftDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(leftDockArea, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	leftDockArea->setMinimumWidth(minimumDockAreaSize);

	QVBox *central = new QVBox(horizontalBox, "central");
	horizontalBox->setStretchFactor(central, 50);

	DockArea *rightDockArea = new DockArea(Qt::Vertical, DockArea::Normal, horizontalBox,
		"chatRightDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(rightDockArea, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	rightDockArea->setMinimumWidth(minimumDockAreaSize);

	vertSplit = new KaduSplitter(Qt::Vertical, central, "vertSplit");

	QVBox *topArea = new QVBox(vertSplit, "topArea");
	DockArea *topDockArea = new DockArea(Qt::Horizontal, DockArea::Normal, topArea,
		"chatTopDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(topDockArea, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	topDockArea->setMinimumHeight(minimumDockAreaSize);

	if (Users->count() > 1)
	{
		horizSplit = new KaduSplitter(Qt::Horizontal, topArea, "horizSplit");
		horizSplit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		body = new KaduTextBrowser(horizSplit, "body");
	}
	else
		body = new KaduTextBrowser(topArea, "body");

	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_ANIMATED)
		body->setStyleSheet(new AnimStyleSheet(body,emoticons->themePath()));
	else
		body->setStyleSheet(new StaticStyleSheet(body,emoticons->themePath()));
//	body->setTrueTransparency(true);

	body->setMargin(ParagraphSeparator);
	body->setMinimumSize(QSize(100,100));
	body->setFont(config_file.readFontEntry("Look","ChatFont"));

	// background color of chat
	QString bgImage = KaduParser::parse(config_file.readEntry("Look", "ChatBgImage"), usrs[0]);
	QBrush brush(config_file.readColorEntry("Look", "ChatBgColor"));
	if (!bgImage.isEmpty() && QFile::exists(bgImage))
		brush.setPixmap(QPixmap(bgImage));
	body->setPaper(brush);

//	QPoint pos = QCursor::pos();

	if (Users->count() > 1)
	{
		userbox = new UserBox(Users, horizSplit, "userbox");
		userbox->setMinimumSize(QSize(30,30));
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
		connect(userbox, SIGNAL(rightButtonPressed(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));

		sizes.append(3);
		sizes.append(1);
		horizSplit->setSizes(sizes);
	}

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
	buttontray->setMinimumHeight(minimumDockAreaSize);
	edtbuttontray->setStretchFactor(buttontray, 50);

	Edit = new CustomInput(downpart, "edit");
	Edit->setMinimumHeight(1);
	Edit->setWordWrap(QMultiLineEdit::WidgetWidth);
	Edit->setFont(config_file.readFontEntry("Look","ChatFont"));
	Edit->setPaper(QBrush(config_file.readColorEntry("Look","ChatTextBgColor")));
	Edit->setAutosend(AutoSend);

	connect(body, SIGNAL(mouseReleased(QMouseEvent *)), Edit, SLOT(setFocus()));
	connect(body, SIGNAL(wheel(QWheelEvent *)), Edit, SLOT(setFocus()));

	QString style = config_file.readEntry("Look", "Style");
	if (style == "kadu")
		Style = new KaduChatStyle();
	else if (style == "hapi")
		Style = new HapiChatStyle();
	else if (style == "irc")
		Style = new IrcChatStyle();
	else
		Style = new CustomChatStyle(config_file.readEntry("Look", "FullStyle"));

	// headers removal stuff
	if (CfgNoHeaderRepeat)
	{
	    CfgHeaderSeparatorHeight = config_file.readNumEntry("Look","HeaderSeparatorHeight");
	    CfgNoHeaderInterval = config_file.readNumEntry("Look","NoHeaderInterval");
	    LastTime = 0;		//zerowanie licznika ró¿nicy czasu miêdzy wiadomo¶ciami
	    PreviousMessage = "";
	}

	DockArea* btnpart = new DockArea(Qt::Horizontal, DockArea::Normal, downpart,
		"chatBottomDockArea", Action::TypeGlobal | Action::TypeUser | Action::TypeChat);
	connect(btnpart, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	btnpart->setMinimumHeight(minimumDockAreaSize);

	QAccel *acc = new QAccel(this, "returnAccel");
	acc->connectItem(acc->insertItem(Key_Return + CTRL), this, SLOT(sendMessage()));

	acc = new QAccel(this, "pageUpAccel");
	acc->connectItem(acc->insertItem(Key_PageUp + SHIFT), this, SLOT(pageUp()));
	acc = new QAccel(this, "pageDownAccel");
	acc->connectItem(acc->insertItem(Key_PageDown + SHIFT), this, SLOT(pageDown()));

	topDockArea->loadFromConfig(this);
	leftDockArea->loadFromConfig(this);
	rightDockArea->loadFromConfig(this);

	if (!buttontray->loadFromConfig(this))
	{
		// TOOLBAR 1
		ToolBar* tb1 = new ToolBar(this, "Chat toolbar 1");
		tb1->setOffset(10000);
		tb1->show();
		buttontray->moveDockWindow(tb1);
		buttontray->setAcceptDockWindow(tb1, true);
		KaduActions.addDefaultActionsToToolbar(tb1);
	}

	if (!btnpart->loadFromConfig(this))
	{
		// TOOLBAR2
		ToolBar* tb2 = new ToolBar(this, "Chat toolbar 2");
		tb2->show();
		btnpart->moveDockWindow(tb2);
		btnpart->setAcceptDockWindow(tb2, true);
		KaduActions.addDefaultActionsToToolbar(tb2);
		// TOOLBAR 3
		ToolBar* tb3 = new ToolBar(this, "Chat toolbar 3");
		tb3->setOffset(10000);
		tb3->show();
		btnpart->moveDockWindow(tb3);
		btnpart->setAcceptDockWindow(tb3, true);
		KaduActions.addDefaultActionsToToolbar(tb3);
	}

	body->setMimeSourceFactory(bodyformat);
	body->setTextFormat(Qt::RichText);
	Edit->setMimeSourceFactory(bodyformat);
	Edit->setTextFormat(Qt::RichText);

	connect(Edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(curPosChanged(int, int)));
	connect(Edit, SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(Edit, SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));
	connect(Edit, SIGNAL(textChanged()), this, SLOT(editTextChanged()));

	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	editTextChanged(); // slot ustawia poprawny stan przycisku Send (tutaj - blokuje)

	connect(KaduActions["sendAction"], SIGNAL(addedToToolbar(ToolButton*, ToolBar*)),
		this, SLOT(sendActionAddedToToolbar(ToolButton*, ToolBar*)));


	Edit->setFocus();

	Edit->installEventFilter(this);

	kdebugf2();
}

Chat::~Chat()
{
	kdebugf();
	chat_manager->unregisterChat(this);

	disconnectAcknowledgeSlots();
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	FOREACH(msg, ChatMessages)
		delete *msg;
	ChatMessages.clear();

	if (userbox)
		delete userbox;
	delete bodyformat;
	delete Users;
	delete Style;

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed: index %d\n", index);
}

void Chat::specialKeyPressed(int key)
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

void Chat::curPosChanged(int, int)
{
	kdebugf();

	UserListElements elems = Users->toUserListElements();
	if (Edit->bold() != KaduActions["boldAction"]->isOn(elems))
		KaduActions["boldAction"]->setOn(elems, Edit->bold());
	if (Edit->italic() != KaduActions["italicAction"]->isOn(elems))
		KaduActions["italicAction"]->setOn(elems, Edit->italic());
	if (Edit->underline() != KaduActions["underlineAction"]->isOn(elems))
		KaduActions["underlineAction"]->setOn(elems, Edit->underline());
	if (Edit->color() != actcolor)
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
		KaduActions["colorAction"]->setPixmaps(elems, p);
	}
	kdebugf2();
}

unsigned int Chat::getNewMessagesNum() const
{
	return newMessagesNum;
}

void Chat::setShowNewMessagesNum(bool toggled)
{
	showNewMessagesNum = toggled;
}

bool Chat::getShowNewMessagesNum() const
{
	return showNewMessagesNum;
}

void Chat::setBlinkChatTitle(bool toggled)
{
	blinkChatTitle = toggled;
}

bool Chat::getBlinkChatTitle() const
{
	return blinkChatTitle;
}

void Chat::pageUp()
{
	body->scrollBy(0, (body->height() * -2) / 3);
}

void Chat::pageDown()
{
	body->scrollBy(0, (body->height() * 2) / 3);
}

void Chat::insertImage()
{
	kdebugf();
	ImageDialog* id = new ImageDialog(this);
	id->setDir(config_file.readEntry("Chat", "LastImagePath"));
	id->setCaption(tr("Insert image"));
	if (id->exec() == QDialog::Accepted)
	{
		config_file.writeEntry("Chat", "LastImagePath", id->dirPath());
		QString selectedFile = id->selectedFile();
		QFileInfo f(selectedFile);
		delete id;id = NULL;
		if (!f.isReadable())
		{
			MessageBox::wrn(tr("This file is not readable"), true);
			QTimer::singleShot(0, this, SLOT(insertImage()));
			kdebugf2();
			return;
		}

		if (f.size() >= (1 << 18)) // 256kB
		{
			MessageBox::wrn(tr("This file is too big (%1 >= %2)").arg(f.size()).arg(1<<18), true);
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
	Edit->setFocus();
	kdebugf2();
}

void Chat::imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& /*path*/)
{
	kdebugf();
	FOREACH(msg, ChatMessages)
		(*msg)->message =
			gadu_images_manager.replaceLoadingImages(
				(*msg)->message,sender,size,crc32);
	repaintMessages();
	kdebugf2();
}

void Chat::sendActionAddedToToolbar(ToolButton* button, ToolBar* /*toolbar*/)
{
	kdebugf();
	button->setEnabled(!Edit->text().isEmpty());
	kdebugf2();
}

void Chat::changeAppearance()
{
	kdebugf();
	if (Users->count() > 1 && userbox)
	{
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
	}
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
	Edit->setFont(config_file.readFontEntry("Look","ChatFont"));
	kdebugf2();
}

void Chat::refreshTitle()
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
		CONST_FOREACH(user, *Users)
		{
			title.append(KaduParser::parse(config_file.readEntry("Look","ConferenceContents"), *user, false));
			if (++i < uinsSize)
				title.append(", ");
		}
		setIcon(icons_manager->loadIcon("Online"));
	}
	else
	{
		UserListElement user = *Users->constBegin();
		if (config_file.readEntry("Look","ChatContents").isEmpty())
		{
			if (user.isAnonymous())
				title = KaduParser::parse(tr("Chat with ")+"%a", user, false);
			else
				title = KaduParser::parse(tr("Chat with ")+"%a (%s[: %d])", user, false);
		}
		else
			title = KaduParser::parse(config_file.readEntry("Look","ChatContents"), user, false);
		setIcon(user.status(currentProtocol()->protocolID()).pixmap());
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	title_buffer = title;

	if (!showNewMessagesNum || (newMessagesNum == 0)) // if we don't have new messages or don't want them to be shown
		setCaption(title_buffer);
	else
		showNewMessagesNumInTitle();

	kdebugf2();
}

void Chat::changeTitle()
{
	if (!isActiveWindow())
	{
		if (!caption().contains(title_buffer) || !blinkChatTitle)
		{
			if (!showNewMessagesNum) // if we don't show number od new messages waiting
				setCaption(title_buffer);
			else
				showNewMessagesNumInTitle();
		}
		else
		{
			setCaption(QString().fill(' ', (title_buffer.length()+5)));
		}

		if (blinkChatTitle) // timer will not be started, if configuration option was changed
			title_timer->start(500,TRUE);
	}
}

void Chat::showNewMessagesNumInTitle()
{
	if (!isActiveWindow())
		setCaption("[" + QString().setNum(newMessagesNum) + "] " + title_buffer);
}

void Chat::windowActivationChange(bool b)
{
	kdebugf();

	if (isActiveWindow())
	{
		setCaption(title_buffer);
		newMessagesNum = 0;

		if (title_timer->isActive())
			title_timer->stop();

		if (!b)
			emit chatActivated(this);
	}

	kdebugf2();
}

bool Chat::keyPressEventHandled(QKeyEvent *e)
{
	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
		clearChatWindow();
	else if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
		close();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
		KaduActions["whoisAction"]->activate(Users);
	else
		return false;
	return true;
}

void Chat::keyPressEvent(QKeyEvent* e)
{
	kdebugf();
	if (keyPressEventHandled(e))
		e->accept();
	else
		QMainWindow::keyPressEvent(e);
	kdebugf2();
}

void Chat::editTextChanged()
{
	kdebugf();
	QValueList<ToolButton*> buttons =
		KaduActions["sendAction"]->toolButtonsForUserListElements(Users->toUserListElements());
	bool buttonsEnabled = !Edit->text().isEmpty();
	CONST_FOREACH(i, buttons)
		(*i)->setEnabled(buttonsEnabled);
	kdebugf2();
}

bool Chat::eventFilter(QObject *watched, QEvent *ev)
{
//	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_START, "watched: %p, Edit: %p, ev->type():%d, KeyPress:%d\n", watched, Edit, ev->type(), QEvent::KeyPress);
	if (watched != Edit || ev->type() != QEvent::KeyPress)
		return QMainWindow::eventFilter(watched, ev);
	kdebugf();
	QKeyEvent *e = static_cast<QKeyEvent *>(ev);
	if (keyPressEventHandled(e))
		return true;
	return QMainWindow::eventFilter(watched, ev);
}

void Chat::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	Edit->setFocus();
	QMainWindow::mouseReleaseEvent(e);
}

/* unregister us */
void Chat::closeEvent(QCloseEvent* e)
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "ChatCloseTimer"))
	{
		unsigned int period = config_file.readUnsignedNumEntry("Chat",
			"ChatCloseTimerPeriod", 2);
		if (QDateTime::currentDateTime() < lastMsgTime.addSecs(period))
		{
			if (!MessageBox::ask(tr("New message received, close window anyway?")))
			{
				e->ignore();
				return;
			}
		}
	}
	QMainWindow::closeEvent(e);
}

void Chat::formatMessages(QValueList<ChatMessage *> &msgs)
{
	OwnChatColors own_colors;
	UserChatColors user_colors;
	EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle");
	FOREACH(msg, msgs)
		formatMessage(**msg, &own_colors, &user_colors, style);
}

void Chat::formatMessage(ChatMessage &msg, const OwnChatColors* own_colors,
	const UserChatColors* user_colors, EmoticonsStyle style)
{
	if (msg.isMyMessage)
	{
		if (own_colors == NULL)
			msg.Colors = OwnChatColors();
		else
			msg.Colors = *own_colors;
	}
	else
	{
		if (user_colors == NULL)
			msg.Colors = UserChatColors();
		else
			msg.Colors = *user_colors;
	}

	if (style != EMOTS_NONE)
		body->mimeSourceFactory()->addFilePath(emoticons->themePath());

	if (CfgNoHeaderRepeat)
	{
		time_t CurTime = msg.date.toTime_t(); // ilo¶æ sekund od 1970 roku
		if ((CurTime - LastTime <= (CfgNoHeaderInterval * 60)) && (PreviousMessage == msg.nick))
			msg.formatMessage(Style, style, false, CfgHeaderSeparatorHeight);
		else
			msg.formatMessage(Style, style, true, ParagraphSeparator);
		PreviousMessage = msg.nick;
		LastTime = CurTime;
	}
	else
		msg.formatMessage(Style, style, true, ParagraphSeparator);
}

void Chat::repaintMessages()
{
	kdebugf();
	body->viewport()->setUpdatesEnabled(false);

	QString text;
	int i;

	QValueList<ChatMessage *>::const_iterator it=ChatMessages.begin();
	//z pierwszej wiadomo¶ci usuwamy obrazek separatora
	if (it!=ChatMessages.end())
	{
		QString msg=(*it)->message;
		msg.remove(QRegExp("<img title=\"\" height=\"[0-9]*\" width=\"10000\" align=\"right\">"));
		text+=msg;
		++it;
	}
	for(; it!=ChatMessages.end(); ++it)
		text+=(*it)->message;
	body->setText(text);

	i=0;
	if (body->paper().pixmap() == 0)
		CONST_FOREACH(msg, ChatMessages)
			body->setParagraphBackgroundColor(i++, (*msg)->Colors.backgroundColor());

	if (!ScrollLocked)
		body->scrollToBottom();

	body->viewport()->setUpdatesEnabled(true);
	body->viewport()->repaint();
	kdebugf2();
}

void Chat::scrollMessages(const QValueList<ChatMessage *> &messages)
{
	kdebugf();
	if (config_file.readBoolEntry("Chat","ChatPrune"))
		pruneWindow();
	ChatMessages+=messages;
	repaintMessages();
	kdebugf2();
}

/* invoked from outside when new message arrives, this is the window to the world */
void Chat::newMessage(const QString &/*protocolName*/, UserListElements senders, const QString &msg, time_t time)
{
	QValueList<ChatMessage *> messages;
	QDateTime date;
	date.setTime_t(time);

	ChatMessage *message = new ChatMessage(senders[0].altNick(), msg, false, QDateTime::currentDateTime(), date);
	formatMessage(*message);
	messages.append(message);

	scrollMessages(messages);

	emit messageReceived(this);
}

void Chat::alertNewMessage()
{
	if (!isActiveWindow())
	{
		newMessagesNum++;

		if (blinkChatTitle)
		{
			if (!title_timer->isActive())
				changeTitle(); // blinking is able to show new messages also...
		}
		else if (showNewMessagesNum) // ... so we check this condition as 'else'
			showNewMessagesNumInTitle();
	}

	lastMsgTime = QDateTime::currentDateTime();
}

void Chat::writeMyMessage()
{
	kdebugf();
	QValueList<ChatMessage *> messages;
	ChatMessage *msg=new ChatMessage(config_file.readEntry("General","Nick"), myLastMessage, true, QDateTime::currentDateTime());
	formatMessage(*msg);
	messages.append(msg);
	scrollMessages(messages);

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

void Chat::clearChatWindow()
{
	kdebugf();
	if (!config_file.readBoolEntry("Chat", "ConfirmChatClear") || MessageBox::ask(tr("Chat window will be cleared. Continue?")))
	{
		FOREACH(msg, ChatMessages)
			delete *msg;
		ChatMessages.clear();
		body->clear();
		if (CfgNoHeaderRepeat)
			LastTime = 0;
		setActiveWindow();
		Edit->setFocus();
	}
	kdebugf2();
}

void Chat::setAutoSend(bool auto_send)
{
	kdebugf();
	AutoSend = auto_send;
	Edit->setAutosend(auto_send);
	kdebugf2();
}

void Chat::setScrollLocked(bool locked)
{
	kdebugf();
	ScrollLocked = locked;
	kdebugf2();
}

void Chat::cancelMessage()
{
	kdebugf();
	seq = 0;
	disconnectAcknowledgeSlots();
	Edit->setReadOnly(false);
	Edit->setEnabled(true);
	Edit->setFocus();
	WaitingForACK = false;
	KaduActions["sendAction"]->setPixmaps(Users->toUserListElements(),
		icons_manager->loadIcon("SendMessage"));
	KaduActions["sendAction"]->setTexts(Users->toUserListElements(), tr("&Send"));
	kdebugf2();
}

void Chat::messageBlockedSlot(int Seq, UinType /*uin*/)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	MessageBox::wrn(tr("Your message has been blocked by server. Message has not been delivered."), true);
	cancelMessage();
	kdebugf2();
}

void Chat::messageBoxFullSlot(int Seq, UinType /*uin*/)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	MessageBox::wrn(tr("User's message box is full. Message has not been delivered."), true);
	cancelMessage();
	kdebugf2();
}

void Chat::messageNotDeliveredSlot(int Seq, UinType /*uin*/)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	MessageBox::wrn(tr("Message has not been delivered."), true);
	cancelMessage();
	kdebugf2();
}

void Chat::messageAcceptedSlot(int Seq, UinType /*uin*/)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	writeMyMessage();
	emit messageSentAndConfirmed(Users->toUserListElements(), myLastMessage);
	seq = 0;
	disconnectAcknowledgeSlots();
	kdebugf2();
}

void Chat::connectAcknowledgeSlots()
{
	kdebugf();
	connect(gadu, SIGNAL(messageBlocked(int, UinType)), this, SLOT(messageBlockedSlot(int, UinType)));
	connect(gadu, SIGNAL(messageBoxFull(int, UinType)), this, SLOT(messageBoxFullSlot(int, UinType)));
	connect(gadu, SIGNAL(messageNotDelivered(int, UinType)), this, SLOT(messageNotDeliveredSlot(int, UinType)));
	connect(gadu, SIGNAL(messageAccepted(int, UinType)), this, SLOT(messageAcceptedSlot(int, UinType)));
	kdebugf2();
}

void Chat::disconnectAcknowledgeSlots()
{
	kdebugf();
	disconnect(gadu, SIGNAL(messageBlocked(int, UinType)), this, SLOT(messageBlockedSlot(int, UinType)));
	disconnect(gadu, SIGNAL(messageBoxFull(int, UinType)), this, SLOT(messageBoxFullSlot(int, UinType)));
	disconnect(gadu, SIGNAL(messageNotDelivered(int, UinType)), this, SLOT(messageNotDeliveredSlot(int, UinType)));
	disconnect(gadu, SIGNAL(messageAccepted(int, UinType)), this, SLOT(messageAcceptedSlot(int, UinType)));
	kdebugf2();
}

void Chat::selectedUsersNeeded(const UserGroup*& user_group)
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
void Chat::sendMessage()
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
		QMessageBox::critical(this, tr("Send message error"),
			tr("Cannot send message while being offline."));
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	QString mesg = Edit->text();
	mesg.replace("\n", "\r\n");
	mesg = unformatGGMessage(mesg, myLastFormatsLength, myLastFormats);
	myLastMessage = mesg;
	if (myLastFormatsLength)
		myLastMessage = formatGGMessage(myLastMessage, myLastFormatsLength - sizeof(struct gg_msg_richtext),
			(void *)((char *)(myLastFormats) + sizeof(struct gg_msg_richtext)),0);
	else
		HtmlDocument::escapeText(myLastMessage);
	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(myLastMessage));
	myLastMessage.replace("\r\n", "\n");

	if (mesg.length() >= 2000)
	{
		MessageBox::wrn(tr("Message too long (%1>=%2)").arg(mesg.length()).arg(2000));
		kdebugmf(KDEBUG_FUNCTION_END, "end: message too long\n");
		return;
	}

	QCString msg = unicode2cp(mesg);

	bool stop = false;
	emit messageFiltering(Users, msg, stop);
	if (stop)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return;
	}

	if (msg.length() >= 2000)
	{
		MessageBox::wrn(tr("Filtered message too long (%1>=%2)").arg(msg.length()).arg(2000));
		kdebugmf(KDEBUG_FUNCTION_END, "end: filtered message too long\n");
		return;
	}

	if (config_file.readBoolEntry("Chat","MessageAcks"))
	{
		Edit->setReadOnly(true);
		Edit->setEnabled(false);
		WaitingForACK = true;
		KaduActions["sendAction"]->setPixmaps(Users->toUserListElements(),
			icons_manager->loadIcon("CancelMessage"));
		KaduActions["sendAction"]->setTexts(Users->toUserListElements(), tr("&Cancel"));
	}

	if (currentProtocol() == gadu)
	{
		if (myLastFormatsLength)
			seq = gadu->sendMessageRichText(Users->toUserListElements(), msg, (unsigned char *)myLastFormats, myLastFormatsLength);
		else
			seq = gadu->sendMessage(Users->toUserListElements(), msg);
	}

	if (myLastFormats)
		delete [](char *)myLastFormats;

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

/* prunes messages */
void Chat::pruneWindow()
{
	kdebugf();
	unsigned int chatPruneLen=config_file.readUnsignedNumEntry("Chat","ChatPruneLen");

	if (ChatMessages.size()<chatPruneLen)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: nothing to do\n");
		return;
	}
	QValueList<ChatMessage *>::iterator start=ChatMessages.begin();
	QValueList<ChatMessage *>::iterator stop=ChatMessages.at(ChatMessages.size()-chatPruneLen+1);
	for(QValueList<ChatMessage *>::iterator it=start; it!=stop; ++it)
		delete *it;
	ChatMessages.erase(start, stop);

	kdebugf2();
}

void Chat::openEmoticonSelector(const QWidget* activating_widget)
{
	//emoticons_selector zawsze bêdzie NULLem gdy wchodzimy do tej funkcji
	//bo EmoticonSelector ma ustawione flagi Qt::WDestructiveClose i Qt::WType_Popup
	//akcj± na opuszczenie okna jest ustawienie zmiennej emoticons_selector w Chacie na NULL
	emoticon_selector = new EmoticonSelector(NULL, "emoticon_selector", this);
	emoticon_selector->alignTo(const_cast<QWidget*>(activating_widget)); //TODO: do something about const_cast
	emoticon_selector->show();
}

void Chat::changeColor(const QWidget* activating_widget)
{
	//sytuacja podobna jak w przypadku emoticon_selectora
	color_selector = new ColorSelector(Edit->paletteForegroundColor(), this, "color_selector");
	color_selector->alignTo(const_cast<QWidget*>(activating_widget)); //TODO: do something about const_cast
	color_selector->show();
	connect(color_selector, SIGNAL(colorSelect(const QColor&)), this, SLOT(colorChanged(const QColor&)));
	connect(color_selector, SIGNAL(aboutToClose()), this, SLOT(colorSelectorAboutToClose()));
}

void Chat::colorSelectorAboutToClose()
{
	kdebugf();
	color_selector = NULL;
	kdebugf2();
}

void Chat::colorChanged(const QColor& color)
{
	color_selector = NULL;
	QPixmap p(12, 12);
	p.fill(color);
	KaduActions["colorAction"]->setPixmaps(Users->toUserListElements(), p);
	Edit->setColor(color);
	actcolor = color;
}

/* adds an emoticon code to the edit window */
void Chat::addEmoticon(QString emot)
{
	if (emot.length())
	{
		emot.replace("&lt;", "<");
		emot.replace("&gt;", ">");
		Edit->insert(emot);
		Edit->setFocus();
	}
	emoticon_selector = NULL;
}


const UserGroup *Chat::users() const
{
	return Users;
}

QValueList<ChatMessage*>& Chat::chatMessages()
{
	return ChatMessages;
}

const QString& Chat::title() const
{
	return title_buffer;
}

CustomInput* Chat::edit()
{
	return Edit;
}

bool Chat::autoSend() const
{
	return AutoSend;
}

bool Chat::waitingForACK() const
{
	return WaitingForACK;
}

void Chat::dragEnterEvent(QDragEnterEvent *e)
{
	QStringList files;

	if (QUriDrag::decodeLocalFiles(e, files))
		e->accept(files.count() > 0);
	else
		e->accept(false);
}

void Chat::dragMoveEvent(QDragMoveEvent *e)
{
	QStringList files;

	if (QUriDrag::decodeLocalFiles(e, files))
		e->accept(files.count() > 0);
	else
		e->accept(false);
}

void Chat::dropEvent(QDropEvent *e)
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

void Chat::scrollMessagesToBottom()
{
	body->scrollToBottom();
}

Protocol *Chat::currentProtocol()
{
	return CurrentProtocol;
}

// TODO: do dupy, zmieniæ przed 0.6
void Chat::makeActive()
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
