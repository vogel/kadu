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
#include "chat_manager.h"
#include "chat_message.h"
#include "color_selector.h"
#include "config_dialog.h"
#include "custom_input.h"
#include "debug.h"
#include "dockarea.h"
#include "gadu_images_manager.h"
#include "gadu_rich_text.h"
#include "history.h"
#include "icons_manager.h"
#include "kadu_splitter.h"
#include "kadu_text_browser.h"
#include "message_box.h"
#include "misc.h"
#include "search.h"
#include "userbox.h"

extern const char *colors[];

Chat::Chat(UserListElements usrs, QWidget* parent, const char* name)
	: QMainWindow(parent, name, Qt::WDestructiveClose), Users(new UserGroup(2 * usrs.count()))
{
	kdebugf();
	Users->addUsers(usrs);
	QValueList<int> sizes;

	setAcceptDrops(true);

	emoticon_selector = NULL;
	color_selector = NULL;

	AutoSend = false;
	if (config_file.readBoolEntry("Chat","AutoSend"))
		AutoSend = true;

	ScrollLocked = false;
	WaitingForACK = false;

	title_timer = new QTimer(this, "title_timer");
	connect(title_timer,SIGNAL(timeout()),this,SLOT(changeTitle()));

	ParagraphSeparator=config_file.readNumEntry("General", "ParagraphSeparator");

	/* register us in the chats registry... */
	index=chat_manager->registerChat(this);

	vertSplit = new KaduSplitter(Qt::Vertical, this, "vertSplit");

	if (Users->count() > 1)
	{
		horizSplit = new KaduSplitter(Qt::Horizontal, vertSplit, "horizSplit");
		body = new KaduTextBrowser(horizSplit, "body");
	}
	else
	{
		horizSplit=NULL;
		body = new KaduTextBrowser(vertSplit, "body");
	}

	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_ANIMATED)
		body->setStyleSheet(new AnimStyleSheet(body,emoticons->themePath()));
	else
		body->setStyleSheet(new StaticStyleSheet(body,emoticons->themePath()));

	body->setMargin(ParagraphSeparator);
	body->setMinimumSize(QSize(100,100));
	body->setFont(config_file.readFontEntry("Look","ChatFont"));

	// background color of chat
	body->setPaper(QBrush(config_file.readColorEntry("Look","ChatBgColor")));

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
	else
		userbox = NULL;

	QVBox *downpart = new QVBox(vertSplit, "downpartBox");
	QHBox *edtbuttontray = new QHBox(downpart, "edtbuttontrayBox");

	QLabel *edt = new QLabel(tr("Edit window:"), edtbuttontray, "editLabel");
	QToolTip::add(edt, tr("This is where you type in the text to be sent"));

	DockArea* buttontray = new DockArea(Qt::Horizontal, DockArea::Normal, edtbuttontray,
		"chatDockAreaGroup", "chatMiddleDockArea");
	connect(buttontray, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	buttontray->setMinimumHeight(20);
	edtbuttontray->setStretchFactor(edt, 1);
	edtbuttontray->setStretchFactor(buttontray, 50);

	Edit = new CustomInput(downpart, "edit");
	Edit->setMinimumHeight(1);
	Edit->setWordWrap(QMultiLineEdit::WidgetWidth);
	Edit->setFont(config_file.readFontEntry("Look","ChatFont"));
	Edit->setPaper(QBrush(config_file.readColorEntry("Look","ChatTextBgColor")));
	Edit->setAutosend(AutoSend);

	connect(body, SIGNAL(mouseReleased(QMouseEvent *, KaduTextBrowser *)), Edit, SLOT(setFocus()));

	// headers removal stuff
	CfgNoHeaderRepeat = config_file.readBoolEntry("Look","NoHeaderRepeat");
	if (CfgNoHeaderRepeat)
	{
	    CfgHeaderSeparatorHeight = config_file.readNumEntry("Look","HeaderSeparatorHeight");
	    CfgNoHeaderInterval = config_file.readNumEntry("Look","NoHeaderInterval");
	    LastTime = 0;		//zerowanie licznika ró¿nicy czasu miêdzy wiadomo¶ciami
	    PreviousMessage = "";
	}

	DockArea* btnpart = new DockArea(Qt::Horizontal, DockArea::Normal, downpart,
		"chatDockAreaGroup", "chatBottomDockArea");
	connect(btnpart, SIGNAL(selectedUsersNeeded(const UserGroup*&)),
		this, SLOT(selectedUsersNeeded(const UserGroup*&)));
	btnpart->setMinimumHeight(20);

	QAccel *acc = new QAccel(this, "returnAccel");
	acc->connectItem(acc->insertItem(Key_Return + CTRL), this, SLOT(sendMessage()));

	acc = new QAccel(this, "pageUpAccel");
	acc->connectItem(acc->insertItem(Key_PageUp + SHIFT), this, SLOT(pageUp()));
	acc = new QAccel(this, "pageDownAccel");
	acc->connectItem(acc->insertItem(Key_PageDown + SHIFT), this, SLOT(pageDown()));

	sizes.clear();
	sizes.append(3);
	sizes.append(2);
	vertSplit->setSizes(sizes);

	QGridLayout *grid = new QGridLayout (this, 5, 4, 3, 3, "main_grid_layout");
	grid->addMultiCellWidget(vertSplit, 0, 4, 0, 3);
	grid->addRowSpacing(1, 5);
	grid->setRowStretch(0, 2);

	if (!buttontray->loadFromConfig(this))
	{
		// TOOLBAR 1
		ToolBar* tb1 = new ToolBar(this, "Chat toolbar 1");
		tb1->setOffset(1000);
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
		tb3->setOffset(1000);
		tb3->show();
		btnpart->moveDockWindow(tb3);
		btnpart->setAcceptDockWindow(tb3, true);
		KaduActions.addDefaultActionsToToolbar(tb3);
	}

	bodyformat = new QMimeSourceFactory();

	body->setMimeSourceFactory(bodyformat);
	body->setTextFormat(Qt::RichText);
	Edit->setMimeSourceFactory(bodyformat);
	Edit->setTextFormat(Qt::RichText);

	connect(Edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(curPosChanged(int, int)));
	connect(Edit, SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(Edit, SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));

	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	Edit->setFocus();
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
	int i;

	kdebugf();
	if (Edit->bold() != KaduActions["boldAction"]->isOn(Users->toUserListElements()))
		KaduActions["boldAction"]->setOn(Users->toUserListElements(), Edit->bold());
	if (Edit->italic() != KaduActions["italicAction"]->isOn(Users->toUserListElements()))
		KaduActions["italicAction"]->setOn(Users->toUserListElements(), Edit->italic());
	if (Edit->underline() != KaduActions["underlineAction"]->isOn(Users->toUserListElements()))
		KaduActions["underlineAction"]->setOn(Users->toUserListElements(), Edit->underline());
	if (Edit->color() != actcolor)
	{
		for (i = 0; i < 16; ++i)
			if (Edit->color() == QColor(colors[i]))
				break;
		QPixmap p(12, 12);
		if (i >= 16)
			actcolor = Edit->paletteForegroundColor();
		else
			actcolor = colors[i];
		p.fill(actcolor);
		KaduActions["colorAction"]->setPixmaps(Users->toUserListElements(), p);
	}
	kdebugf2();
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
		Edit->insert(QString("[IMAGE ") + selectedFile + "]");
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
			title.append(parse(config_file.readEntry("Look","ConferenceContents"), *user, false));
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
				title = parse(tr("Chat with ")+"%a", user, false);
			else
				title = parse(tr("Chat with ")+"%a (%s[: %d])", user, false);
		}
		else
			title = parse(config_file.readEntry("Look","ChatContents"), user, false);
		setIcon(user.status("Gadu").pixmap());
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	setCaption(title);
	title_buffer = title;
	kdebugf2();
}

void Chat::changeTitle()
{
	if(!isActiveWindow())
	{
		if (caption() != title_buffer)
			setCaption(title_buffer);
		else
			setCaption(QString().fill(' ', title_buffer.length()));
		title_timer->start(500,TRUE);
	}
}

void Chat::windowActivationChange(bool)
{
	if (isActiveWindow() && title_timer->isActive())
	{
		title_timer->stop();
		setCaption(title_buffer);
	}
}

void Chat::keyPressEvent(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
		clearChatWindow();
	else if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
		close();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_viewhistory"))
		KaduActions["showHistoryAction"]->activate(Users);
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
		KaduActions["whoisAction"]->activate(Users);
	QWidget::keyPressEvent(e);
}

void Chat::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	Edit->setFocus();
	QWidget::mouseReleaseEvent(e);
}

/* convert special characters into emoticons, HTML into plain text and so forth */
QString Chat::convertCharacters(QString edit, const QColor &bgcolor, EmoticonsStyle style)
{
	// zmieniamy windowsowe \r\n na unixowe \n
	edit.replace("\r\n", "<br/>");
	edit.replace("\n",   "<br/>");

	HtmlDocument doc;
	doc.parseHtml(edit);

	// detekcja adresow url
	doc.convertUrlsToHtml();

	if (style!=EMOTS_NONE)
	{
		body->mimeSourceFactory()->addFilePath(emoticons->themePath());
		emoticons->expandEmoticons(doc, bgcolor, style);
	}

	GaduImagesManager::setBackgroundsForAnimatedImages(doc, bgcolor);
	edit=doc.generateHtml();
	return edit;
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
	QWidget::closeEvent(e);
}

void Chat::formatMessages(QValueList<ChatMessage *> &msgs)
{
	QColor myBgColor=config_file.readColorEntry("Look", "ChatMyBgColor");
	QColor usrBgColor=config_file.readColorEntry("Look", "ChatUsrBgColor");
	QColor myFontColor=config_file.readColorEntry("Look", "ChatMyFontColor");
	QColor usrFontColor=config_file.readColorEntry("Look", "ChatUsrFontColor");
	EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle");
	FOREACH(msg, msgs)
		formatMessage(**msg, myBgColor, usrBgColor, myFontColor, usrFontColor, style);
}

void Chat::formatMessage(ChatMessage &msg, QColor myBgColor, QColor usrBgColor, QColor myFontColor, QColor usrFontColor, EmoticonsStyle style)
{
	const static QString formatStringFull("<p style=\"background-color: %1\"><img title=\"\" height=\"%5\" width=\"10000\" align=\"right\"><font color=\"%2\"><b>%3 :: %4</b><br/>%6</font></p>");
	const static QString formatStringPure("<p style=\"background-color: %1\"><img title=\"\" height=\"%3\" width=\"10000\" align=\"right\"><font color=\"%2\">%4</font></p>");
	const static QString formatStringWithoutSeparator("<p style=\"background-color: %1\"><font color=\"%2\">%3</font></p>");

	if (msg.isMyMessage)
	{
		if (myBgColor.isValid())
			msg.backgroundColor=myBgColor;
		else
			msg.backgroundColor=config_file.readColorEntry("Look","ChatMyBgColor");

		if (myFontColor.isValid())
			msg.textColor=myFontColor;
		else
			msg.textColor=config_file.readColorEntry("Look","ChatMyFontColor");
	}
	else
	{
		if (usrBgColor.isValid())
			msg.backgroundColor=usrBgColor;
		else
			msg.backgroundColor=config_file.readColorEntry("Look","ChatUsrBgColor");

		if (usrFontColor.isValid())
			msg.textColor=usrFontColor;
		else
			msg.textColor=config_file.readColorEntry("Look","ChatUsrFontColor");
	}

	QString date=printDateTime(msg.date);

	// ilo¶æ minut od 1970 roku
	time_t CurTime = msg.date.toTime_t() / 60;

	if (!msg.sdate.isNull())
		date.append(" / S "+printDateTime(msg.sdate));

	QString nick = msg.nick;
	HtmlDocument::escapeText(nick);

	if (CfgNoHeaderRepeat && (CurTime - LastTime <= CfgNoHeaderInterval))
	{
		if (PreviousMessage == msg.nick)
		{
			if (CfgHeaderSeparatorHeight > 0)
			{
				msg.message = narg(formatStringPure,
				msg.backgroundColor.name(),
				msg.textColor.name(),
				QString::number(CfgHeaderSeparatorHeight),
				convertCharacters(msg.unformattedMessage, msg.backgroundColor, style));
			}
			else
			{
				msg.message = narg(formatStringWithoutSeparator,
					msg.backgroundColor.name(),
					msg.textColor.name(),
					convertCharacters(msg.unformattedMessage, msg.backgroundColor, style));
			}
		}
		else
		{
			msg.message = narg(formatStringFull,
				msg.backgroundColor.name(),
				msg.textColor.name(),
				nick,
				date,
				QString::number(ParagraphSeparator),
				convertCharacters(msg.unformattedMessage, msg.backgroundColor, style));
		}
	}
	else
	{
		msg.message = narg(formatStringFull,
			msg.backgroundColor.name(),
			msg.textColor.name(),
			nick,
			date,
			QString::number(ParagraphSeparator),
			convertCharacters(msg.unformattedMessage, msg.backgroundColor, style));
	}

	msg.needsToBeFormatted = false;

	PreviousMessage = msg.nick;
	LastTime = CurTime;
}

void Chat::repaintMessages()
{
	kdebugf();
	body->viewport()->setUpdatesEnabled(false);

	QString text;
	int i;
	if (config_file.readBoolEntry("Chat","ScrollDown"))
	{
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
		CONST_FOREACH(msg, ChatMessages)
			body->setParagraphBackgroundColor(i++, (*msg)->backgroundColor);

		if (!ScrollLocked)
			body->scrollToBottom();
	}
	else
	{
		QValueList<ChatMessage *>::const_iterator it=ChatMessages.begin();
		if (it!=ChatMessages.end())
		{
			QString msg=(*it)->message;
			msg.remove(QRegExp("<img title=\"\" height=\"[0-9]*\" width=\"10000\" align=\"right\">"));
			text+=msg;
			++it;
		}
		for(; it!=ChatMessages.end(); ++it)
			text=(*it)->message+text;
		body->setText(text);

		i=ChatMessages.size()-1;
		CONST_FOREACH(msg, ChatMessages)
			body->setParagraphBackgroundColor(i--, (*msg)->backgroundColor);
	}

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

void Chat::writeMessagesFromHistory(UserListElements senders, time_t time)
{
	kdebugf();
	QValueList<HistoryEntry> entries;
	QValueList<HistoryEntry> entriestmp;
	QDateTime date;
	unsigned int from, end, count;

	date.setTime_t(time);

	UinsList uins;//TODO: throw out UinsList as soon as possible!
	CONST_FOREACH(user, senders)
		uins.append((*user).ID("Gadu").toUInt());

	count = history.getHistoryEntriesCount(uins);
	end = count - 1;

	from = count;
	unsigned int chatHistoryQuotation=config_file.readUnsignedNumEntry("History", "ChatHistoryCitation");
	while (from >= 1 && entries.count() < chatHistoryQuotation)
	{
		if (end < chatHistoryQuotation)
			from = 0;
		else
			from = end - chatHistoryQuotation + 1;

		entriestmp = history.getHistoryEntries(uins, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebugmf(KDEBUG_INFO, "temp entries = %d\n", entriestmp.count());
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
		kdebugmf(KDEBUG_INFO, "entries = %d\n", entries.count());
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
	formatMessages(messages);
	if (!messages.empty())
		scrollMessages(messages);
	kdebugf2();
}

/* invoked from outside when new message arrives, this is the window to the world */
void Chat::newMessage(const QString &protocolName, UserListElements senders, const QString &msg, time_t time)
{
	QValueList<ChatMessage *> messages;
	QDateTime date;
	date.setTime_t(time);

	ChatMessage *message = new ChatMessage(senders[0].altNick(), msg, false, QDateTime::currentDateTime(), date);
	formatMessage(*message);
	messages.append(message);

	scrollMessages(messages);
}

void Chat::alertNewMessage()
{
	if (config_file.readBoolEntry("Chat","BlinkChatTitle"))
		if (!isActiveWindow() && !title_timer->isActive())
			changeTitle();
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

void Chat::addMyMessageToHistory()
{
	UinsList uins;
	CONST_FOREACH(user, *Users)
		uins.append((*user).ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	history.addMyMessage(uins, myLastMessage);
}

void Chat::clearChatWindow()
{
	kdebugf();
	if (MessageBox::ask(tr("Chat window will be cleared. Continue?")))
	{
		FOREACH(msg, ChatMessages)
			delete *msg;
		ChatMessages.clear();
		body->clear();
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
	addMyMessageToHistory();
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

	if (gadu->currentStatus().isOffline())
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

	if (myLastFormatsLength)
		seq = gadu->sendMessageRichText(Users->toUserListElements(), msg, (unsigned char *)myLastFormats, myLastFormatsLength);
	else
		seq = gadu->sendMessage(Users->toUserListElements(), msg);

	if (myLastFormats)
		delete [](char *)myLastFormats;

 	if (config_file.readBoolEntry("Chat","MessageAcks"))
		connectAcknowledgeSlots();
	else
	{
		writeMyMessage();
		addMyMessageToHistory();
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
